/* tc-tdm.c -- Assemble code for TDM (The Disintegrated Machine).
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* The instruction syntax follows the reference Python assembler in the
   TDM project (rtl/asm.py):

     insn $reg, opb		register destination (D=0)
     insn [mem], $reg		memory destination (D=1)
     insn $reg			single operand forms
     insn [mem]

   where opb is one of

     [$pc+e] [$sp+e] [$r0+e] [e]	memory references
     $pc+e   $sp+e   $r0+e   e		immediates

   E is a signed 6-bit value.  The "+e" after a base register may be
   omitted, but if present has to start with '+' or '-'.  The memory
   pseudo-registers $mr0 to $mr31 are aliases of [0] to [31].

   TDM is word addressed.  GAS works in octets internally, so every
   instruction or data word is two octets, but symbol values, '.' and
   relocation addresses are in 16-bit units.  */

#include "as.h"
#include "safe-ctype.h"
#include "dwarf2dbg.h"
#include "subsegs.h"
#include "opcode/tdm.h"
#include "elf/tdm.h"

const char comment_chars[]        = ";";
const char line_separator_chars[] = "";
const char line_comment_chars[]   = "#";

const char FLT_CHARS[] = "rRsSfFdDxXpP";
const char EXP_CHARS[] = "eE";

static htab_t opcode_hash_control;

/* Octet data.  .octet and the string directives emit single octets,
   two per word, the first in the low octet.  Consecutive octet data is
   packed, which lets a C compiler lay out char arrays and packed
   structures.  The last word is padded with a zero octet lazily, when
   anything else (an instruction, a label, other data, alignment, a
   section switch or the end of the section) follows.  */

/* Set while an .octet directive emits its values.  */
static bool tdm_octet_mode;

/* Whether words need padding in the current section.  The debug
   sections are addressed in octets: their data (DWARF written by a
   compiler) puts multi-octet values at any octet.  */

static bool
tdm_word_section_p (void)
{
  return (now_seg != absolute_section
	  && (bfd_section_flags (now_seg) & SEC_OCTETS) == 0);
}

/* The octet in its word that the current frag starts at.  A frag
   doesn't necessarily start at a word: one of fixed size ends where
   its memory filled up, which a long string can do at any octet.
   Count from the start of the subsection, or from the end of a frag
   of another kind, which aligns (to words at least).  The answer for
   the frag last asked about is kept, as the frags mostly grow from
   it.  */

static unsigned int
tdm_frag_start_octet (void)
{
  static fragS *known_frag;
  static unsigned int known_octet;
  fragS *f = known_frag;
  unsigned int octet = known_octet;

  if (f == frag_now)
    return known_octet;
  while (f != NULL && f != frag_now)
    {
      octet = (f->fr_type == rs_fill
	       ? octet + f->fr_fix + f->fr_var * f->fr_offset : 0);
      f = f->fr_next;
    }
  if (f == NULL)
    {
      octet = 0;
      for (f = frchain_now->frch_root; f != frag_now; f = f->fr_next)
	octet = (f->fr_type == rs_fill
		 ? octet + f->fr_fix + f->fr_var * f->fr_offset : 0);
    }
  known_frag = frag_now;
  known_octet = octet % OCTETS_PER_BYTE;
  return known_octet;
}

/* The current octet, counted from the word that the frag starts in.  */

static unsigned int
tdm_frag_octets (void)
{
  return tdm_frag_start_octet () + frag_now_fix_octets ();
}

/* Pad the current word with a zero octet if it is half full.  */

void
tdm_pad_octet (void)
{
  if (tdm_octet_mode || !tdm_word_section_p ())
    return;
  if (tdm_frag_octets () % OCTETS_PER_BYTE != 0)
    FRAG_APPEND_1_CHAR (0);
}

static void
tdm_octet (int ignore ATTRIBUTE_UNUSED)
{
  tdm_octet_mode = true;
  cons (1);
  tdm_octet_mode = false;
}

/* .ascii and friends: as .octet, with a character per octet.  This
   matches the string layout of the reference assembler, except that
   consecutive strings are packed.  */

static void
tdm_stringer (int bits_appendzero)
{
  tdm_octet_mode = true;
  stringer (bits_appendzero);
  tdm_octet_mode = false;
}

/* A label at a half full word would get the address of the word it
   shares with the preceding octet; move it to the next word.  The
   value of a label counts words from the word its frag starts in,
   which differs from what colon gave it in a frag that starts in the
   middle of a word.  This replaces obj_frob_label, so it also records
   the label for DWARF.  */

