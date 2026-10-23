#source: dwarf-line.s
#as: --gdwarf-2
#readelf: -r
#name: TDM DWARF relocations

#...
Relocation section '\.rela\.debug_line' at offset 0x[0-9a-f]+ contains 1 entry:
 Offset +Info +Type +Sym\.Value +Sym\. Name \+ Addend
0000002d +[0-9a-f]+ R_TDM_16 +00000000 +\.text \+ 0

Relocation section '\.rela\.debug_info' at offset 0x[0-9a-f]+ contains 7 entries:
 Offset +Info +Type +Sym\.Value +Sym\. Name \+ Addend
00000006 +[0-9a-f]+ R_TDM_32 +00000000 +\.debug_abbrev \+ 0
0000000c +[0-9a-f]+ R_TDM_32 +00000000 +\.debug_line \+ 0
00000010 +[0-9a-f]+ R_TDM_16 +00000000 +\.text \+ 0
00000012 +[0-9a-f]+ R_TDM_16 +00000000 +\.text \+ 5
00000014 +[0-9a-f]+ R_TDM_32 +00000000 +\.debug_str \+ [0-9a-f]+
00000018 +[0-9a-f]+ R_TDM_32 +00000000 +\.debug_str \+ [0-9a-f]+
0000001c +[0-9a-f]+ R_TDM_32 +00000000 +\.debug_str \+ [0-9a-f]+

Relocation section '\.rela\.debug_aranges' at offset 0x[0-9a-f]+ contains 2 entries:
 Offset +Info +Type +Sym\.Value +Sym\. Name \+ Addend
00000006 +[0-9a-f]+ R_TDM_32 +00000000 +\.debug_info \+ 0
0000000c +[0-9a-f]+ R_TDM_16 +00000000 +\.text \+ 0
