; A byte pointer below zero wraps around: with A at word 0x2000,
; %bptr(a) - 0x4000 is 0, %bptr(a) - 0x4001 is 0xffff and
; %bptr(a) - 0x7600 is 0xca00.
	.text
	.global	_start
_start:	brk
	.section .rodata
	.word	%bptr(a) - 0x4000
	.word	%bptr(a) - 0x4001
	.word	%bptr(a) - 0x7600
	.word	%bptr(b) - 0x7600
	.data
a:	.word	0
b:	.word	0
