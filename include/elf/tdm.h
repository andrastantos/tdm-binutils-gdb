/* TDM ELF support for BFD.
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _ELF_TDM_H
#define _ELF_TDM_H

#include "elf/reloc-macros.h"

/* Relocation types.  TDM is word addressed: a byte is 16 bits, and all
   offsets and addresses below are in 16-bit units.  */
START_RELOC_NUMBERS (elf_tdm_reloc_type)
  RELOC_NUMBER (R_TDM_NONE, 0)
  /* A 16-bit data word.  */
  RELOC_NUMBER (R_TDM_16, 1)
  /* A 32-bit data value, low word first.  */
  RELOC_NUMBER (R_TDM_32, 2)
  /* The signed 6-bit IMMED field of an instruction, with 16-bit wrap
     (so 0xffe0..0xffff are encodable as -32..-1).  */
  RELOC_NUMBER (R_TDM_IMM6, 3)
  /* As R_TDM_IMM6, but relative to the address of the instruction.  */
  RELOC_NUMBER (R_TDM_PCREL6, 4)
  /* A 16-bit byte pointer, as used by C: twice the word address of the
     symbol, plus the addend, which counts octets.  */
  RELOC_NUMBER (R_TDM_BYTEPTR16, 5)
END_RELOC_NUMBERS (R_TDM_max)

#endif /* _ELF_TDM_H */
