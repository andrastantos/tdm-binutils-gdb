	.global	_start
_start:	mov	$r0, far			; R_TDM_IMM6 overflow
	mov	$r0, [$pc + (konst - .)]	; R_TDM_PCREL6 overflow
	.section .rodata
	.space	40
konst:	.word	1
	.data
far:	.word	1
