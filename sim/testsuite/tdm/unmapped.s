; Accessing an address nothing decodes stops the simulator.  0xfff0 is
; between the stdio device and the CPU's own registers (at octet address
; 0x1ffe0).
# mach: tdm
# xerror:
# output: *unmapped address 0x1ffe0*signal 11*

	.include "testutils.inc"

	start
	li	$r0, 0xfff0
	mov	$r1, [$r0]
	pass
