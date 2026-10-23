; Check the binary operations, with register and memory destinations.
# mach: tdm

	.include "testutils.inc"

	start

	; Register destination.
	li	$sp, 0x00ff
	li	$r0, 0x1234
	or	$r0, $sp
	check	$r0, 0x12ff
	li	$r0, 0x1234
	and	$r0, $sp
	check	$r0, 0x0034
	li	$r0, 0x1234
	xor	$r0, $sp
	check	$r0, 0x12cb
	li	$r0, 0x1234
	add	$r0, $sp
	check	$r0, 0x1333
	li	$r0, 0x1234
	sub	$r0, $sp
	check	$r0, 0x1135
	li	$r0, 0x1234
	isub	$r0, $sp
	check	$r0, 0xeecb

	; Immediates, 16-bit wrap around.
	li	$r0, 0xffff
	add	$r0, 1
	check	$r0, 0
	sub	$r0, 1
	check	$r0, 0xffff
	mov	$r0, -32
	check	$r0, 0xffe0
	mov	$r0, 31
	check	$r0, 31
	mov	$r0, 0xffe0		; same as -32
	check	$r0, 0xffe0

	; Memory destination: the memory operand comes first.
	li	$sp, buf
	mov	$r0, 3
	add	[$sp], $r0		; 5 + 3
	mov	$r1, [$sp]
	check	$r1, 8
	sub	[$sp], $r0		; 8 - 3
	mov	$r1, [$sp]
	check	$r1, 5
	isub	[$sp], $r0		; 3 - 5
	mov	$r1, [$sp]
	check	$r1, 0xfffe
	or	[$sp+1], $r0		; 0x00f0 | 3
	mov	$r1, [$sp+1]
	check	$r1, 0x00f3
	and	[$sp+1], $r0		; 0x00f3 & 3
	mov	$r1, [$sp+1]
	check	$r1, 3
	xor	[$sp+1], $r0		; 3 ^ 3
	mov	$r1, [$sp+1]
	check	$r1, 0
	mov	[$sp+1], $sp
	mov	$r1, [$sp+1]
	check	$r1, buf

	; A memory source leaves memory alone.
	mov	$r0, 1
	add	$r0, [$sp]
	check	$r0, 0xffff
	mov	$r1, [$sp]
	check	$r1, 0xfffe

	pass

	.data
buf:	.word	5
	.word	0x00f0
