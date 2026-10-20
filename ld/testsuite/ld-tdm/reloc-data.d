#source: reloc1.s
#source: reloc2.s
#ld: -Ttext=0x1000
#objdump: -s -j .rodata -j .data
#name: TDM data relocations

.*: +file format elf32-tdm

Contents of section \.rodata:
 1004 0a100b10 0000 +.*
Contents of section \.data:
 1007 00000000 00000700 +.*
