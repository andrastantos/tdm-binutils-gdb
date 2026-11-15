#source: bptr-wrap.s
#ld: -Ttext=0x1000 -Tdata=0x2000
#objdump: -s -j .rodata
#name: TDM byte pointer wrapping below zero

.*: +file format elf32-tdm

Contents of section \.rodata:
 1001 0000ffff 00ca02ca +.*
