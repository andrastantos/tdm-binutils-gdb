/* TDM (The Disintegrated Machine) specific support for 32-bit ELF.
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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* TDM is word addressed: a BFD byte is 16 bits.  Following the generic
   ELF code, section and segment headers hold octet addresses, while
   symbol values and relocation offsets are in 16-bit units.  Howto
   sizes are in octets, so a single instruction word has size 2.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/tdm.h"

/* Apply R_TDM_BYTEPTR16 outside the linker (which handles it in
   tdm_elf_relocate_section): the value is twice the word address of
   the symbol, plus the addend in octets.  */

static bfd_reloc_status_type
tdm_elf_byteptr_reloc (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
		       void *data, asection *input_section, bfd *output_bfd,
		       char **error_message)
{
  bfd_size_type octets;
  bfd_vma value;

  if (output_bfd != NULL)
    return bfd_elf_generic_reloc (abfd, reloc_entry, symbol, data,
				  input_section, output_bfd, error_message);

  if (bfd_is_und_section (symbol->section))
    return bfd_reloc_undefined;

  octets = reloc_entry->address * bfd_octets_per_byte (abfd, input_section);
  if (!bfd_reloc_offset_in_range (reloc_entry->howto, abfd, input_section,
				  octets))
    return bfd_reloc_outofrange;

  value = 2 * (symbol->value + symbol->section->output_section->vma
	       + symbol->section->output_offset)
	  + reloc_entry->addend;
  bfd_put_16 (abfd, value & 0xffff, (bfd_byte *) data + octets);
  return value > 0xffff ? bfd_reloc_overflow : bfd_reloc_ok;
}

static reloc_howto_type tdm_elf_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_TDM_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size */
	 0,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TDM_NONE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 16-bit data word.  Both signed and unsigned values are fine.  */
  HOWTO (R_TDM_16,		/* type */
	 0,			/* rightshift */
	 2,			/* size */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TDM_16",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32-bit data value, stored low word first.  */
  HOWTO (R_TDM_32,		/* type */
	 0,			/* rightshift */
	 4,			/* size */
	 32,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TDM_32",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* The signed 6-bit IMMED field of an instruction.  The overflow check
     wraps at the 16-bit address size, so addresses 0xffe0..0xffff (the
     I/O page) are encodable as -32..-1.  */
  HOWTO (R_TDM_IMM6,		/* type */
	 0,			/* rightshift */
	 2,			/* size */
	 6,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TDM_IMM6",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x3f,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* As R_TDM_IMM6, but relative to the address of the instruction,
     which is what $pc holds while the operand is computed.  */
  HOWTO (R_TDM_PCREL6,		/* type */
	 0,			/* rightshift */
	 2,			/* size */
	 6,			/* bitsize */
	 true,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_TDM_PCREL6",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0x3f,			/* dst_mask */
	 true),			/* pcrel_offset */

  /* A 16-bit byte pointer: twice the word address of the symbol, plus
     the addend in octets.  Byte pointers reach the first 32K words.  */
  HOWTO (R_TDM_BYTEPTR16,	/* type */
	 0,			/* rightshift */
	 2,			/* size */
	 16,			/* bitsize */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_unsigned, /* complain_on_overflow */
	 tdm_elf_byteptr_reloc,	/* special_function */
	 "R_TDM_BYTEPTR16",	/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */
};

/* Map BFD reloc types to TDM ELF reloc types.  */

struct tdm_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned int tdm_reloc_val;
};

static const struct tdm_reloc_map tdm_reloc_map[] =
{
  { BFD_RELOC_NONE,		R_TDM_NONE },
  { BFD_RELOC_16,		R_TDM_16 },
  { BFD_RELOC_32,		R_TDM_32 },
  { BFD_RELOC_TDM_IMM6,		R_TDM_IMM6 },
  { BFD_RELOC_TDM_PCREL6,	R_TDM_PCREL6 },
  { BFD_RELOC_TDM_BYTEPTR16,	R_TDM_BYTEPTR16 },
};

static reloc_howto_type *
tdm_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
		       bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = 0; i < sizeof (tdm_reloc_map) / sizeof (tdm_reloc_map[0]); i++)
    if (tdm_reloc_map[i].bfd_reloc_val == code)
      return &tdm_elf_howto_table[tdm_reloc_map[i].tdm_reloc_val];

  return NULL;
}

