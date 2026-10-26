; Byte pointers to local and global data, with octet offsets.
	.text
	.global	_start
_start:	brk
	.section .rodata
	.word	%bptr(str), %bptr(str) + 3, %bptr(ext + 1) + 1
	.data
	.octet	1
str:	.asciz	"hi"
