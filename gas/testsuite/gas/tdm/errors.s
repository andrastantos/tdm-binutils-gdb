; Invalid instructions.
	mov	$r0, 32
	mov	$r0, -33
	mov	$r0, 0xffdf
	mov	$r0, [$r1+1]
	mov	$r0, $r1
	mov	$r0, [$sp 1]
	swap	$r0, 5
	swapi	$r0, $sp
	if_ltu	[$sp], $r0
	if_geu	[$sp], $r0
	mov	5, $r0
	mov	$r0
	rol	$r0, $r1
	frob	$r0, 1
	mov	$r0, [$sp+1
	mov	$r2, 1
	brk	256
	brk	x
	brk	1, 2
	mov	$r0, $mr32
	add	$mr1, $mr2
	mov	$r0, $mr5+1
