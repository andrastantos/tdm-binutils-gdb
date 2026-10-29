; Accessing an address nothing decodes stops the simulator.  0x9000 is
; past the 32K words of default memory (at octet address 0x12000).
# mach: tdm
# xerror:
# output: *unmapped address 0x12000*signal 11*

	.include "testutils.inc"

	start
	li	$r0, 0x9000
	mov	$r1, [$r0]
	pass