static reloc_howto_type *
tdm_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0; i < sizeof (tdm_elf_howto_table) / sizeof (tdm_elf_howto_table[0]); i++)
    if (tdm_elf_howto_table[i].name != NULL
	&& strcasecmp (tdm_elf_howto_table[i].name, r_name) == 0)
      return &tdm_elf_howto_table[i];

  return NULL;
}

/* Set the howto pointer for a TDM ELF reloc.  */

static bool
tdm_info_to_howto_rela (bfd *abfd,
			arelent *cache_ptr,
			Elf_Internal_Rela *dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  if (r_type >= (unsigned int) R_TDM_max)
    {
      /* xgettext:c-format */
      _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
			  abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return false;
    }
  cache_ptr->howto = &tdm_elf_howto_table[r_type];
  return true;
}

/* Relocate a TDM ELF section.  All relocations are handled by the
   generic routines; see elf32-moxie.c for a description of the
   arguments.  */

static int
tdm_elf_relocate_section (bfd *output_bfd,
			  struct bfd_link_info *info,
			  bfd *input_bfd,
			  asection *input_section,
			  bfd_byte *contents,
			  Elf_Internal_Rela *relocs,
			  Elf_Internal_Sym *local_syms,
			  asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = &elf_symtab_hdr (input_bfd);
  sym_hashes = elf_sym_hashes (input_bfd);
  relend = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *name;
      unsigned int r_type;

      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_type >= (unsigned int) R_TDM_max)
	{
	  _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
			      input_bfd, r_type);
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}
      howto = tdm_elf_howto_table + r_type;
      h = NULL;
      sym = NULL;
      sec = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  if (name == NULL || *name == '\0')
	    name = bfd_section_name (sec);
	}
      else
	{
	  bool unresolved_reloc, warned, ignored;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned, ignored);

	  name = h->root.root.string;
	}

      if (sec != NULL && discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, 1, relend, R_TDM_NONE,
					 howto, 0, contents);

      if (bfd_link_relocatable (info))
	{
	  /* The generic code adds the output offset of a section to the
	     addend of a relocation against its section symbol; the addend
	     of a byte pointer counts octets, so that has to be doubled.  */
	  if (r_type == R_TDM_BYTEPTR16
	      && sym != NULL
	      && ELF_ST_TYPE (sym->st_info) == STT_SECTION)
	    rel->r_addend += sec->output_offset;
	  continue;
	}

      if (r_type == R_TDM_BYTEPTR16)
	{
	  /* Twice the word address, plus the addend in octets.  The
	     generic overflow check masks the value to the 16-bit address
	     width first, which would hide pointers past 32K words.  */
	  bfd_size_type octets
	    = rel->r_offset * bfd_octets_per_byte (input_bfd, input_section);
	  bfd_vma value = 2 * relocation + rel->r_addend;

	  if (!bfd_reloc_offset_in_range (howto, input_bfd, input_section,
					  octets))
	    r = bfd_reloc_outofrange;
	  else
	    {
	      bfd_put_16 (input_bfd, value & 0xffff, contents + octets);
	      r = value > 0xffff ? bfd_reloc_overflow : bfd_reloc_ok;
	    }
	}
      else
	r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				      contents, rel->r_offset,
				      relocation, rel->r_addend);

      if (r != bfd_reloc_ok)
	{
	  const char *msg = NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      (*info->callbacks->reloc_overflow)
		(info, (h ? &h->root : NULL), name, howto->name,
		 (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      (*info->callbacks->undefined_symbol)
		(info, name, input_bfd, input_section, rel->r_offset, true);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    (*info->callbacks->warning) (info, msg, name, input_bfd,
					 input_section, rel->r_offset);
	}
    }

  return true;
}

#define ELF_ARCH		bfd_arch_tdm
#define ELF_MACHINE_CODE	EM_TDM
#define ELF_MAXPAGESIZE		0x1

#define TARGET_LITTLE_SYM	tdm_elf32_vec
#define TARGET_LITTLE_NAME	"elf32-tdm"

#define elf_info_to_howto_rel			NULL
#define elf_info_to_howto			tdm_info_to_howto_rela
#define elf_backend_relocate_section		tdm_elf_relocate_section

#define elf_backend_can_gc_sections		1
#define elf_backend_rela_normal			1

#define bfd_elf32_bfd_reloc_type_lookup		tdm_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup		tdm_reloc_name_lookup

#include "elf32-target.h"
