; Data directives.  A TDM byte is 16 bits: .byte, .space, .zero and the
; alignment directives count words.  Strings pack two characters per
; word, first character in the low octet, padded to a whole word.
	.data
	.byte	1, -1
	.word	0x1234, -2
	.long	0x12345678
	.ascii	"abc"
	.asciz	"de"
	.string	"fgh"
	.space	2
	.space	2, 0x55
	.zero	1
	.balign	4
	.word	0xa4
	.p2align 3
	.word	0xa8
	.balignw 4, 0x1780
	.word	0xb4
	.org	0x20
	.word	0x20
