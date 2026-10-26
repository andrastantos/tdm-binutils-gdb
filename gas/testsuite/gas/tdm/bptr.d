#objdump: -s -r -j .rodata
#name: TDM byte pointers
#source: bptr.s

.*: +file format elf32-tdm

RELOCATION RECORDS FOR \[\.rodata\]:
OFFSET +TYPE +VALUE
0+0000 R_TDM_BYTEPTR16 +str
0+0001 R_TDM_BYTEPTR16 +str\+0x0+0003
0+0002 R_TDM_BYTEPTR16 +str\+0x0+0001
0+0003 R_TDM_BYTEPTR16 +ext
0+0004 R_TDM_BYTEPTR16 +\.Lloc


Contents of section \.rodata:
 0000 00000000 00000000 00000102 4000 +.*
