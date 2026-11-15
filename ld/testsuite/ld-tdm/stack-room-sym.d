#source: stack-room.s
#ld: -Ttext=0x10 -Tdata=0x20 --defsym=_stack=0x7e80
#name: TDM stack room, _stack defined
#warning: .*: warning: only 96 words between the end of the data at 0x0*7e20 and the top of the stack at 0x0*7e80 \(--stack-room=256\)$
