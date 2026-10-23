#objdump: -d
#name: TDM memory pseudo-registers

.*: +file format elf32-tdm

Disassembly of section \.text:

00000000 <\.text>:
   0:	8385 	mov	\$r0, \$mr5
   1:	8385 	mov	\$r0, \$mr5
   2:	8bdf 	mov	\$mr31, \$r1
   3:	8bdf 	mov	\$mr31, \$r1
   4:	5b40 	add	\$mr0, \$sp
   5:	7b87 	sub	\$mr7, \$r0
   6:	fb82 	if_gts	\$r0, \$mr2
   7:	c382 	if_eq	\$r0, \$mr2
   8:	0b01 	swap	\$pc, \$mr1
   9:	0301 	swapi	\$pc, \$mr1
   a:	bb03 	rol	\$mr3
   b:	9b04 	istat	\$mr4
   c:	83bf 	mov	\$r0, \[-1\]
   d:	8388 	mov	\$r0, \$mr8
