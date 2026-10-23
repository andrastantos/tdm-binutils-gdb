; Check ROR and ROL, on registers and memory.
# mach: tdm

	.include "testutils.inc"

	start

	li	$r0, 0x8001
	ror	$r0
	check	$r0, 0xc000
	li	$r0, 0x8001
	rol	$r0
	check	$r0, 0x0003

	li	$sp, buf
	ror	[$sp]
	mov	$r0, [$sp]
	check	$r0, 0x8000
	rol	[$sp+1]
	mov	$r0, [$sp+1]
	check	$r0, 0x0001

	pass

	.data
buf:	.word	0x0001, 0x8000
