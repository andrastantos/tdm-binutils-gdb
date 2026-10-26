; Byte pointers: %bptr(EXPR) is twice the word address EXPR, plus an
; optional offset in octets.
	.data
str:	.asciz	"hello"
	.section .rodata
	.word	%bptr(str)
	.word	%bptr(str) + 3
	.word	%bptr(str + 1) - 1
	.word	%bptr(ext)
	.word	%BPTR (.Lloc)
	.word	%bptr(0x100) + 1
	.equ	abs, 0x20
	.word	%bptr(abs)
	.data
.Lloc:	.word	0
