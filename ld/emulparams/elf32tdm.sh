# TDM (The Disintegrated Machine) is word addressed: all addresses and
# sizes in the linker script are in 16-bit units.
SCRIPT_NAME=elf
TEMPLATE_NAME=elf
OUTPUT_FORMAT="elf32-tdm"
# MEM[0] and MEM[1] are the reset and interrupt vectors, MEM[2] is a
# scratch location by convention.
TEXT_START_ADDR=0x1000
MAXPAGESIZE="CONSTANT (MAXPAGESIZE)"
ARCH=tdm
EMBEDDED=yes
# Nothing on TDM needs more than word alignment.
ALIGNMENT=1
