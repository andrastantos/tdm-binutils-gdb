; Relocations.  Symbol values, '.' and relocation offsets are in words.
	.text
	.global	_start
_start:
	mov	$r0, [$pc + (konst - .)]	; other section: R_TDM_PCREL6
	mov	$r1, [ioport]			; R_TDM_IMM6
	mov	$sp, small			; R_TDM_IMM6
	add	$r0, [$pc + (local - .)]	; same section: resolved here
	mov	$pc, $pc + (_start - .)
local:	.word	0x1234
	.section .rodata
	.word	0xdead
konst:	.word	far_away			; R_TDM_16
	.long	far_away + 1			; R_TDM_32
	.word	.				; R_TDM_16 against .rodata
