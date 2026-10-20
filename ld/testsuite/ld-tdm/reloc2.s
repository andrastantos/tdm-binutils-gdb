	.global	small, far_away, ioport
	.set	small, 31
	.set	ioport, 0xfff8
	.data
	.space	3
far_away: .word	7
