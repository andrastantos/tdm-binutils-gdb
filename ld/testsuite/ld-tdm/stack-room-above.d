#source: stack-room.s
#ld: -Ttext=0x10 -Tdata=0x20 --defsym=_stack=0x7000
#name: TDM stack room, data above the stack
#warning: .*: warning: the data ends at word 0x0*7e20, above the top of the stack at 0x0*7000$
