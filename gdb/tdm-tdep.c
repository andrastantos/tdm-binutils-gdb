/* Target-dependent code for TDM (The Disintegrated Machine), for GDB.

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

/* TDM is word addressed: an address names a 16-bit word, and GDB's
   addressable memory unit is two octets.  Memory lengths passed to
   target functions are in these units.

   Calling convention
   ------------------

   The stack grows down; $sp points to the first free word.  To call a
   function, the caller stores its address in the free word and swaps
   it with the PC, which leaves the return address there:

	mov	[$sp], $r1		; $r1: the address of the callee
	swap	$pc, [$sp]		; [$sp]: the return address

   The callee allocates its frame below the return address, and frees it
   before returning through the return address:

	sub	$sp, N			; return address at [$sp+N]
	...				; locals at [$sp+1]..[$sp+N-1]
	add	$sp, N
	mov	$pc, [$sp]

   The first argument and the return value are in $r0 (a 32-bit value in
   $r1:$r0, low word in $r0); further arguments are pushed by the caller
   before the call, so they sit above the return address.  $r0 and $r1
   are caller saved, $sp is preserved.

   For unwinding, the CFA of a frame is the address of its return
   address: the caller's $sp at the time of the call.  */

#include "extract-store-integer.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "value.h"
#include "inferior.h"
#include "arch-utils.h"
#include "regcache.h"
#include "reggroups.h"
#include "osabi.h"
#include "dis-asm.h"

#include "opcode/tdm.h"
#include "sim/sim-tdm.h"

/* GDB's register numbers are the simulator's.  */
#define TDM_PC_REGNUM	TDM_SIM_PC_REGNUM
#define TDM_SP_REGNUM	TDM_SIM_SP_REGNUM
#define TDM_R0_REGNUM	TDM_SIM_R0_REGNUM
#define TDM_R1_REGNUM	TDM_SIM_R1_REGNUM
#define TDM_NUM_REGS	TDM_SIM_NUM_REGS

/* Octets in an addressable unit (a word).  */
#define TDM_UNIT_SIZE	2

/* Limit the instructions examined when analyzing a function.  */
#define TDM_MAX_ANALYSIS 4096

static const char *const tdm_register_names[] =
{
  "pc", "sp", "r0", "r1",
  "intdis", "mmu_offset", "mmu_limit", "mmu_user", "mmu_av",
  "int_enable", "int_pending", "cycles"
};

static_assert (ARRAY_SIZE (tdm_register_names) == TDM_NUM_REGS);

static const char *
tdm_register_name (struct gdbarch *gdbarch, int regnum)
{
  return tdm_register_names[regnum];
}

static struct type *
tdm_register_type (struct gdbarch *gdbarch, int regnum)
{
  switch (regnum)
    {
    case TDM_PC_REGNUM:
      return builtin_type (gdbarch)->builtin_func_ptr;
    case TDM_SP_REGNUM:
      return builtin_type (gdbarch)->builtin_data_ptr;
    case TDM_SIM_CYCLES_REGNUM:
      return builtin_type (gdbarch)->builtin_uint64;
    default:
      return builtin_type (gdbarch)->builtin_uint16;
    }
}

/* The architectural registers are general registers; the rest is CPU
   state only the simulator can show.  */

static bool
tdm_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
			 const struct reggroup *group)
{
  bool general = regnum < TDM_SIM_INTDIS_REGNUM;

  if (group == all_reggroup || group == save_reggroup
      || group == restore_reggroup)
    return true;
  if (group == general_reggroup)
    return general;
  if (group == system_reggroup)
    return !general;
  return false;
}

static int
tdm_addressable_memory_unit_size (struct gdbarch *gdbarch)
{
  return TDM_UNIT_SIZE;
}

/* BRK: the canonical breakpoint instruction.  */

constexpr gdb_byte tdm_break_insn[] =
  { TDM_BRK_INSN & 0xff, TDM_BRK_INSN >> 8 };

typedef BP_MANIPULATION (tdm_break_insn) tdm_breakpoint;

/* Read the instruction word at ADDR.  */

static unsigned int
tdm_read_insn (CORE_ADDR addr)
{
  gdb_byte buf[TDM_UNIT_SIZE];

  /* The length is in addressable units.  */
  read_code (addr, buf, 1);
  return extract_unsigned_integer (buf, sizeof (buf), BFD_ENDIAN_LITTLE);
}

/* Return true if INSN is "mov $pc, [$sp]", i.e. a return.  */

