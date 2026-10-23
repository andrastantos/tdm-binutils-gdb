/* TDM simulator stdio and exit device.
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

/* DEVICE

   tdm_stdio - console and exit ports for simulated programs

   DESCRIPTION

   Three 16-bit registers, at consecutive word addresses:

   |0	EXIT	write: stop the simulation, the value is the exit status
   |1	STDOUT	write: the low 8 bits go to standard output
   |2	STDIN	read: the next character from standard input, or 0xffff
   |		at end of file

   TDM writes back every memory operand it reads, so all of these have
   to tolerate a read followed by a write of the value read.  Writes with
   bit 15 set are ignored, and reads (other than of STDIN) return 0x8000.
   Writes to STDIN are ignored.  So

   |	mov	[EXIT], $r0	; read 0x8000, write $r0: exit
   |	mov	[STDOUT], $r0	; read 0x8000, write $r0: print a character
   |	mov	$r0, [STDIN]	; read a character, write it back: ignored

   PROPERTIES

   reg = <address> <size>

   Where to attach, in octets (twice the TDM word address); size 6.

   disabled = <boolean>

   Don't attach the device.  The simulator adds this device by default;
   use this to free its addresses for a different device.  */

/* This must come before any other includes.  */
#include "defs.h"

#include "sim-main.h"
#include "hw-main.h"
#include "sim-io.h"

enum
{
  TDM_STDIO_EXIT,
  TDM_STDIO_STDOUT,
  TDM_STDIO_STDIN,
  TDM_STDIO_NR_REGS
};

/* Writes with this bit set are ignored, reads return it.  */
#define TDM_STDIO_IGNORE	0x8000
#define TDM_STDIO_EOF		0xffff

struct tdm_stdio
{
  address_word base;
};

static int
tdm_stdio_reg (struct hw *me, struct tdm_stdio *stdio, address_word addr,
	       unsigned nr_bytes)
{
  if (nr_bytes != 2 || (addr - stdio->base) % 2 != 0)
    hw_abort (me, "%u byte access to %#" PRIx64 ": only words are supported",
	      nr_bytes, (uint64_t) addr);
  return (addr - stdio->base) / 2;
}

static unsigned
tdm_stdio_io_read_buffer (struct hw *me, void *dest, int space,
			  address_word addr, unsigned nr_bytes)
{
  struct tdm_stdio *stdio = hw_data (me);
  unsigned char *p = dest;
  uint16_t value = TDM_STDIO_IGNORE;

  if (tdm_stdio_reg (me, stdio, addr, nr_bytes) == TDM_STDIO_STDIN)
    {
      char c;

      if (sim_io_read_stdin (hw_system (me), &c, 1) == 1)
	value = (unsigned char) c;
      else
	value = TDM_STDIO_EOF;
    }

  p[0] = value;
  p[1] = value >> 8;
  return nr_bytes;
}

static unsigned
tdm_stdio_io_write_buffer (struct hw *me, const void *source, int space,
			   address_word addr, unsigned nr_bytes)
{
  struct tdm_stdio *stdio = hw_data (me);
  const unsigned char *p = source;
  int reg = tdm_stdio_reg (me, stdio, addr, nr_bytes);
  uint16_t value = p[0] | (p[1] << 8);

  if (value & TDM_STDIO_IGNORE)
    return nr_bytes;

  switch (reg)
    {
    case TDM_STDIO_EXIT:
      hw_halt (me, sim_exited, value);
      break;

    case TDM_STDIO_STDOUT:
      {
	char c = value;

	sim_io_write_stdout (hw_system (me), &c, 1);
	sim_io_flush_stdout (hw_system (me));
      }
      break;

    default:
      break;
    }
  return nr_bytes;
}

static void
attach_tdm_stdio_regs (struct hw *me, struct tdm_stdio *stdio)
{
  address_word attach_address;
  int attach_space;
  unsigned attach_size;
  reg_property_spec reg;

  if (hw_find_property (me, "reg") == NULL)
    hw_abort (me, "Missing \"reg\" property");
  if (!hw_find_reg_array_property (me, "reg", 0, &reg))
    hw_abort (me, "\"reg\" property must contain an address and a size");
  hw_unit_address_to_attach_address (hw_parent (me), &reg.address,
				     &attach_space, &attach_address, me);
  hw_unit_size_to_attach_size (hw_parent (me), &reg.size, &attach_size, me);
  if (attach_size != TDM_STDIO_NR_REGS * 2)
    hw_abort (me, "\"reg\" size must be %d", TDM_STDIO_NR_REGS * 2);

  stdio->base = attach_address;
  hw_attach_address (hw_parent (me), 0, attach_space, attach_address,
		     attach_size, me);
}

static void
tdm_stdio_finish (struct hw *me)
{
  struct tdm_stdio *stdio;

  if (hw_find_property (me, "disabled") != NULL
      && hw_find_boolean_property (me, "disabled"))
    return;

  stdio = HW_ZALLOC (me, struct tdm_stdio);
  set_hw_data (me, stdio);
  set_hw_io_read_buffer (me, tdm_stdio_io_read_buffer);
  set_hw_io_write_buffer (me, tdm_stdio_io_write_buffer);
  attach_tdm_stdio_regs (me, stdio);
}

const struct hw_descriptor dv_tdm_stdio_descriptor[] =
{
  {"tdm_stdio", tdm_stdio_finish,},
  {NULL, NULL},
};
