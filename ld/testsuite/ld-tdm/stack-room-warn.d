#source: stack-room.s
#ld: -Ttext=0x10 -Tdata=0x20 --stack-room=1024
#name: TDM stack room, too little
#warning: .*: warning: only 478 words between the end of the data at 0x0*7e20 and the top of the stack at 0x0*7ffe \(--stack-room=1024\)$
