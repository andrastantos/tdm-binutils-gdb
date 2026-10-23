; DWARF line information through the linker: line table addresses are
; relocated to absolute word addresses.
	.file	1 "dwarf-line.c"
	.text
	.global	f
f:	.loc	1 10
	mov	$r0, 3
	add	$r0, $r0
	.loc	1 11
	mov	$r1, [$pc + (k - .)]
	.loc	1 12
	mov	$pc, [$sp]
k:	.word	0x1234
