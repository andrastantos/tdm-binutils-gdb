; LTO's streamed IR is addressed in octets, like the debug sections:
; an odd number of octets isn't padded to a whole word.
	.section .gnu.lto_.main,"e",@progbits
	.ascii	"abc"
