; Accessing an address nothing decodes stops the simulator.  0x5000 is
; past the 16K words of default memory (at octet address 0xa000).
# mach: tdm
# xerror:
# output: *unmapped address 0xa000*signal 11*

	.include "testutils.inc"

	start
	li	$r0, 0x5000
	mov	$r1, [$r0]
	pass
