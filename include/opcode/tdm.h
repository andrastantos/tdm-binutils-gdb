/* Definitions for decoding the TDM (The Disintegrated Machine) opcode table.
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is part of GDB, GAS, and the GNU binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef OPCODE_TDM_H
#define OPCODE_TDM_H

/* Every TDM instruction is a single 16-bit word:

   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
   |    OPCODE     | D |    OPB    |  OPA  |         IMMED         |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

   OPA names a register.  OPB is a (memory reference or immediate)
   computed from IMMED and an optional base register.  D selects
   whether the register (0) or the memory location (1) is the
   destination; for predicates it selects the inverted condition.  */

#define TDM_OPCODE_SHIFT	12
#define TDM_OPCODE_MASK		0xf
#define TDM_D_SHIFT		11
#define TDM_D_MASK		0x1
#define TDM_OPB_SHIFT		8
#define TDM_OPB_MASK		0x7
#define TDM_OPA_SHIFT		6
#define TDM_OPA_MASK		0x3
#define TDM_IMMED_SHIFT		0
#define TDM_IMMED_MASK		0x3f
#define TDM_IMMED_MIN		(-32)
#define TDM_IMMED_MAX		31

#define TDM_INSN(opcode, d, opb, opa, immed)		\
  ((((opcode) & TDM_OPCODE_MASK) << TDM_OPCODE_SHIFT)	\
   | (((d) & TDM_D_MASK) << TDM_D_SHIFT)		\
   | (((opb) & TDM_OPB_MASK) << TDM_OPB_SHIFT)		\
   | (((opa) & TDM_OPA_MASK) << TDM_OPA_SHIFT)		\
   | (((immed) & TDM_IMMED_MASK) << TDM_IMMED_SHIFT))

#define TDM_INSN_OPCODE(i)	(((i) >> TDM_OPCODE_SHIFT) & TDM_OPCODE_MASK)
#define TDM_INSN_D(i)		(((i) >> TDM_D_SHIFT) & TDM_D_MASK)
#define TDM_INSN_OPB(i)		(((i) >> TDM_OPB_SHIFT) & TDM_OPB_MASK)
#define TDM_INSN_OPA(i)		(((i) >> TDM_OPA_SHIFT) & TDM_OPA_MASK)
#define TDM_INSN_IMMED(i)	\
  ((int) ((((i) >> TDM_IMMED_SHIFT) & TDM_IMMED_MASK) ^ 0x20) - 0x20)

/* OPA: register operand.  */
#define TDM_OPA_PC		0
#define TDM_OPA_SP		1
#define TDM_OPA_R0		2
#define TDM_OPA_R1		3

/* OPB: bit 2 set means immediate, clear means memory reference.  The
   low two bits select the base register (PC, SP, R0 or none).  */
#define TDM_OPB_MEM_PC		0	/* [$pc+IMMED] */
#define TDM_OPB_MEM_SP		1	/* [$sp+IMMED] */
#define TDM_OPB_MEM_R0		2	/* [$r0+IMMED] */
#define TDM_OPB_MEM		3	/* [IMMED] */
#define TDM_OPB_IMM_PC		4	/* $pc+IMMED */
#define TDM_OPB_IMM_SP		5	/* $sp+IMMED */
#define TDM_OPB_IMM_R0		6	/* $r0+IMMED */
#define TDM_OPB_IMM		7	/* IMMED */
#define TDM_OPB_IS_IMM(opb)	(((opb) & 4) != 0)
#define TDM_OPB_BASE(opb)	((opb) & 3)
#define TDM_OPB_BASE_PC		0
#define TDM_OPB_BASE_SP		1
#define TDM_OPB_BASE_R0		2
#define TDM_OPB_BASE_NONE	3

/* OPCODE values.  */
#define TDM_OP_SWAP		0x0	/* D=0: SWAPI (also flips INTDIS).  */
#define TDM_OP_OR		0x1
#define TDM_OP_AND		0x2
#define TDM_OP_XOR		0x3
#define TDM_OP_UNUSED		0x4
#define TDM_OP_ADD		0x5
#define TDM_OP_SUB		0x6	/* OPA - OPB.  */
#define TDM_OP_ISUB		0x7	/* OPB - OPA.  */
#define TDM_OP_MOV		0x8
#define TDM_OP_ISTAT		0x9
#define TDM_OP_ROR		0xa
#define TDM_OP_ROL		0xb
#define TDM_OP_EQ		0xc	/* D=1: NEQ.  */
#define TDM_OP_LTU		0xd	/* D=1: GEU.  */
#define TDM_OP_LTS		0xe	/* D=1: GES.  */
#define TDM_OP_LES		0xf	/* D=1: GTS.  */

#define TDM_OP_IS_PREDICATE(op)	(((op) & 0xc) == 0xc)

/* Assembler operand forms.  */
enum tdm_operand_form
{
  /* "insn $reg, opb" or "insn [mem], $reg".  */
  TDM_FORM_TWO,
  /* Like TDM_FORM_TWO, but opb has to be a memory reference.  */
  TDM_FORM_TWO_MEM,
  /* "insn $reg" or "insn [mem]".  */
  TDM_FORM_ONE
};

/* How a given operand order is encoded.  D < 0 means that the operand
   order can't be encoded at all.  */
struct tdm_encoding
{
  unsigned char opcode;
  signed char d;
};

typedef struct tdm_opc_info_t
{
  const char *name;
  enum tdm_operand_form form;
  /* Encoding when the register operand is written first.  */
  struct tdm_encoding reg_first;
  /* Encoding when the memory operand is written first.  Swapping the
     operands of a non-commutative operation selects a different
     opcode (sub <-> isub, lts <-> les) rather than a different D.  */
  struct tdm_encoding mem_first;
} tdm_opc_info_t;

extern const tdm_opc_info_t tdm_opc_info[];
extern const unsigned int tdm_num_opcodes;

#endif /* OPCODE_TDM_H */