static bool
tdm_insn_is_return (unsigned int insn)
{
  return insn == TDM_INSN (TDM_OP_MOV, 0, TDM_OPB_MEM_SP, TDM_OPA_PC, 0);
}

/* If INSN adds to or subtracts from $sp, return true and set *DELTA to
   how much it lowers $sp (negative for an add).  INSN is at ADDR, in
   case its operand is a PC-relative constant.  */

static bool
tdm_insn_sp_adjust (unsigned int insn, CORE_ADDR addr, int *delta)
{
  unsigned int opcode = TDM_INSN_OPCODE (insn);
  unsigned int opb = TDM_INSN_OPB (insn);
  int amount;

  if ((opcode != TDM_OP_ADD && opcode != TDM_OP_SUB)
      || TDM_INSN_D (insn) != 0 || TDM_INSN_OPA (insn) != TDM_OPA_SP)
    return false;

  if (opb == TDM_OPB_IMM)
    amount = TDM_INSN_IMMED (insn);
  else if (opb == TDM_OPB_MEM_PC)
    amount = (int16_t) tdm_read_insn (addr + TDM_INSN_IMMED (insn));
  else
    return false;

  *delta = opcode == TDM_OP_SUB ? amount : -amount;
  return true;
}

/* Return true if INSN changes $sp in a way the analysis can't follow.  */

static bool
tdm_insn_clobbers_sp (unsigned int insn)
{
  unsigned int opcode = TDM_INSN_OPCODE (insn);

  if (TDM_INSN_IS_BRK (insn) || TDM_OP_IS_PREDICATE (opcode))
    return false;
  if (opcode == TDM_OP_SWAP)
    return TDM_INSN_OPA (insn) == TDM_OPA_SP;
  return TDM_INSN_D (insn) == 0 && TDM_INSN_OPA (insn) == TDM_OPA_SP;
}

/* Skip the prologue: the $sp adjustments at the start of the function.  */

static CORE_ADDR
tdm_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  CORE_ADDR func_start;
  int delta;

  if (!find_pc_partial_function (pc, nullptr, &func_start, nullptr))
    return pc;

  pc = func_start;
  while (tdm_insn_sp_adjust (tdm_read_insn (pc), pc, &delta) && delta > 0)
    pc++;
  return pc;
}

/* Return how much lower $sp is at PC than at the entry of the function
   starting at FUNC_START.

   The instructions between the two are taken in address order, summing
   their $sp adjustments: the prologue, the pushes of arguments before a
   call and their removal after it.  An adjustment followed by a return
   is an epilogue: once PC is past the return, it's on a path that
   didn't reach PC.  */

static int
tdm_sp_offset (CORE_ADDR func_start, CORE_ADDR pc)
{
  CORE_ADDR addr;
  int offset = 0;

  if (pc - func_start > TDM_MAX_ANALYSIS)
    return 0;

  for (addr = func_start; addr < pc; addr++)
    {
      unsigned int insn = tdm_read_insn (addr);
      int delta;

      if (tdm_insn_sp_adjust (insn, addr, &delta))
	{
	  if (addr + 1 < pc && tdm_insn_is_return (tdm_read_insn (addr + 1)))
	    continue;
	  offset += delta;
	}
      else if (tdm_insn_clobbers_sp (insn))
	break;
    }
  return offset;
}

struct tdm_frame_cache
{
  /* The function's start, and the frame's CFA: the address of the
     return address.  */
  CORE_ADDR func;
  CORE_ADDR cfa;
};

static struct tdm_frame_cache *
tdm_frame_cache (const frame_info_ptr &this_frame, void **this_cache)
{
  struct tdm_frame_cache *cache;
  CORE_ADDR sp, pc;

  if (*this_cache != nullptr)
    return (struct tdm_frame_cache *) *this_cache;

  cache = frame_obstack_zalloc<struct tdm_frame_cache> ();
  *this_cache = cache;

  sp = get_frame_register_unsigned (this_frame, TDM_SP_REGNUM);
  pc = get_frame_pc (this_frame);
  cache->func = get_frame_func (this_frame);
  if (cache->func == 0)
    cache->func = pc;
  cache->cfa = (sp + tdm_sp_offset (cache->func, pc)) & 0xffff;
  return cache;
}

static void
tdm_frame_this_id (const frame_info_ptr &this_frame, void **this_cache,
		   struct frame_id *this_id)
{
  struct tdm_frame_cache *cache = tdm_frame_cache (this_frame, this_cache);

