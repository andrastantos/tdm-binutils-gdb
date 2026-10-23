#objdump: -dr
#name: TDM relocations in code
#source: reloc.s

.*: +file format elf32-tdm
Disassembly of section \.text:
00000000 <_start>:
   0:	8080 	mov	\$r0, \[\$pc\]	; 0 <_start>
			0: R_TDM_PCREL6	\.rodata\+0x1
   1:	83c0 	mov	\$r1, \$mr0
			1: R_TDM_IMM6	ioport
   2:	8740 	mov	\$sp, 0
			2: R_TDM_IMM6	small
   3:	5082 	add	\$r0, \[\$pc\+2\]	; 5 <local>
   4:	843c 	mov	\$pc, \$pc-4	; 0 <_start>
00000005 <local>:
   5:	1234 	or	\$pc, \[\$r0-12\]
