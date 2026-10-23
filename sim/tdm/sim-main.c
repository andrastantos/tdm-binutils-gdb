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

/* The CPU core, including the parts the hardware implements on the CPU
   itself: the MMU and the interrupt controller.  The reference for the
   behavior is the RTL model of the TDM project (rtl/cpu.py).

   Every memory operand is read, then written back: with the result if
   memory is the destination, with the value read otherwise.  Devices
   see both accesses.  The instruction fetch is written back as well.  */

/* This must come before any other includes.  */
#include "defs.h"

#include <string.h>

#include "sim-main.h"
#include "sim-signal.h"
#include "opcode/tdm.h"
#include "sim/sim-tdm.h"

#include "tdm-sim.h"

#define TDM_PAGE(addr)		(((addr) >> 9) & 0x7f)

/* Instructions take 5 clock cycles, SWAP takes 6.  */
#define TDM_CYCLES(opcode)	((opcode) == TDM_OP_SWAP ? 6 : 5)

static uint16_t
translate (struct tdm_sim_cpu *tc, uint16_t addr)
{
  return (((TDM_PAGE (addr) + tc->mmu_offset) & 0x7f) << 9) | (addr & 0x1ff);
}

/* Read a word from a physical address.  */

static uint16_t
bus_read (SIM_CPU *cpu, uint16_t addr)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);

  switch (addr)
    {
    case TDM_MMU_ADDR:
      /* Write only, nothing drives the (pulled up) bus.  */
      return 0xffff;
    case TDM_INTC_ADDR:
      /* The enable mask comes back in the upper byte, so the write-back
	 of a read keeps it, and acknowledges what was read.  */
      return (tc->ic_enable << 8) | tc->ic_pending;
    default:
      return sim_core_read_aligned_2 (cpu, tc->insn_addr, read_map,
				      TDM_OCTET_ADDR (addr));
    }
}

/* Write a word to a physical address.  */

static void
bus_write (SIM_CPU *cpu, uint16_t addr, uint16_t value)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);

  switch (addr)
    {
    case TDM_MMU_ADDR:
      tc->mmu_offset = (value >> 9) & 0x7f;
      tc->mmu_limit = (value >> 2) & 0x7f;
      tc->mmu_user = (value >> 1) & 1;
      if (value & 1)
	tc->av = false;
      break;
    case TDM_INTC_ADDR:
      tc->ic_enable = value >> 8;
      tc->ic_pending &= ~value;
      break;
    default:
      sim_core_write_aligned_2 (cpu, tc->insn_addr, write_map,
				TDM_OCTET_ADDR (addr), value);
      break;
    }
}

/* Take an interrupt: the hardware replaces the instruction with
   SWAPI $pc, [1], with interrupts disabled and without SWAPI's INTDIS
   flip.  The saved PC is the instruction that didn't get to execute.  */

static void
take_interrupt (SIM_CPU *cpu)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);
  uint16_t handler;

  tc->intdis = tc->intdis_irq = true;
  handler = bus_read (cpu, 1);
  bus_write (cpu, 1, tc->regs[TDM_PC_REGNUM]);
  tc->regs[TDM_PC_REGNUM] = handler;
  tc->cycles += TDM_CYCLES (TDM_OP_SWAP);
}

/* Cancel the instruction because of an access violation.  Nothing it
   would do happens; execution continues with the next address (which
   the interrupt handler will see as the saved PC).  */

static void
cancel_insn (struct tdm_sim_cpu *tc, int cycles)
{
  tc->av = true;
  tc->regs[TDM_PC_REGNUM] = tc->insn_addr + 1;
  tc->cycles += cycles;
}