  *this_id = frame_id_build (cache->cfa, cache->func);
}

static struct value *
tdm_frame_prev_register (const frame_info_ptr &this_frame,
			 void **this_cache, int regnum)
{
  struct tdm_frame_cache *cache = tdm_frame_cache (this_frame, this_cache);

  if (regnum == TDM_PC_REGNUM)
    return frame_unwind_got_memory (this_frame, regnum, cache->cfa);
  if (regnum == TDM_SP_REGNUM)
    return frame_unwind_got_constant (this_frame, regnum, cache->cfa);
  return frame_unwind_got_register (this_frame, regnum, regnum);
}

static const struct frame_unwind_legacy tdm_frame_unwind (
  "tdm prologue",
  NORMAL_FRAME,
  FRAME_UNWIND_ARCH,
  default_frame_unwind_stop_reason,
  tdm_frame_this_id,
  tdm_frame_prev_register,
  nullptr,
  default_frame_sniffer
);

static CORE_ADDR
tdm_frame_base_address (const frame_info_ptr &this_frame, void **this_cache)
{
  return tdm_frame_cache (this_frame, this_cache)->cfa;
}

static const struct frame_base tdm_frame_base =
{
  &tdm_frame_unwind,
  tdm_frame_base_address,
  tdm_frame_base_address,
  tdm_frame_base_address
};

/* Values up to 16 bits are returned in $r0, up to 32 bits in $r1:$r0
   (low word in $r0).  Larger values aren't supported.  */

static enum return_value_convention
tdm_return_value (struct gdbarch *gdbarch, struct value *function,
		  struct type *valtype, struct regcache *regcache,
		  gdb_byte *readbuf, const gdb_byte *writebuf)
{
  int len = valtype->length ();
  int i;

  if (len > 4 || valtype->code () == TYPE_CODE_STRUCT
      || valtype->code () == TYPE_CODE_UNION
      || valtype->code () == TYPE_CODE_ARRAY)
    return RETURN_VALUE_STRUCT_CONVENTION;

  for (i = 0; i * 2 < len; i++)
    {
      gdb_byte buf[2] = { 0, 0 };
      int n = std::min (2, len - i * 2);

      if (readbuf != nullptr)
	{
	  regcache->raw_read (TDM_R0_REGNUM + i, buf);
	  memcpy (readbuf + i * 2, buf, n);
	}
      if (writebuf != nullptr)
	{
	  memcpy (buf, writebuf + i * 2, n);
	  regcache->raw_write (TDM_R0_REGNUM + i, buf);
	}
    }
  return RETURN_VALUE_REGISTER_CONVENTION;
}

static struct gdbarch *
tdm_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  /* If there is already a candidate, use it.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != nullptr)
    return arches->gdbarch;

  gdbarch *gdbarch = gdbarch_alloc (&info, nullptr);

  set_gdbarch_addressable_memory_unit_size (gdbarch,
					    tdm_addressable_memory_unit_size);

  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 16);
  set_gdbarch_long_bit (gdbarch, 32);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_ptr_bit (gdbarch, 16);
  set_gdbarch_addr_bit (gdbarch, 16);

  set_gdbarch_num_regs (gdbarch, TDM_NUM_REGS);
  set_gdbarch_pc_regnum (gdbarch, TDM_PC_REGNUM);
  set_gdbarch_sp_regnum (gdbarch, TDM_SP_REGNUM);
  set_gdbarch_register_name (gdbarch, tdm_register_name);
  set_gdbarch_register_type (gdbarch, tdm_register_type);
  set_gdbarch_register_reggroup_p (gdbarch, tdm_register_reggroup_p);

  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_skip_prologue (gdbarch, tdm_skip_prologue);
  set_gdbarch_breakpoint_kind_from_pc (gdbarch, tdm_breakpoint::kind_from_pc);
  set_gdbarch_sw_breakpoint_from_kind (gdbarch, tdm_breakpoint::bp_from_kind);
  set_gdbarch_return_value (gdbarch, tdm_return_value);

  gdbarch_init_osabi (info, gdbarch);

  frame_base_set_default (gdbarch, &tdm_frame_base);
  frame_unwind_append_unwinder (gdbarch, &tdm_frame_unwind);

  return gdbarch;
}

INIT_GDB_FILE (tdm_tdep)
{
  gdbarch_register (bfd_arch_tdm, tdm_gdbarch_init);
}
