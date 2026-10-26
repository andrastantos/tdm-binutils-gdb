#source: bptr-overflow.s
#ld: -Ttext=0x1000 -Tdata=0x7fff
#name: TDM byte pointer overflow
#error: .*\(\.rodata\+0x1\): relocation truncated to fit: R_TDM_BYTEPTR16 against `a'$
