; DWARF line information.  TDM is word addressed: line table addresses,
; low_pc and high_pc count 16-bit words, while relocations in the debug
; sections (which are addressed in octets) have octet offsets.
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
