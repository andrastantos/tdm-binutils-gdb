; BRK: 0b0xxx_1_111_xxxxxxxx (binary ops with a memory destination and an
; immediate operand B, which do nothing).  The assembler emits the OR form.
	.text
	brk
	brk	5
	brk	255
	.word	0x0f00, 0x2fab, 0x4000
