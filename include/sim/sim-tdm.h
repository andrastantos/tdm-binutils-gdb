/* This file defines the interface between the TDM simulator and GDB.

   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SIM_TDM_H
#define SIM_TDM_H

/* Register numbers.  The first four are the architectural registers, in
   OPA encoding order.  The rest is CPU state the simulator models (the
   MMU and the interrupt controller are part of the CPU), which hardware
   doesn't let software read.  All registers are 16 bits wide except
   TDM_SIM_CYCLES_REGNUM, which is 64 bits.  */

enum tdm_sim_regnum
{
  TDM_SIM_PC_REGNUM,
  TDM_SIM_SP_REGNUM,
  TDM_SIM_R0_REGNUM,
  TDM_SIM_R1_REGNUM,
  TDM_SIM_INTDIS_REGNUM,	/* Interrupts disabled.  */
  TDM_SIM_MMU_OFFSET_REGNUM,	/* MMU offset, in pages.  */
  TDM_SIM_MMU_LIMIT_REGNUM,	/* MMU limit, in pages (inclusive).  */
  TDM_SIM_MMU_USER_REGNUM,	/* MMU user mode bit.  */
  TDM_SIM_MMU_AV_REGNUM,	/* Access violation.  */
  TDM_SIM_INT_ENABLE_REGNUM,	/* Interrupt controller enable mask.  */
  TDM_SIM_INT_PENDING_REGNUM,	/* Interrupt controller pending bits.  */
  TDM_SIM_CYCLES_REGNUM,	/* Clock cycles executed.  */
  TDM_SIM_NUM_REGS
};

#endif /* SIM_TDM_H */
