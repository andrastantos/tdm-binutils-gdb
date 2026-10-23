; Helpers for the TDM simulator tests.
;
; Tests print "pass" (or "fail") through the simulator's stdio device and
; exit with status 0 (or 1).  Immediates only reach -32..31, so constants
; are stored next to the code and loaded PC-relative.

	.set	EXIT, -32		; tdm_stdio registers at 0xffe0
	.set	STDOUT, -31
	.set	STDIN, -30

; Load a 16-bit constant (or address) into REG.
	.macro	li reg, value
	mov	\reg, [$pc + 2]
	mov	$pc, $pc + 2
	.word	\value
	.endm

; The predicate INSN must hold (the next instruction executes).
	.macro	taken insn:vararg
	\insn
	mov	$pc, $pc + 3
	mov	$pc, [$pc + 1]
	.word	fail
	.endm

; The predicate INSN must not hold (the next instruction is skipped).
	.macro	not_taken insn:vararg
	\insn
	mov	$pc, [$pc + 2]
	mov	$pc, $pc + 2
	.word	fail
	.endm

; Print the character in WORD (an assembly time constant).  Clobbers $r1.
	.macro	putc word
	mov	$r1, [$pc + 2]
	mov	$pc, $pc + 2
	.word	\word
	mov	[STDOUT], $r1
	.endm

; Exit with status CODE.  Clobbers $r1.
	.macro	exit code
	mov	$r1, \code
	mov	[EXIT], $r1
	mov	$pc, $pc
	.endm

; Every test starts with this.  It also provides "fail", which prints
; "fail" and exits with status 1.
	.macro	start
	.text
	.global	_start
	mov	$pc, $pc + (_start - .)
fail:	putc	'f'
	putc	'a'
	putc	'i'
	putc	'l'
	putc	'\n'
	exit	1
_start:
	.endm

; Print "pass" and exit with status 0.
	.macro	pass
	putc	'p'
	putc	'a'
	putc	's'
	putc	's'
	putc	'\n'
	exit	0
	.endm

; Jump to fail unless REG equals VALUE (any 16-bit constant).
	.macro	check reg, value
	if_neq	\reg, [$pc + 4]
	mov	$pc, [$pc + 2]
	mov	$pc, $pc + 3
	.word	fail
	.word	\value
	.endm

; Jump to fail unconditionally.
	.macro	fail_here
	mov	$pc, [$pc + 1]
	.word	fail
	.endm
