/* tc-tdm.h -- Header file for tc-tdm.c.
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

   You should have received a copy of the GNU General Public License along
   with GAS; see the file COPYING.  If not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#define TC_TDM 1
#define TARGET_BYTES_BIG_ENDIAN 0
#define WORKING_DOT_WORD

#define TARGET_FORMAT "elf32-tdm"
#define TARGET_ARCH bfd_arch_tdm

/* TDM is word addressed: a target byte is two octets.  */
#define OCTETS_PER_BYTE_POWER 1

/* Make the alignment directives count 16-bit words.  do_align takes
   an octet alignment power and an octet count for the maximum skip.  */
#define md_do_align(N, FILL, LEN, MAX, LABEL)	\
  do						\
    {						\
      (N) += OCTETS_PER_BYTE_POWER;		\
      (MAX) *= OCTETS_PER_BYTE;			\
    }						\
  while (0)

/* '$' starts a register name, it is never part of a symbol.  */
#define LEX_DOLLAR 0

/* Allow "sym - ." to become a PC-relative relocation.  */
#define DIFF_EXPR_OK

#define md_undefined_symbol(NAME)           0
#define md_operand(X)

/* These macros must be defined, but is will be a fatal assembler
   error if we ever hit them.  */
#define md_estimate_size_before_relax(A, B) (as_fatal (_("estimate size\n")), 0)
#define md_convert_frag(B, S, F)            as_fatal (_("convert_frag\n"))

#define md_section_align(SEGMENT, SIZE)     (SIZE)

/* Octet data (.octet, .ascii) is packed; the word it ends in is padded
   when anything else follows.  See tc-tdm.c.  */
#define md_flush_pending_output()	tdm_pad_octet ()
#define tc_frob_label(SYM)		tdm_frob_label (SYM)
extern void tdm_pad_octet (void);
extern void tdm_frob_label (symbolS *);

/* Pad the last word of each subsection, except in the debug sections,
   which are addressed in octets.  */
#define SUB_SEGMENT_ALIGN(SEG, FRCHAIN) \
  ((bfd_section_flags (SEG) & SEC_OCTETS) != 0 ? 0 : OCTETS_PER_BYTE_POWER)

/* %bptr(sym) byte pointers in data.  */
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) \
  tdm_parse_cons_expression (EXP, NBYTES)
extern bfd_reloc_code_real_type tdm_parse_cons_expression (expressionS *,
							   int);
#define tc_fix_adjustable(FIX)		tdm_fix_adjustable (FIX)
extern bool tdm_fix_adjustable (struct fix *);

#define MD_PCREL_FROM_SECTION(FIX, SEC)     md_pcrel_from_section (FIX, SEC)
extern long md_pcrel_from_section (struct fix *, segT);
