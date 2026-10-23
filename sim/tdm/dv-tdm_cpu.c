/* TDM simulator CPU interrupt inputs.
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

   tdm_cpu - the external interrupt inputs of the TDM CPU

   DESCRIPTION

   The CPU's interrupt controller has eight sources.  Source 0 is the
   MMU's access violation; sources 1 to 7 are external, level sensitive
   lines.  This device makes them available to other devices as input
   ports int1 to int7.  The interrupt controller and the MMU themselves
   are part of the CPU model.

   PORTS

   int1 .. int7 (input)

   Drive the corresponding interrupt controller source.  */

/* This must come before any other includes.  */
#include "defs.h"

#include "sim-main.h"
#include "hw-main.h"

#include "tdm-sim.h"

static void
tdm_cpu_port_event (struct hw *me, int my_port, struct hw *source,
		    int source_port, int level)
{
  if (my_port < 1 || my_port > 7)
    hw_abort (me, "interrupt port %d out of range", my_port);
  tdm_set_irq_line (STATE_CPU (hw_system (me), 0), my_port, level);
}

static const struct hw_port_descriptor tdm_cpu_ports[] =
{
  { "int", 0, 8, input_port },	/* int0 is the MMU: not connectable.  */
  { NULL, 0, 0, 0 }
};

static void
tdm_cpu_finish (struct hw *me)
{
  set_hw_ports (me, tdm_cpu_ports);
  set_hw_port_event (me, tdm_cpu_port_event);
}

const struct hw_descriptor dv_tdm_cpu_descriptor[] =
{
  {"tdm_cpu", tdm_cpu_finish,},
  {NULL, NULL},
};
