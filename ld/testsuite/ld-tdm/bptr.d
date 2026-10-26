#source: bptr1.s
#source: bptr2.s
#ld: -Ttext=0x1000
#objdump: -s -j .rodata -j .data
#name: TDM byte pointers

.*: +file format elf32-tdm

Contents of section \.rodata:
 1001 0a200d20 1320 +.*
Contents of section \.data:
 1004 01006869 00005555 00000000 +.*
