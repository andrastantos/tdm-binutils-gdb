#source: stack-room.s
#ld: -Ttext=0x10 -Tdata=0x20 --defsym=_stack=0x7e80 --stack-room=0
#name: TDM stack room, check turned off
#nm: -n
#...
00007e20 . _end
#pass
