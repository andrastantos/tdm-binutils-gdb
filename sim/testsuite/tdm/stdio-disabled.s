; The default stdio device can be removed with --hw-device, so another
; device can take its place.  Without it, EXIT isn't decoded.
# mach: tdm
# sim: --hw-device "/core/tdm_stdio/disabled true"
# xerror:
# output: *unmapped address 0x1ffc0*

	.include "testutils.inc"

	start
	exit	0
