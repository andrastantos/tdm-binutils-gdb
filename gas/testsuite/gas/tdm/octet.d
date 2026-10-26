#objdump: -s -t
#name: TDM octet data
#source: octet.s

.*: +file format elf32-tdm

SYMBOL TABLE:
#...
0+0000 l       \.data	0+0000 o1
0+0002 l       \.data	0+0000 o2
#...
0+0001 l       \.text	0+0000 start
#...
Contents of section \.text:
 0000 0a008187 0b00001f 0c00 +.*
Contents of section \.data:
 0000 01020300 04616205 63003412 06000000  .*
 0008 07000900 +.*
Contents of section \.rodata:
 0000 0800 +.*