void
tdm_frob_label (symbolS *sym)
{
  if (tdm_word_section_p () && symbol_get_frag (sym) == frag_now)
    {
      if (tdm_frag_octets () % OCTETS_PER_BYTE != 0)
	FRAG_APPEND_1_CHAR (0);
      symbol_set_frag (sym, frag_now);
      S_SET_VALUE (sym, tdm_frag_octets () / OCTETS_PER_BYTE);
    }
  dwarf2_emit_label (sym);
}

/* Byte pointers.  C data pointers on TDM are byte addresses: twice the
   word address, plus one for the high octet of the word.  In data,
   %bptr(EXPR) is the byte address of the word address EXPR, optionally
   followed by "+ N" or "- N" octets:

     .word %bptr(str) + 3	; the fourth character of STR

   A symbolic EXPR produces an R_TDM_BYTEPTR16 relocation.  */

bfd_reloc_code_real_type
tdm_parse_cons_expression (expressionS *exp, int nbytes)
{
  offsetT octets = 0;

  SKIP_WHITESPACE ();
  if (strncasecmp (input_line_pointer, "%bptr", 5) != 0)
    {
      expression (exp);
      return BFD_RELOC_NONE;
    }
  input_line_pointer += 5;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != '(')
    {
      as_bad (_("missing `(' after %%bptr"));
      while (!is_end_of_stmt (*input_line_pointer))
	++input_line_pointer;
      exp->X_op = O_constant;
      exp->X_add_number = 0;
      return BFD_RELOC_NONE;
    }
  ++input_line_pointer;
  expression (exp);
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ')')
    as_bad (_("missing `)' after %%bptr expression"));
  else
    ++input_line_pointer;

  SKIP_WHITESPACE ();
  if (*input_line_pointer == '+' || *input_line_pointer == '-')
    {
      expressionS off;

      expression (&off);
      if (off.X_op != O_constant)
	as_bad (_("the octet offset after %%bptr(...) must be a constant"));
      else
	octets = off.X_add_number;
    }

  if (nbytes != 2)
    {
      as_bad (_("a byte pointer has to be a 16-bit word"));
      exp->X_op = O_constant;
      exp->X_add_number = 0;
      return BFD_RELOC_NONE;
    }

  if (exp->X_op == O_symbol
      && S_GET_SEGMENT (exp->X_add_symbol) == absolute_section
      && S_IS_DEFINED (exp->X_add_symbol))
    {
      exp->X_op = O_constant;
      exp->X_add_number += S_GET_VALUE (exp->X_add_symbol);
    }

  switch (exp->X_op)
    {
    case O_constant:
      exp->X_add_number = 2 * exp->X_add_number + octets;
      return BFD_RELOC_NONE;

    case O_symbol:
      exp->X_add_number = 2 * exp->X_add_number + octets;
      return BFD_RELOC_TDM_BYTEPTR16;

    default:
      as_bad (_("%%bptr needs a symbol plus a constant"));
      exp->X_op = O_constant;
      exp->X_add_number = 0;
      return BFD_RELOC_NONE;
    }
}

/* The addend of a byte pointer counts octets, while symbol values count
   words, so its symbol can't be replaced by a section symbol.  */

bool
tdm_fix_adjustable (fixS *fixP)
{
  return fixP->fx_r_type != BFD_RELOC_TDM_BYTEPTR16;
}

const pseudo_typeS md_pseudo_table[] =
{
  /* The smallest addressable unit is 16 bits wide, so these count
     words, not octets.  */
  { "byte",	cons,		2 },
  { "space",	s_space,	2 },
  { "skip",	s_space,	2 },
  { "zero",	s_space,	2 },
  { "octet",	tdm_octet,	0 },
  { "ascii",	tdm_stringer,	8 + 0 },
  { "asciz",	tdm_stringer,	8 + 1 },
  { "string",	tdm_stringer,	8 + 1 },
  { NULL,	NULL,		0 }
};

void
md_begin (void)
{
  unsigned int i;

  opcode_hash_control = str_htab_create ();
  for (i = 0; i < tdm_num_opcodes; i++)
    str_hash_insert (opcode_hash_control, tdm_opc_info[i].name,
		     &tdm_opc_info[i], 0);

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, bfd_mach_tdm);
}

/* Return true if V fits the signed 6-bit IMMED field.  Addresses wrap at
   16 bits, so 0xffe0..0xffff are accepted as -32..-1.  */

static bool
tdm_immed_in_range (offsetT v)
{
  if (v < -0x10000 || v > 0xffff)
    return false;
  v &= 0xffff;
  if (v & 0x8000)
    v -= 0x10000;
  return v >= TDM_IMMED_MIN && v <= TDM_IMMED_MAX;
}

