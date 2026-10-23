#source: dwarf.s
#as: --gdwarf-5
#ld: -Ttext=0x1000 -e f
#readelf: -wL
#name: TDM DWARF line table after linking

Contents of the \.debug_line section:

dwarf-line\.c:
File name +Line number +Starting address +View +Stmt
dwarf-line\.c +10 +0x1000 +x
dwarf-line\.c +11 +0x1002 +x
dwarf-line\.c +12 +0x1003 +x
dwarf-line\.c +- +0x1005
