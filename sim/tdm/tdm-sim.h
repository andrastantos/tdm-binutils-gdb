/* Simulator for TDM (The Disintegrated Machine).
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is part of simulators.

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

#ifndef TDM_SIM_H
#define TDM_SIM_H

/* TDM is word addressed.  The sim core is octet addressed, so word N
   lives at octet address 2 * N, the same as in ELF section headers.  */
#define TDM_OCTET_ADDR(word_addr) ((address_word) (word_addr) * 2)

/* Addresses decoded by the CPU itself, before the bus.  */
#define TDM_INTC_ADDR		0xfffe	/* Interrupt controller.  */
#define TDM_MMU_ADDR		0xffff	/* MMU control register (write only).  */

/* Default location of the simulator's stdio/exit device.  */
#define TDM_STDIO_ADDR		0xffe0

/* Default memory: 32K words of core, all that C data can address.  */
#define TDM_DEFAULT_MEM_WORDS	0x8000

/* Register numbers, as used by the sim register interface.  */
enum tdm_regnum
{
  TDM_PC_REGNUM,
  TDM_SP_REGNUM,
  TDM_R0_REGNUM,
  TDM_R1_REGNUM,
  TDM_NUM_REGS
};

struct tdm_sim_cpu
{
  /* PC, SP, R0, R1, indexed by enum tdm_regnum (which matches the OPA
     encoding).  pc holds the address of the next instruction.  */
  uint16_t regs[TDM_NUM_REGS];

  /* Interrupts disabled.  intdis_irq is the value the interrupt check
     sees: in the hardware a change made by an instruction only reaches
     it one instruction later.  */
  bool intdis;
  bool intdis_irq;
  /* Interrupt request, latched at the end of each instruction.  */
  bool irq;

  /* MMU control register and access violation sticky bit.  */
  uint8_t mmu_offset;
  uint8_t mmu_limit;
  bool mmu_user;
  bool av;

  /* Interrupt controller.  Source 0 is the MMU's access violation,
     sources 1..7 are external lines (driven by hw devices).  */
  uint8_t ic_enable;
  uint8_t ic_pending;
  uint8_t ext_irq;

  /* Address of the instruction being executed, for diagnostics.  */
  uint16_t insn_addr;
  uint64_t cycles;
  uint64_t insns;
};

#define TDM_SIM_CPU(cpu) ((struct tdm_sim_cpu *) CPU_ARCH_DATA (cpu))

extern void tdm_step_once (SIM_CPU *);
extern void tdm_initialize_cpu (SIM_DESC, SIM_CPU *);
extern void tdm_set_irq_line (SIM_CPU *, int line, int level);

#endif