static char *
skip_space (char *s)
{
  while (is_whitespace (*s))
    s++;
  return s;
}

/* Parse a register name at *PTR.  Return its OPA number and advance
   *PTR, or return -1 and leave *PTR alone if there's no register.  */

static int
parse_register (char **ptr)
{
  char *s = *ptr;
  int reg;

  if (s[0] != '$')
    return -1;
  if (TOLOWER (s[1]) == 'p' && TOLOWER (s[2]) == 'c')
    reg = TDM_OPA_PC;
  else if (TOLOWER (s[1]) == 's' && TOLOWER (s[2]) == 'p')
    reg = TDM_OPA_SP;
  else if (TOLOWER (s[1]) == 'r' && s[2] == '0')
    reg = TDM_OPA_R0;
  else if (TOLOWER (s[1]) == 'r' && s[2] == '1')
    reg = TDM_OPA_R1;
  else
    return -1;
  if (is_part_of_name (s[3]))
    return -1;

  *ptr = s + 3;
  return reg;
}

/* Return true if S starts with a memory pseudo-register: $mr0 to
   $mr31 are aliases of [0] to [31].  (Negative absolute addresses are
   the I/O page; they have no aliases.)  */

static bool
is_mem_register (const char *s)
{
  int i = 3;

  if (s[0] != '$' || TOLOWER (s[1]) != 'm' || TOLOWER (s[2]) != 'r'
      || !ISDIGIT (s[3]))
    return false;
  while (ISDIGIT (s[i]))
    i++;
  return !is_part_of_name (s[i]);
}

/* Parse the memory pseudo-register at *PTR (see is_mem_register),
   advance *PTR past it and return its number, or -1 if it's out of
   range.  */

static int
parse_mem_register (char **ptr)
{
  char *end;
  long n = strtol (*ptr + 3, &end, 10);

  *ptr = end;
  if (n > TDM_IMMED_MAX)
    {
      as_bad (_("memory pseudo-register $mr%ld is out of range (0..%d)"),
	      n, TDM_IMMED_MAX);
      return -1;
    }
  return n;
}

/* Parse an expression at S, return the first character after it.  */

static char *
parse_expression (char *s, expressionS *exp)
{
  char *save = input_line_pointer;

  input_line_pointer = s;
  expression (exp);
  s = input_line_pointer;
  input_line_pointer = save;

  if (exp->X_op == O_absent || exp->X_op == O_illegal
      || exp->X_op == O_register)
    {
      as_bad (_("bad expression"));
      return NULL;
    }
  return s;
}

/* Parse the base register of OPB along with its optional offset.  The
   offset, if present, has to start with '+' or '-'.  */

static char *
parse_base_and_offset (char *s, int base, int *opb, expressionS *exp,
		       bool is_mem)
{
  if (base == TDM_OPA_R1)
    {
      as_bad (_("$r1 can't be used as a base register"));
      return NULL;
    }
  /* Register numbers for PC, SP and R0 match the OPB base encoding.  */
  *opb = (is_mem ? 0 : 4) | base;

  s = skip_space (s);
  if (*s != '+' && *s != '-')
    {
      exp->X_op = O_constant;
      exp->X_add_number = 0;
      return s;
    }
  return parse_expression (s, exp);
}

/* Parse operand B.  If ALLOW_IMM is false, it has to be a memory
   reference.  Return NULL on error.  */

static char *
parse_opb (char *s, bool allow_imm, int *opb, expressionS *exp)
{
  int base;

  s = skip_space (s);
  if (is_mem_register (s))
    {
      int n = parse_mem_register (&s);

      if (n < 0)
	return NULL;
      *opb = TDM_OPB_MEM;
      exp->X_op = O_constant;
      exp->X_add_number = n;
      return s;
    }
  if (*s == '[')
    {
      s = skip_space (s + 1);
      base = parse_register (&s);
      if (base >= 0)
	s = parse_base_and_offset (s, base, opb, exp, true);
      else
	{
	  *opb = TDM_OPB_MEM;
	  s = parse_expression (s, exp);
	}
      if (s == NULL)
	return NULL;
      s = skip_space (s);
      if (*s != ']')
	{
	  as_bad (_("memory reference is not terminated properly"));
	  return NULL;
	}
      return s + 1;
    }

  if (!allow_imm)
    {
      as_bad (_("expecting a memory operand"));
      return NULL;
    }

  base = parse_register (&s);
  if (base >= 0)
    return parse_base_and_offset (s, base, opb, exp, false);

  *opb = TDM_OPB_IMM;
  return parse_expression (s, exp);
}

