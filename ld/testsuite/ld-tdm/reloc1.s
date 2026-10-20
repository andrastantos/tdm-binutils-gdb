	.text
	.global	_start
_start:
	mov	$r0, [$pc + (konst - .)]	; R_TDM_PCREL6
	mov	$r1, [ioport]			; R_TDM_IMM6, wraps to -8
	mov	$sp, small			; R_TDM_IMM6
	mov	$pc, $pc
	.section .rodata
konst:	.word	far_away			; R_TDM_16
	.long	far_away + 1			; R_TDM_32
