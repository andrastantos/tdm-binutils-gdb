; Check SWAP, SWAPI and ISTAT.
# mach: tdm

	.include "testutils.inc"

	start

	; Interrupts are disabled after reset, nothing is pending.
	istat	$r0
	check	$r0, 2

	; Exchange a register and memory, in both operand orders.
	li	$sp, buf
	mov	$r0, 9
	swap	$r0, [$sp]
	check	$r0, 0x1111
	mov	$r1, [$sp]
	check	$r1, 9
	swap	[$sp], $r0
	check	$r0, 9
	mov	$r1, [$sp]
	check	$r1, 0x1111

	; SWAP doesn't change INTDIS, SWAPI flips it (in supervisor mode).
	istat	$r0
	check	$r0, 2
	swapi	$r0, [$sp+1]
	istat	$r0
	check	$r0, 0
	swapi	$r0, [$sp+1]
	istat	$r0
	check	$r0, 2

	; ISTAT with a memory destination.
	istat	[$sp+1]
	mov	$r0, [$sp+1]
	check	$r0, 2

	; Swapping the PC is a call: memory gets the return address.
	li	$r0, func
	mov	[$sp], $r0
	swap	$pc, [$sp]
ret:	check	$r1, 0x5555
	mov	$r0, [$sp]
	check	$r0, func_end

	pass

func:	li	$r1, 0x5555
	swap	$pc, [$sp]		; return, memory gets func_end
func_end:
	fail_here

	.data
buf:	.word	0x1111, 0
