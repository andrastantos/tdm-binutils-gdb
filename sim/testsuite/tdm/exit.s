; Check the EXIT port: the exit status, and that reads and writes with
; bit 15 set (as done by the write-back of a read) don't exit.
# mach: tdm
# status: 7
# output:

	.include "testutils.inc"

	start

	mov	$r0, [EXIT]		; reads 0x8000, the write-back is ignored
	check	$r0, 0x8000
	mov	$r0, [STDOUT]		; the same for STDOUT: prints nothing
	check	$r0, 0x8000
	li	$r0, 0x8001
	mov	[EXIT], $r0		; bit 15 set: ignored
	exit	7
