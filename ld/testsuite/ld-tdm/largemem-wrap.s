; .text bigger than the whole address space: from 0x8000 it would
; wrap past 0xffff and end above 0x8000 again.
	.text
	.global	_start
_start:	brk
	.space	0x10000
