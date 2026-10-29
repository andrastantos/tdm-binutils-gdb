#source: largemem.s
#ld: -m elf32tdm_largemem
#objdump: -s -j .text -j .data -j .rodata
#name: TDM largemem layout

.*: +file format elf32-tdm

Contents of section \.text:
 8000 001f4200 40004600 +.*
Contents of section \.data:
 0020 0080 +.*
Contents of section \.rodata:
 0021 68690000 +.*
