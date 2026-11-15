# This shell script emits a C file. -*- C -*-
#   Copyright (C) 2026 Free Software Foundation, Inc.
#
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.
#

# This file is sourced from elf.em, and defines extra tdm-elf
# specific routines.
#
fragment <<EOF

#include "elf-bfd.h"

/* Where the startup code puts the stack if the link doesn't define
   _stack: the top of the 32K words that C data can be in.  It has to
   match crt0.S in libgloss.  */
#define TDM_DEFAULT_STACK 0x7ffe

/* The least room, in words, that there should be between the end of
   the data (_end, where the heap starts) and the top of the stack.
   Zero turns the check off.  */
static bfd_vma tdm_stack_room = 256;

/* The value of the symbol NAME, if the link defines it.  */

static bool
tdm_defined_symbol (const char *name, bfd_vma *value)
{
  struct bfd_link_hash_entry *h
    = bfd_link_hash_lookup (link_info.hash, name, false, false, true);

  if (h == NULL
      || (h->type != bfd_link_hash_defined
	  && h->type != bfd_link_hash_defweak))
    return false;
  *value = h->u.def.value;
  if (h->u.def.section->output_section != NULL)
    *value += (h->u.def.section->output_section->vma
	       + h->u.def.section->output_offset);
  return true;
}

/* Whether an object file of the link refers to the symbol NAME.  */

static bool
tdm_referenced_symbol_p (const char *name)
{
  struct elf_link_hash_entry *h;

  if (!is_elf_hash_table (link_info.hash))
    return false;
  h = elf_link_hash_lookup (elf_hash_table (&link_info), name,
			    false, false, true);
  return h != NULL && h->ref_regular;
}

/* The stack grows down from _stack towards the end of the data, and
   nothing stops it there.  Warn about a program that leaves it too
   little room: it would overwrite its data, or its code after that.
   Only programs that refer to _end are checked, as the startup code
   does to clear .bss and the heap does to start there: one that
   doesn't has a memory layout of its own.  */

static void
tdm_elf_finish (void)
{
  bfd_vma end, stack;

  if (tdm_stack_room != 0
      && !bfd_link_relocatable (&link_info)
      && tdm_defined_symbol ("_end", &end)
      && tdm_referenced_symbol_p ("_end"))
    {
      if (!tdm_defined_symbol ("_stack", &stack))
	stack = TDM_DEFAULT_STACK;
      if (end > stack)
	einfo (_("%P: warning: the data ends at word 0x%v, "
		 "above the top of the stack at 0x%v\n"), end, stack);
      else if (stack - end < tdm_stack_room)
	einfo (_("%P: warning: only %u words between the end of the data "
		 "at 0x%v and the top of the stack at 0x%v "
		 "(--stack-room=%u)\n"),
	       (unsigned int) (stack - end), end, stack,
	       (unsigned int) tdm_stack_room);
    }

  ldelf_finish ();
}

EOF

# Define some shell vars to insert bits of code into the standard elf
# parse_args and list_options functions.
#
PARSE_AND_LIST_LONGOPTS='
  { "stack-room", required_argument, NULL, OPTION_TDM_STACK_ROOM },
'

PARSE_AND_LIST_OPTIONS='
  fprintf (file, _("\
  --stack-room=WORDS          Warn if there are fewer than WORDS words between\n\
                                the end of the data and the top of the stack\n\
                                (default 256, 0 for no check)\n"));
'

PARSE_AND_LIST_ARGS_CASES='
    case OPTION_TDM_STACK_ROOM:
      {
	const char *end;
	tdm_stack_room = bfd_scan_vma (optarg, &end, 0);
	if (*end != 0)
	  fatal (_("%P: invalid number `%s'\''\n"), optarg);
      }
      break;
'

LDEMUL_FINISH=tdm_elf_finish
