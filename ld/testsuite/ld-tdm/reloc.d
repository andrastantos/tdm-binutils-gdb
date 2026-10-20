#source: reloc1.s
#source: reloc2.s
#ld: -Ttext=0x1000
#objdump: -d
#name: TDM instruction relocations

.*: +file format elf32-tdm

Disassembly of section \.text:

00001000 <_start>:
    1000:	8084 	mov	\$r0, \[\$pc\+4\]	; 1004 <konst>
    1001:	83f8 	mov	\$r1, \[-8\]
    1002:	875f 	mov	\$sp, 31
    1003:	8400 	mov	\$pc, \$pc	; 1003 <_start\+0x3>