static char *
expect_comma (char *s)
{
  s = skip_space (s);
  if (*s != ',')
    {
      as_bad (_("there must be a comma after the first operand"));
      return NULL;
    }
  return s + 1;
}

static char *
expect_register (char *s, int *reg)
{
  s = skip_space (s);
  *reg = parse_register (&s);
  if (*reg < 0)
    {
      as_bad (_("expecting a register"));
      return NULL;
    }
  return s;
}

void
md_assemble (char *str)
{
  char mnemonic[16];
  const tdm_opc_info_t *opcode;
  const struct tdm_encoding *enc;
  expressionS exp;
  unsigned int iword;
  int opa, opb;
  size_t len;
  char *s, *p;

  str = skip_space (str);
  /* The mnemonic ends at the first character that can't be part of a
     name: macro expansion may drop the space in "mov [$sp], $r0".  */
  for (len = 0; is_part_of_name (str[len]); len++)
    if (len < sizeof (mnemonic) - 1)
      mnemonic[len] = TOLOWER (str[len]);
  if (len == 0)
    {
      as_bad (_("can't find opcode"));
      return;
    }
  mnemonic[len < sizeof (mnemonic) ? len : sizeof (mnemonic) - 1] = 0;

  /* The ISA has no NOP: XOR a register with 0.  */
  if (strcmp (mnemonic, "nop") == 0)
    {
      char xor_r0[] = "xor $r0, 0";

      s = skip_space (str + len);
      if (!is_end_of_stmt (*s))
	{
	  as_bad (_("junk at end of line: `%s'"), s);
	  return;
	}
      md_assemble (xor_r0);
      return;
    }

  opcode = len < sizeof (mnemonic)
	   ? str_hash_find (opcode_hash_control, mnemonic) : NULL;
  if (opcode == NULL)
    {
      as_bad (_("unknown opcode `%.*s'"), (int) len, str);
      return;
    }

  s = skip_space (str + len);
  if (opcode->form == TDM_FORM_BRK)
    {
      offsetT num = 0;

      if (!is_end_of_stmt (*s))
	{
	  s = parse_expression (s, &exp);
	  if (s == NULL)
	    return;
	  if (exp.X_op != O_constant
	      || exp.X_add_number < 0 || exp.X_add_number > 0xff)
	    {
	      as_bad (_("breakpoint number must be a constant in 0..255"));
	      return;
	    }
	  num = exp.X_add_number;
	  s = skip_space (s);
	}
      if (!is_end_of_stmt (*s))
	{
	  as_bad (_("junk at end of line: `%s'"), s);
	  return;
	}
      tdm_pad_octet ();
      p = frag_more (2);
      md_number_to_chars (p, TDM_BRK_INSN | num, 2);
      dwarf2_emit_insn (2);
      return;
    }

  if (*s == '[' || is_mem_register (s))
    {
      /* Memory operand first.  */
      enc = &opcode->mem_first;
      s = parse_opb (s, false, &opb, &exp);
      if (s != NULL && opcode->form != TDM_FORM_ONE)
	{
	  s = expect_comma (s);
	  if (s != NULL)
	    s = expect_register (s, &opa);
	}
      else
	/* Single operand instructions with a memory operand don't use
	   OPA.  */
	opa = TDM_OPA_PC;
    }
  else
    {
      /* Register operand first.  */
      enc = &opcode->reg_first;
      s = expect_register (s, &opa);
      if (s != NULL && opcode->form != TDM_FORM_ONE)
	{
	  s = expect_comma (s);
	  if (s != NULL)
	    s = parse_opb (s, opcode->form == TDM_FORM_TWO, &opb, &exp);
	}
      else
	{
	  opb = TDM_OPB_IMM;
	  exp.X_op = O_constant;
	  exp.X_add_number = 0;
	}
    }
  if (s == NULL)
    return;

  s = skip_space (s);
  if (!is_end_of_stmt (*s))
    {
      as_bad (_("junk at end of line: `%s'"), s);
      return;
    }

  if (enc->d < 0)
    {
      as_bad (_("`%s' can't be encoded with the memory operand first: "
		"there are no unsigned <= or > predicates"), opcode->name);
      return;
    }

  iword = TDM_INSN (enc->opcode, enc->d, opb, opa, 0);

  tdm_pad_octet ();
  p = frag_more (2);

  if (exp.X_op == O_constant)
    {
      if (!tdm_immed_in_range (exp.X_add_number))
	as_bad (_("immediate value %" PRId64 " is out of range (%d..%d)"),
		(int64_t) exp.X_add_number, TDM_IMMED_MIN, TDM_IMMED_MAX);
      iword |= exp.X_add_number & TDM_IMMED_MASK;
    }
  else
    fix_new_exp (frag_now, p - frag_now->fr_literal, 2, &exp, false,
		 BFD_RELOC_TDM_IMM6);

  md_number_to_chars (p, iword, 2);
  dwarf2_emit_insn (2);
}

