; All TDM instructions in all operand forms.
	.text
	or	$pc, [$pc+3]
	or	$sp, [$sp-4]
	or	$r0, [$r0]
	or	$r1, [17]
	or	$pc, $pc+5
	or	$sp, $sp-6
	or	$r0, $r0
	or	$r1, -7
	or	[$pc-1], $sp
	or	[$sp+31], $r0
	or	[$r0-32], $r1
	or	[-1], $pc
	and	$pc, [$pc+3]
	and	$sp, [$sp-4]
	and	$r0, [$r0]
	and	$r1, [17]
	and	$pc, $pc+5
	and	$sp, $sp-6
	and	$r0, $r0
	and	$r1, -7
	and	[$pc-1], $sp
	and	[$sp+31], $r0
	and	[$r0-32], $r1
	and	[-1], $pc
	xor	$pc, [$pc+3]
	xor	$sp, [$sp-4]
	xor	$r0, [$r0]
	xor	$r1, [17]
	xor	$pc, $pc+5
	xor	$sp, $sp-6
	xor	$r0, $r0
	xor	$r1, -7
	xor	[$pc-1], $sp
	xor	[$sp+31], $r0
	xor	[$r0-32], $r1
	xor	[-1], $pc
	add	$pc, [$pc+3]
	add	$sp, [$sp-4]
	add	$r0, [$r0]
	add	$r1, [17]
	add	$pc, $pc+5
	add	$sp, $sp-6
	add	$r0, $r0
	add	$r1, -7
	add	[$pc-1], $sp
	add	[$sp+31], $r0
	add	[$r0-32], $r1
	add	[-1], $pc
	sub	$pc, [$pc+3]
	sub	$sp, [$sp-4]
	sub	$r0, [$r0]
	sub	$r1, [17]
	sub	$pc, $pc+5
	sub	$sp, $sp-6
	sub	$r0, $r0
	sub	$r1, -7
	sub	[$pc-1], $sp
	sub	[$sp+31], $r0
	sub	[$r0-32], $r1
	sub	[-1], $pc
	isub	$pc, [$pc+3]
	isub	$sp, [$sp-4]
	isub	$r0, [$r0]
	isub	$r1, [17]
	isub	$pc, $pc+5
	isub	$sp, $sp-6
	isub	$r0, $r0
	isub	$r1, -7
	isub	[$pc-1], $sp
	isub	[$sp+31], $r0
	isub	[$r0-32], $r1
	isub	[-1], $pc
	mov	$pc, [$pc+3]
	mov	$sp, [$sp-4]
	mov	$r0, [$r0]
	mov	$r1, [17]
	mov	$pc, $pc+5
	mov	$sp, $sp-6
	mov	$r0, $r0
	mov	$r1, -7
	mov	[$pc-1], $sp
	mov	[$sp+31], $r0
	mov	[$r0-32], $r1
	mov	[-1], $pc
	swap	$pc, [$pc-1]
	swap	$sp, [$sp+31]
	swap	$r0, [$r0-32]
	swap	$r1, [-1]
	swap	[$sp+2], $r0
	swapi	$pc, [$pc-1]
	swapi	$sp, [$sp+31]
	swapi	$r0, [$r0-32]
	swapi	$r1, [-1]
	swapi	[$sp+2], $r0
	if_eq	$r0, $sp
	if_eq	$r1, [$sp+1]
	if_eq	$sp, 12
	if_eq	[$r0+2], $r1
	if_neq	$r0, $sp
	if_neq	$r1, [$sp+1]
	if_neq	$sp, 12
	if_neq	[$r0+2], $r1
	if_ltu	$r0, $sp
	if_ltu	$r1, [$sp+1]
	if_ltu	$sp, 12
	if_geu	$r0, $sp
	if_geu	$r1, [$sp+1]
	if_geu	$sp, 12
	if_lts	$r0, $sp
	if_lts	$r1, [$sp+1]
	if_lts	$sp, 12
	if_lts	[$r0+2], $r1
	if_ges	$r0, $sp
	if_ges	$r1, [$sp+1]
	if_ges	$sp, 12
	if_ges	[$r0+2], $r1
	if_les	$r0, $sp
	if_les	$r1, [$sp+1]
	if_les	$sp, 12
	if_les	[$r0+2], $r1
	if_gts	$r0, $sp
	if_gts	$r1, [$sp+1]
	if_gts	$sp, 12
	if_gts	[$r0+2], $r1
	istat	$pc
	istat	$sp
	istat	$r0
	istat	$r1
	istat	[$pc-1]
	istat	[$sp+31]
	istat	[$r0-32]
	istat	[-1]
	ror	$pc
	ror	$sp
	ror	$r0
	ror	$r1
	ror	[$pc-1]
	ror	[$sp+31]
	ror	[$r0-32]
	ror	[-1]
	rol	$pc
	rol	$sp
	rol	$r0
	rol	$r1
	rol	[$pc-1]
	rol	[$sp+31]
	rol	[$r0-32]
	rol	[-1]
; Range limits; addresses wrap at 16 bits.
	mov	$r0, -32
	mov	$r0, 31
	mov	$r0, 0xffff
	mov	$r0, [0xffe0]
; Case insensitive, optional whitespace.
	MOV	$SP, [ $R0 + 1 ]
	Add $r1,$pc