static void
execute (SIM_CPU *cpu)
{
  SIM_DESC sd = CPU_STATE (cpu);
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);
  uint16_t pc = tc->insn_addr;
  /* The MMU is active in user mode, which needs interrupts enabled.  */
  bool user = tc->mmu_user && !tc->intdis;
  uint16_t iw, a, b, ea, phys = 0, mem = 0, mem_value = 0, result = 0;
  unsigned int opcode, d, opb, opa;
  bool is_mem, write_reg = false, skip = false;

  /* A faulting fetch executes nothing: the hardware loads a cancelled
     SWAPI instead of the undriven bus.  Once an access violation
     happened, the same goes for every fetch in user mode until the
     handler is entered.  */
  if (user && (tc->av || TDM_PAGE (pc) > tc->mmu_limit))
    {
      cancel_insn (tc, TDM_CYCLES (TDM_OP_SWAP));
      return;
    }

  phys = user ? translate (tc, pc) : pc;
  if (TRACE_DISASM_P (cpu))
    {
      /* The prefix shows the (logical) PC; the disassembler reads the
	 physical address, so that's what PC-relative targets are shown
	 relative to.  */
      trace_prefix (sd, cpu, NULL_CIA, pc, 0, NULL, 0, "%s", "");
      trace_disasm (sd, cpu, phys);
    }
  iw = bus_read (cpu, phys);
  bus_write (cpu, phys, iw);

  opcode = TDM_INSN_OPCODE (iw);
  d = TDM_INSN_D (iw);
  opb = TDM_INSN_OPB (iw);
  opa = TDM_INSN_OPA (iw);
  is_mem = !TDM_OPB_IS_IMM (opb);

  /* Breakpoint: stop with the PC on it (it's not executed).  */
  if (TDM_INSN_IS_BRK (iw))
    sim_engine_halt (sd, cpu, NULL, pc, sim_stopped, SIM_SIGTRAP);

  /* Opcode 4 is unused, and swapping two registers isn't supported.  */
  if (opcode == TDM_OP_UNUSED || (opcode == TDM_OP_SWAP && !is_mem))
    sim_engine_halt (sd, cpu, NULL, pc, sim_signalled, SIM_SIGILL);

  /* Operand B: a register (the PC is the address of this instruction)
     plus the immediate, or the memory location at that address.  */
  ea = TDM_INSN_IMMED (iw);
  if (TDM_OPB_BASE (opb) == TDM_OPB_BASE_PC)
    ea += pc;
  else if (TDM_OPB_BASE (opb) != TDM_OPB_BASE_NONE)
    ea += tc->regs[TDM_OPB_BASE (opb)];
  if (is_mem)
    {
      if (user && TDM_PAGE (ea) > tc->mmu_limit)
	{
	  cancel_insn (tc, TDM_CYCLES (opcode));
	  return;
	}
      phys = user ? translate (tc, ea) : ea;
      mem = mem_value = bus_read (cpu, phys);
      b = mem;
    }
  else
    b = ea;

  a = opa == TDM_PC_REGNUM ? pc : tc->regs[opa];

  switch (opcode)
    {
    case TDM_OP_SWAP:
      /* The register gets the memory value, memory gets the register;
	 the PC is stored incremented, i.e. as a return address.  */
      result = mem;
      write_reg = true;
      mem_value = a + (opa == TDM_PC_REGNUM);
      /* SWAPI flips INTDIS, except in user mode where it's a SWAP.  */
      if (d == 0 && !user)
	tc->intdis = !tc->intdis;
      break;

    case TDM_OP_OR:
    case TDM_OP_AND:
    case TDM_OP_XOR:
    case TDM_OP_ADD:
    case TDM_OP_SUB:
    case TDM_OP_ISUB:
      switch (opcode)
	{
	case TDM_OP_OR:   result = a | b; break;
	case TDM_OP_AND:  result = a & b; break;
	case TDM_OP_XOR:  result = a ^ b; break;
	case TDM_OP_ADD:  result = a + b; break;
	case TDM_OP_SUB:  result = a - b; break;
	default:          result = b - a; break;
	}
      if (d)
	mem_value = result;
      else
	write_reg = true;
      break;

    case TDM_OP_MOV:
      if (d)
	mem_value = a;
      else
	{
	  result = b;
	  write_reg = true;
	}
      break;

    case TDM_OP_ISTAT:
      result = (tc->intdis << 1) | tc->irq;
      if (d)
	mem_value = result;
      else
	write_reg = true;
      break;

    case TDM_OP_ROR:
    case TDM_OP_ROL:
      /* Rotate the register, or the memory location.  */
      result = d ? mem : a;
      if (opcode == TDM_OP_ROR)
	result = (result >> 1) | (result << 15);
      else
	result = (result << 1) | (result >> 15);
      if (d)
	mem_value = result;
      else
	write_reg = true;
      break;

    default:
      {
	/* Predicates: execute the next instruction only if the condition
	   holds, D selects the inverted condition.  */
	bool cond;

	switch (opcode)
	  {
	  case TDM_OP_EQ:  cond = a == b; break;
	  case TDM_OP_LTU: cond = a < b; break;
	  case TDM_OP_LTS: cond = (int16_t) a < (int16_t) b; break;
	  default:         cond = (int16_t) a <= (int16_t) b; break;
	  }
	skip = cond == d;
      }
      break;
    }

  /* Memory operands are always written back, so is a memory result
     (writes with an immediate operand B go nowhere).  */
  if (is_mem)
    bus_write (cpu, phys, mem_value);

  if (write_reg && opa == TDM_PC_REGNUM)
    tc->regs[TDM_PC_REGNUM] = result;
  else
    {
      if (write_reg)
	tc->regs[opa] = result;
      tc->regs[TDM_PC_REGNUM] = pc + 1 + skip;
    }
  tc->cycles += TDM_CYCLES (opcode);
}