const char *
md_atof (int type, char *litP, int *sizeP)
{
  return ieee_md_atof (type, litP, sizeP, false);
}

const char md_shortopts[] = "";

const struct option md_longopts[] =
{
  { NULL, no_argument, NULL, 0 }
};
const size_t md_longopts_size = sizeof (md_longopts);

int
md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
}

/* Apply a fixup to the object file.  */

void
md_apply_fix (fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  offsetT val = *valP;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_TDM_IMM6:
      /* "sym - ." with SYM in another section ends up here.  */
      if (fixP->fx_pcrel)
	fixP->fx_r_type = BFD_RELOC_TDM_PCREL6;
      /* Fall through.  */
    case BFD_RELOC_TDM_PCREL6:
      if (fixP->fx_addsy == NULL)
	{
	  unsigned int iword = bfd_getl16 (buf);

	  if (!tdm_immed_in_range (val))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("immediate value %" PRId64
			    " is out of range (%d..%d)"),
			  (int64_t) val, TDM_IMMED_MIN, TDM_IMMED_MAX);
	  iword = (iword & ~TDM_IMMED_MASK) | (val & TDM_IMMED_MASK);
	  bfd_putl16 (iword, buf);
	}
      break;

    case BFD_RELOC_TDM_BYTEPTR16:
      /* The value of an absolute symbol would have to be doubled; that
	 is done in tdm_parse_cons_expression, which can't see symbols
	 that only become absolute later.  */
      if (fixP->fx_addsy == NULL)
	as_bad_where (fixP->fx_file, fixP->fx_line,
		      _("%%bptr of a symbol that is only known to be "
			"absolute at the end of assembly"));
      break;

    case BFD_RELOC_8:
    case BFD_RELOC_16:
    case BFD_RELOC_32:
      if (fixP->fx_addsy == NULL)
	md_number_to_chars (buf, val, fixP->fx_size);
      break;

    default:
      abort ();
    }

  if (fixP->fx_addsy == NULL)
    fixP->fx_done = 1;
}

/* Put number into target byte order.  */

void
md_number_to_chars (char *ptr, valueT use, int nbytes)
{
  number_to_chars_littleendian (ptr, use, nbytes);
}

/* Generate a machine-dependent relocation.  */

arelent *
tc_gen_reloc (asection *section, fixS *fixP)
{
  arelent *relP;
  bfd_reloc_code_real_type code = fixP->fx_r_type;

  switch (code)
    {
    case BFD_RELOC_TDM_IMM6:
    case BFD_RELOC_TDM_PCREL6:
      break;

    case BFD_RELOC_TDM_BYTEPTR16:
    case BFD_RELOC_16:
    case BFD_RELOC_32:
      if (!fixP->fx_pcrel)
	break;
      /* Fall through.  */
    default:
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("this type of operand can not be relocated, "
		      "it must be an assembly-time constant"));
      return NULL;
    }

  relP = notes_alloc (sizeof (arelent));
  relP->sym_ptr_ptr = notes_alloc (sizeof (asymbol *));
  *relP->sym_ptr_ptr = symbol_get_bfdsym (fixP->fx_addsy);
  /* Relocation addresses are in 16-bit units, except in sections
     addressed in octets (the DWARF debug sections).  */
  relP->address = fixP->fx_frag->fr_address + fixP->fx_where;
  if ((section->flags & SEC_OCTETS) == 0)
    relP->address /= OCTETS_PER_BYTE;
  relP->addend = fixP->fx_offset;

  relP->howto = bfd_reloc_type_lookup (stdoutput, code);
  if (relP->howto == NULL)
    {
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("cannot generate relocation type %s"),
		    bfd_get_reloc_code_name (code));
      return NULL;
    }

  return relP;
}

/* PC-relative operands are relative to the address of the instruction
   itself, in 16-bit units (in octets in octet addressed sections).  */

long
md_pcrel_from_section (fixS *fixP, segT sec)
{
  long addr = fixP->fx_frag->fr_address + fixP->fx_where;

  if ((sec->flags & SEC_OCTETS) == 0)
    addr /= OCTETS_PER_BYTE;
  return addr;
}
