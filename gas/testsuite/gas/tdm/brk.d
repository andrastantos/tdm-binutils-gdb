#objdump: -d
#name: TDM brk

.*: +file format elf32-tdm

Disassembly of section \.text:

00000000 <\.text>:
   0:	1f00 	brk
   1:	1f05 	brk	5
   2:	1fff 	brk	255
   3:	0f00 	brk
   4:	2fab 	brk	171
   5:	4000 	\.word	0x4000
