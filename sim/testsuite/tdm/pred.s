; Check the predicates, in both operand orders.
# mach: tdm

	.include "testutils.inc"

	start

	mov	$r0, 3
	mov	$sp, -3			; 0xfffd: 65533 unsigned, -3 signed

	taken		if_eq $r0, 3
	not_taken	if_eq $r0, $sp
	taken		if_neq $r0, $sp
	not_taken	if_neq $r0, $r0

	taken		if_ltu $r0, $sp
	not_taken	if_ltu $sp, $r0
	not_taken	if_ltu $r0, $r0
	taken		if_geu $sp, $r0
	taken		if_geu $r0, $r0
	not_taken	if_geu $r0, $sp

	not_taken	if_lts $r0, $sp
	taken		if_lts $sp, $r0
	not_taken	if_lts $r0, $r0
	taken		if_ges $r0, $sp
	taken		if_ges $r0, $r0
	not_taken	if_ges $sp, $r0
	taken		if_les $sp, $r0
	taken		if_les $r0, $r0
	not_taken	if_les $r0, $sp
	taken		if_gts $r0, $sp
	not_taken	if_gts $r0, $r0
	not_taken	if_gts $sp, $r0

	; Signed overflow: 0x8000 < 0x7fff.
	li	$r0, 0x8000
	li	$sp, 0x7fff
	taken		if_lts $r0, $sp
	not_taken	if_ltu $r0, $sp

	; Memory operand first: the assembler encodes the mirrored condition.
	li	$sp, val		; val = -3
	mov	$r0, 3
	taken		if_lts [$sp], $r0
	not_taken	if_gts [$sp], $r0
	taken		if_les [$sp], $r0
	not_taken	if_ges [$sp], $r0
	taken		if_eq [$sp+1], $r0
	taken		if_neq [$sp], $r0
	taken		if_ges [$sp+1], $r0
	taken		if_les [$sp+1], $r0

	pass

	.data
val:	.word	-3
	.word	3
