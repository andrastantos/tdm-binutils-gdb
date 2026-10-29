# TDM with code and data apart: C data, which byte pointers can only
# reach below word 0x8000, starts right after the $mr registers, and
# code goes above it.  .rodata is data, so it moves down with .data.
source_sh ${srcdir}/emulparams/elf32tdm.sh
TEXT_START_ADDR=0x8000
DATA_ADDR=0x20
WRITABLE_RODATA=
