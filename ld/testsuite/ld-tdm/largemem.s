; The largemem layout: code from 0x8000, C data (.rodata included)
; from 0x20, so byte pointers to data fit and code addresses don't
; need to.
	.text
	.global	_start
_start:	brk
	.word	%bptr(str), %bptr(var), %bptr(zero)
	.section .rodata
str:	.asciz	"hi"
	.data
var:	.word	_start
	.bss
zero:	.word	0
