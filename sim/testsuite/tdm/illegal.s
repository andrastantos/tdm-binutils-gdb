; Opcode 4 is unused: the simulator stops with SIGILL.
# mach: tdm
# xerror:
# output: *signal 4 (Illegal instruction)*

	.include "testutils.inc"

	start
	.word	0x4000
	pass
