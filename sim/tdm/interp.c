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

/* Glue between the sim framework and the TDM CPU core.  */

/* This must come before any other includes.  */
#include "defs.h"

#include "sim/callback.h"
#include "sim-main.h"
#include "sim-options.h"
#include "sim-hw.h"

#include "tdm-sim.h"

void
sim_engine_run (SIM_DESC sd,
		int next_cpu_nr, /* ignore  */
		int nr_cpus, /* ignore  */
		int siggnal) /* ignore  */
{
  SIM_CPU *cpu;

  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  cpu = STATE_CPU (sd, 0);

  while (1)
    {
      tdm_step_once (cpu);
      if (sim_events_tick (sd))
	sim_events_process (sd);
    }
}

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

/* The default devices.  Devices are configured through the hw device
   tree, so everything here can be changed with --hw-device, which is
   processed first: the first definition of a property wins.  To move
   the stdio/exit device, give it a different "reg"; to remove it (say,
   to put a different device at its address), use
     --hw-device "/core/tdm_stdio/disabled true"
   External interrupt lines are ports int1..int7 of /tdm_cpu, e.g.
     --hw-device "/some/device > irq int3 /tdm_cpu"  */

static void
add_default_devices (SIM_DESC sd)
{
  sim_hw_parse (sd, "/core/tdm_stdio/reg %#" PRIx64 " %d",
		(uint64_t) TDM_OCTET_ADDR (TDM_STDIO_ADDR), 3 * 2);
  sim_hw_parse (sd, "/tdm_cpu");
}

SIM_DESC
sim_open (SIM_OPEN_KIND kind, host_callback *callback,
	  struct bfd *abfd, char * const *argv)
{
  char c;
  int i;
  SIM_DESC sd = sim_state_alloc (kind, callback);

  /* Set default options before parsing user options.  */
  current_alignment = STRICT_ALIGNMENT;
  current_target_byte_order = BFD_ENDIAN_LITTLE;

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all_extra (sd, 0, sizeof (struct tdm_sim_cpu))
      != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  if (STATE_ENVIRONMENT (sd) == ALL_ENVIRONMENT)
    STATE_ENVIRONMENT (sd) = VIRTUAL_ENVIRONMENT;

  /* The parser will print an error message for us, so we silently return.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  add_default_devices (sd);

  /* Check for/establish the a reference program image.  */
  if (sim_analyze_program (sd, STATE_PROG_FILE (sd), abfd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Establish any remaining configuration options.  */
  if (sim_config (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* CPU specific initialization.  */
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      SIM_CPU *cpu = STATE_CPU (sd, i);

      tdm_initialize_cpu (sd, cpu);
    }

  /* Memory, unless the user configured some.  Addresses given to the
     --memory-* options are octet addresses: twice the word address.  */
  if (sim_core_read_buffer (sd, NULL, read_map, &c, 0, 1) == 0)
    sim_do_commandf (sd, "memory-region 0,%#x",
		     (unsigned) TDM_OCTET_ADDR (TDM_DEFAULT_MEM_WORDS));

  return sd;
}

/* Prepare to run a program that has already been loaded into memory.  */

SIM_RC
sim_create_inferior (SIM_DESC sd, struct bfd *abfd,
		     char * const *argv, char * const *env)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  host_callback *cb = STATE_CALLBACK (sd);
  sim_cia addr;

  /* Start at the entry point of the program rather than going through
     the reset vector at MEM[0].  */
  if (abfd != NULL)
    addr = bfd_get_start_address (abfd);
  else
    addr = 0;
  sim_pc_set (cpu, addr);

  /* Standalone mode (i.e. `run`) will take care of the argv for us in
     sim_open() -> sim_parse_args().  But in debug mode (i.e. 'target sim'
     with `gdb`), we need to handle it because the user can change the
     argv on the fly via gdb's 'run'.  */
  if (STATE_PROG_ARGV (sd) != argv)
    {
      freeargv (STATE_PROG_ARGV (sd));
      STATE_PROG_ARGV (sd) = dupargv (argv);
    }

  if (STATE_PROG_ENVP (sd) != env)
    {
      freeargv (STATE_PROG_ENVP (sd));
      STATE_PROG_ENVP (sd) = dupargv (env);
    }

  cb->argv = STATE_PROG_ARGV (sd);
  cb->envp = STATE_PROG_ENVP (sd);

  return SIM_RC_OK;
}
