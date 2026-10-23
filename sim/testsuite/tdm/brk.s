; BRK stops the simulator with SIGTRAP.
# mach: tdm
# xerror:
# output: *signal 5 (Trace/breakpoint trap)*

	.include "testutils.inc"

	start
	brk
	pass
