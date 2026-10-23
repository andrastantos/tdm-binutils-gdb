/* Disassemble TDM (The Disintegrated Machine) instructions.
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "opcode/tdm.h"
#include "disassemble.h"

/* TDM bytes are 16 bits wide; the disassembler works in octets.  */
#define TDM_INSN_OCTETS 2

static const char *const reg_names[4] = { "$pc", "$sp", "$r0", "$r1" };

/* Mnemonics indexed by opcode.  Where D changes the meaning (SWAP and
   the predicates) the second column holds the D=1 spelling.  */
static const char *const mnemonics[16][2] =
{
  { "swapi",  "swap" },
  { "or",     "or" },
  { "and",    "and" },
  { "xor",    "xor" },
  { NULL,     NULL },
  { "add",    "add" },
  /* With a memory destination the operands are written swapped, so
     SUB (reg - mem) becomes "isub [mem], reg" and vice versa.  */
  { "sub",    "isub" },
  { "isub",   "sub" },
  { "mov",    "mov" },
  { "istat",  "istat" },
  { "ror",    "ror" },
  { "rol",    "rol" },
  { "if_eq",  "if_neq" },
  { "if_ltu", "if_geu" },
  { "if_lts", "if_ges" },
  { "if_les", "if_gts" },
};

static void
print_opa (disassemble_info *info, unsigned int opa)
{
  info->fprintf_styled_func (info->stream, dis_style_register, "%s",
			     reg_names[opa]);
}

static void
print_opb (disassemble_info *info, unsigned int opb, int immed)
{
  bool is_mem = !TDM_OPB_IS_IMM (opb);
  unsigned int base = TDM_OPB_BASE (opb);

  if (is_mem)
    info->fprintf_styled_func (info->stream, dis_style_text, "[");
  if (base == TDM_OPB_BASE_NONE)
    info->fprintf_styled_func (info->stream,
			       is_mem ? dis_style_address_offset
			       : dis_style_immediate, "%d", immed);
  else
    {
      info->fprintf_styled_func (info->stream, dis_style_register, "%s",
				 reg_names[base]);
      if (immed != 0)
	info->fprintf_styled_func (info->stream,
				   is_mem ? dis_style_address_offset
				   : dis_style_immediate, "%+d", immed);
    }
  if (is_mem)
    info->fprintf_styled_func (info->stream, dis_style_text, "]");
}

int
print_insn_tdm (bfd_vma addr, struct disassemble_info *info)
{
  bfd_byte buffer[TDM_INSN_OCTETS];
  unsigned int iword, opcode, d, opb, opa;
  int immed, status;
  const char *name;

  info->bytes_per_chunk = TDM_INSN_OCTETS;
  info->bytes_per_line = TDM_INSN_OCTETS;
  info->display_endian = info->endian;

  status = info->read_memory_func (addr, buffer, TDM_INSN_OCTETS, info);
  if (status != 0)
    {
      info->memory_error_func (status, addr, info);
      return -1;
    }
  iword = bfd_getl16 (buffer);

  opcode = TDM_INSN_OPCODE (iword);
  d = TDM_INSN_D (iword);
  opb = TDM_INSN_OPB (iword);
  opa = TDM_INSN_OPA (iword);
  immed = TDM_INSN_IMMED (iword);

  if (TDM_INSN_IS_BRK (iword))
    {
      info->fprintf_styled_func (info->stream, dis_style_mnemonic, "brk");
      if (TDM_BRK_NUM (iword) != 0)
	{
	  info->fprintf_styled_func (info->stream, dis_style_text, "\t");
	  info->fprintf_styled_func (info->stream, dis_style_immediate, "%d",
				     TDM_BRK_NUM (iword));
	}
      return TDM_INSN_OCTETS;
    }

  name = mnemonics[opcode][d];
  if (name == NULL)
    {
      info->fprintf_styled_func (info->stream, dis_style_assembler_directive,
				 ".word");
      info->fprintf_styled_func (info->stream, dis_style_text, "\t");
      info->fprintf_styled_func (info->stream, dis_style_immediate,
				 "0x%04x", iword);
      return TDM_INSN_OCTETS;
    }

  info->fprintf_styled_func (info->stream, dis_style_mnemonic, "%s", name);
  info->fprintf_styled_func (info->stream, dis_style_text, "\t");

  switch (opcode)
    {
    case TDM_OP_ISTAT:
    case TDM_OP_ROR:
    case TDM_OP_ROL:
      /* Single operand: the register or the memory location.  */
      if (d)
	print_opb (info, opb, immed);
      else
	print_opa (info, opa);
      break;

    case TDM_OP_SWAP:
    case TDM_OP_EQ:
    case TDM_OP_LTU:
    case TDM_OP_LTS:
    case TDM_OP_LES:
      /* D doesn't select the destination here.  */
      print_opa (info, opa);
      info->fprintf_styled_func (info->stream, dis_style_text, ", ");
      print_opb (info, opb, immed);
      break;

    default:
      /* The destination is written first.  */
      if (d)
	{
	  print_opb (info, opb, immed);
	  info->fprintf_styled_func (info->stream, dis_style_text, ", ");
	  print_opa (info, opa);
	}
      else
	{
	  print_opa (info, opa);
	  info->fprintf_styled_func (info->stream, dis_style_text, ", ");
	  print_opb (info, opb, immed);
	}
      break;
    }

  /* Show where PC-relative operands point.  The PC holds the address of
     the current instruction while the operand is computed.  */
  if (TDM_OPB_BASE (opb) == TDM_OPB_BASE_PC
      && (d || (opcode != TDM_OP_ISTAT && opcode != TDM_OP_ROR
		&& opcode != TDM_OP_ROL)))
    {
      info->fprintf_styled_func (info->stream, dis_style_comment_start,
				 "\t; ");
      info->print_address_func ((addr + immed) & 0xffff, info);
    }

  return TDM_INSN_OCTETS;
}
