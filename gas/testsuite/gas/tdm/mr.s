; Memory pseudo-registers: $mr0..$mr31 are aliases of [0]..[31], in
; every operand position.  The disassembler prints them for [0]..[31].
	.text
	mov	$r0, $mr5
	mov	$r0, [5]
	mov	$mr31, $r1
	mov	[31], $r1
	add	$mr0, $sp
	sub	$MR7, $r0
	if_lts	$mr2, $r0
	if_eq	$r0, $mr2
	swap	$pc, $mr1
	swapi	$mr1, $pc
	rol	$mr3
	istat	$mr4
	mov	$r0, [-1]
	mov	$r0, $mr08
