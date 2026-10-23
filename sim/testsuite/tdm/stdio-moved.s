; The stdio device can be moved with --hw-device.  The reg property is
; in octets: 0x1ffd0 is word address 0xffe8.
# mach: tdm
# sim: --hw-device "/core/tdm_stdio/reg 0x1ffd0 6"

	.include "testutils.inc"

	.set	EXIT, -24
	.set	STDOUT, -23

	start
	pass
