; Instructions passed through macro arguments: macro expansion drops the
; space between the mnemonic and a leading '['.
	.macro	emit insn:vararg
	\insn
	.endm

	.text
	emit	mov [$sp+1], $r0
	emit	if_lts [$sp], $r0
	emit	add $r1, [$pc-1]
