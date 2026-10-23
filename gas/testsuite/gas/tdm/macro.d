#objdump: -d
#name: TDM instructions in macro arguments
#source: macro.s

.*: +file format elf32-tdm
Disassembly of section \.text:
00000000 <\.text>:
   0:	8981 	mov	\[\$sp\+1\], \$r0
   1:	f980 	if_gts	\$r0, \[\$sp\]
   2:	50ff 	add	\$r1, \[\$pc-1\]	; 0x1
