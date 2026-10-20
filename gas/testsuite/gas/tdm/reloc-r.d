#readelf: -r
#name: TDM relocation types
#source: reloc.s

Relocation section '\.rela\.text' at offset 0x[0-9a-f]+ contains 3 entries:
 Offset     Info    Type            Sym\.Value  Sym\. Name \+ Addend
00000000  00000604 R_TDM_PCREL6      00000000   \.rodata \+ 1
00000001  00000803 R_TDM_IMM6        00000000   ioport \+ 0
00000002  00000903 R_TDM_IMM6        00000000   small \+ 0
Relocation section '\.rela\.rodata' at offset 0x[0-9a-f]+ contains 3 entries:
 Offset     Info    Type            Sym\.Value  Sym\. Name \+ Addend
00000001  00000a01 R_TDM_16          00000000   far_away \+ 0
00000002  00000a02 R_TDM_32          00000000   far_away \+ 1
00000004  00000601 R_TDM_16          00000000   \.rodata \+ 4