void
tdm_step_once (SIM_CPU *cpu)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);
  bool serve = tc->irq && !tc->intdis_irq;

  tc->intdis_irq = tc->intdis;
  tc->insn_addr = tc->regs[TDM_PC_REGNUM];
  if (serve)
    take_interrupt (cpu);
  else
    execute (cpu);
  tc->insns++;

  /* Pending bits are sticky, set while their source is active.  The
     request is latched at the end of the instruction.  */
  tc->ic_pending |= tc->ext_irq | tc->av;
  tc->irq = (tc->ic_pending & tc->ic_enable) != 0;
}

/* Drive external interrupt line LINE (1..7) to LEVEL.  */

void
tdm_set_irq_line (SIM_CPU *cpu, int line, int level)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);
  uint8_t bit = 1 << line;

  if (level)
    {
      tc->ext_irq |= bit;
      tc->ic_pending |= bit;
    }
  else
    tc->ext_irq &= ~bit;
}

/* Registers, as numbered in sim/sim-tdm.h.  */

static int
tdm_reg_fetch (SIM_CPU *cpu, int rn, void *buf, int length)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);
  unsigned char *p = buf;
  uint64_t value;
  int size = rn == TDM_SIM_CYCLES_REGNUM ? 8 : 2;
  int i;

  if (length != size)
    return 0;
  switch (rn)
    {
    case TDM_SIM_PC_REGNUM:
    case TDM_SIM_SP_REGNUM:
    case TDM_SIM_R0_REGNUM:
    case TDM_SIM_R1_REGNUM:	   value = tc->regs[rn]; break;
    case TDM_SIM_INTDIS_REGNUM:	   value = tc->intdis; break;
    case TDM_SIM_MMU_OFFSET_REGNUM: value = tc->mmu_offset; break;
    case TDM_SIM_MMU_LIMIT_REGNUM:  value = tc->mmu_limit; break;
    case TDM_SIM_MMU_USER_REGNUM:   value = tc->mmu_user; break;
    case TDM_SIM_MMU_AV_REGNUM:	   value = tc->av; break;
    case TDM_SIM_INT_ENABLE_REGNUM: value = tc->ic_enable; break;
    case TDM_SIM_INT_PENDING_REGNUM: value = tc->ic_pending; break;
    case TDM_SIM_CYCLES_REGNUM:	   value = tc->cycles; break;
    default:
      return 0;
    }
  for (i = 0; i < size; i++)
    p[i] = value >> (8 * i);
  return size;
}

static int
tdm_reg_store (SIM_CPU *cpu, int rn, const void *buf, int length)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);
  const unsigned char *p = buf;
  uint64_t value = 0;
  int size = rn == TDM_SIM_CYCLES_REGNUM ? 8 : 2;
  int i;

  if (length != size)
    return 0;
  for (i = size - 1; i >= 0; i--)
    value = (value << 8) | p[i];
  switch (rn)
    {
    case TDM_SIM_PC_REGNUM:
    case TDM_SIM_SP_REGNUM:
    case TDM_SIM_R0_REGNUM:
    case TDM_SIM_R1_REGNUM:	   tc->regs[rn] = value; break;
    /* A debugger write takes effect right away, for the interrupt check
       as well.  */
    case TDM_SIM_INTDIS_REGNUM:	   tc->intdis = tc->intdis_irq = value & 1; break;
    case TDM_SIM_MMU_OFFSET_REGNUM: tc->mmu_offset = value & 0x7f; break;
    case TDM_SIM_MMU_LIMIT_REGNUM:  tc->mmu_limit = value & 0x7f; break;
    case TDM_SIM_MMU_USER_REGNUM:   tc->mmu_user = value & 1; break;
    case TDM_SIM_MMU_AV_REGNUM:	   tc->av = value & 1; break;
    case TDM_SIM_INT_ENABLE_REGNUM: tc->ic_enable = value; break;
    case TDM_SIM_INT_PENDING_REGNUM: tc->ic_pending = value; break;
    case TDM_SIM_CYCLES_REGNUM:	   tc->cycles = value; break;
    default:
      return 0;
    }
  return size;
}

static sim_cia
tdm_pc_get (SIM_CPU *cpu)
{
  return TDM_SIM_CPU (cpu)->regs[TDM_PC_REGNUM];
}

static void
tdm_pc_set (SIM_CPU *cpu, sim_cia pc)
{
  TDM_SIM_CPU (cpu)->regs[TDM_PC_REGNUM] = pc;
}

/* Reset.  The hardware leaves the MMU and the interrupt controller
   undefined; the simulator starts with everything off and clear.  */

void
tdm_initialize_cpu (SIM_DESC sd, SIM_CPU *cpu)
{
  struct tdm_sim_cpu *tc = TDM_SIM_CPU (cpu);

  memset (tc, 0, sizeof (*tc));
  tc->intdis = tc->intdis_irq = true;

  CPU_REG_FETCH (cpu) = tdm_reg_fetch;
  CPU_REG_STORE (cpu) = tdm_reg_store;
  CPU_PC_FETCH (cpu) = tdm_pc_get;
  CPU_PC_STORE (cpu) = tdm_pc_set;
}
