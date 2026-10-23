; Check the MMU (user mode, translation, access violations) and the
; interrupt controller that reports them.
# mach: tdm

	.include "testutils.inc"

	.set	MMU, -1
	.set	INTC, -2
	; MMU control register: offset[15:9] limit[8:2] user[1] clear AV[0].
	.set	LIMIT, 9		; user code may use pages 0..9 (0x0000..0x13ff)
	.set	USER, (LIMIT << 2) | 2 | 1

	start

	li	$r0, USER
	mov	[MMU], $r0
	li	$r0, 0x01ff		; enable source 0 (AV), clear pending
	mov	[INTC], $r0

	; 1. Data access violation, through the syscall instruction.
	li	$r0, handler1
	mov	[1], $r0
	li	$r0, user1
	mov	[2], $r0
	swapi	$pc, [2]

user1:	mov	$r0, -1			; immediates above the limit are fine
	mov	$r1, [$pc + (kffff - .)]	; and so are values read
sys1:	mov	$r0, [MMU]		; page 127: AV
	fail_here
kffff:	.word	0xffff

handler1:
	istat	$sp			; interrupts disabled, request pending
	check	$sp, 3
	mov	$sp, [1]
	check	$sp, sys1 + 1
	check	$r0, 0xffff		; the faulting load didn't happen
	check	$r1, 0xffff
	mov	$r0, [INTC]		; enable mask | pending (and acknowledge)
	check	$r0, 0x0101
	mov	$r0, 1
	mov	[MMU], $r0		; clear AV, leave user mode
	mov	$r0, [INTC]		; acknowledge what was left pending
	mov	$r0, [INTC]
	check	$r0, 0x0100		; mask kept, nothing pending

	; 2. Fetch access violation.
	li	$r0, handler2
	mov	[1], $r0
	li	$r0, USER
	mov	[MMU], $r0
	li	$r0, user2
	mov	[2], $r0
	swapi	$pc, [2]

user2:	li	$sp, 0x1400		; page 10
	mov	$pc, $sp

handler2:
	mov	$sp, [1]
	check	$sp, 0x1401
	mov	$r0, 1
	mov	[MMU], $r0
	mov	$r0, [INTC]

	; 3. Translation: with an offset of one page, the user code runs at
	; its address - 0x200.  The very first user mode fetch is translated
	; too.
	li	$r0, handler3
	mov	[1], $r0
	li	$r0, (1 << 9) | USER
	mov	[MMU], $r0
	li	$r0, user3 - 0x200
	mov	[2], $r0
	swapi	$pc, [2]

user3:	mov	$r0, $pc		; the logical address
	mov	$r1, [$pc + (k1234 - .)]	; PC-relative data is translated too
sys3:	mov	$sp, [MMU]
	fail_here
k1234:	.word	0x1234

handler3:
	mov	$sp, [1]
	check	$sp, sys3 - 0x200 + 1
	check	$r0, user3 - 0x200
	check	$r1, 0x1234

	pass
