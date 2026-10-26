; Byte pointers reach the first 32K words: with A at word 0x7fff,
; %bptr(a) + 1 is 0xffff, %bptr(a) + 2 doesn't fit.
	.text
	.global	_start
_start:	brk
	.section .rodata
	.word	%bptr(a) + 1
	.word	%bptr(a) + 2
	.data
a:	.word	0
