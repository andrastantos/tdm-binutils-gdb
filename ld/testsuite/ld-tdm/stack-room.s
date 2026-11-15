; A program with 0x7e00 words of data: 0x1fe words are left between
; the end of the data and the default top of the stack, 0x7ffe.
	.text
	.global	_start
_start:	brk
	.word	_end
	.data
	.space	0x7e00
