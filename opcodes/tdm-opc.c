/* tdm-opc.c -- Definitions for TDM (The Disintegrated Machine) opcodes.
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

#define NONE { 0, -1 }

/* The assembler mnemonics.  The disassembler doesn't use this table, as
   it has to pick a single canonical spelling for each encoding.  */

const tdm_opc_info_t tdm_opc_info[] =
{
  /* SWAP writes both operands, so operand order doesn't matter.  D=0
     selects SWAPI, which also flips INTDIS.  */
  { "swap",   TDM_FORM_TWO_MEM, { TDM_OP_SWAP, 1 }, { TDM_OP_SWAP, 1 } },
  { "swapi",  TDM_FORM_TWO_MEM, { TDM_OP_SWAP, 0 }, { TDM_OP_SWAP, 0 } },

  /* D selects the destination.  */
  { "or",     TDM_FORM_TWO, { TDM_OP_OR,  0 }, { TDM_OP_OR,  1 } },
  { "and",    TDM_FORM_TWO, { TDM_OP_AND, 0 }, { TDM_OP_AND, 1 } },
  { "xor",    TDM_FORM_TWO, { TDM_OP_XOR, 0 }, { TDM_OP_XOR, 1 } },
  { "add",    TDM_FORM_TWO, { TDM_OP_ADD, 0 }, { TDM_OP_ADD, 1 } },
  { "mov",    TDM_FORM_TWO, { TDM_OP_MOV, 0 }, { TDM_OP_MOV, 1 } },
  /* SUB computes reg-opb and ISUB opb-reg.  "sub [mem], $reg" means
     mem = mem - reg, which is ISUB with a memory destination.  */
  { "sub",    TDM_FORM_TWO, { TDM_OP_SUB,  0 }, { TDM_OP_ISUB, 1 } },
  { "isub",   TDM_FORM_TWO, { TDM_OP_ISUB, 0 }, { TDM_OP_SUB,  1 } },

  /* Predicates always test "reg <cond> opb", with D selecting the
     inverted condition.  With the memory operand first we need the
     mirrored condition (a < b <=> b > a), which for signed compares
     lives in the other opcode.  There are no unsigned <= or >
     predicates, so the unsigned ones can't be mirrored.  */
  { "if_eq",  TDM_FORM_TWO, { TDM_OP_EQ,  0 }, { TDM_OP_EQ,  0 } },
  { "if_neq", TDM_FORM_TWO, { TDM_OP_EQ,  1 }, { TDM_OP_EQ,  1 } },
  { "if_ltu", TDM_FORM_TWO, { TDM_OP_LTU, 0 }, NONE },
  { "if_geu", TDM_FORM_TWO, { TDM_OP_LTU, 1 }, NONE },
  { "if_lts", TDM_FORM_TWO, { TDM_OP_LTS, 0 }, { TDM_OP_LES, 1 } },
  { "if_ges", TDM_FORM_TWO, { TDM_OP_LTS, 1 }, { TDM_OP_LES, 0 } },
  { "if_les", TDM_FORM_TWO, { TDM_OP_LES, 0 }, { TDM_OP_LTS, 1 } },
  { "if_gts", TDM_FORM_TWO, { TDM_OP_LES, 1 }, { TDM_OP_LTS, 0 } },

  /* Single operand: D selects whether the register or the memory
     location is the source and destination.  */
  { "istat",  TDM_FORM_ONE, { TDM_OP_ISTAT, 0 }, { TDM_OP_ISTAT, 1 } },
  { "ror",    TDM_FORM_ONE, { TDM_OP_ROR,   0 }, { TDM_OP_ROR,   1 } },
  { "rol",    TDM_FORM_ONE, { TDM_OP_ROL,   0 }, { TDM_OP_ROL,   1 } },
};

const unsigned int tdm_num_opcodes = sizeof (tdm_opc_info) / sizeof (tdm_opc_info[0]);
