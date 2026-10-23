; Check the addressing modes of operand B.
# mach: tdm

	.include "testutils.inc"

	start

	; Register plus immediate.  $pc is the address of the instruction.
here:	mov	$r0, $pc+5
	check	$r0, here+5
	li	$sp, 0x0100
	mov	$r0, $sp-1
	check	$r0, 0x00ff
	mov	$r1, $r0+31
	check	$r1, 0x011e
	mov	$r1, $sp
	check	$r1, 0x0100

	; Memory at register plus immediate.
	li	$sp, buf
	mov	$r0, [$sp]
	check	$r0, 0x1111
	mov	$r0, [$sp+2]
	check	$r0, 0x3333
	li	$sp, buf+3
	mov	$r0, [$sp-1]
	check	$r0, 0x3333
	li	$r0, buf
	mov	$sp, [$r0+1]
	check	$sp, 0x2222
there:	mov	$sp, [$pc + (konst - .)]
	check	$sp, 0x4444
	mov	$pc, $pc+2
konst:	.word	0x4444

	; Absolute memory: the low and high ends of the address space.
	mov	$r0, 7
	mov	[5], $r0
	mov	$sp, [5]
	check	$sp, 7

	pass

	.data
buf:	.word	0x1111, 0x2222, 0x3333
