#source: overflow.s
#ld: -Ttext=0x1000
#name: TDM relocation overflow
#error: .*relocation truncated to fit: R_TDM_IMM6 against `\.data'\n.*relocation truncated to fit: R_TDM_PCREL6 against `\.rodata'
