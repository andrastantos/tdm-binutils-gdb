; .text fills most of the 32K words above 0x8000.  Section sizes are in
; octets and addresses in words, so this mustn't count as wrapping
; around the address space.
	.text
	.global	_start
_start:	brk
	.space	0x7000
end:	brk
