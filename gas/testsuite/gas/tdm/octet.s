; Octet data.  .octet emits single octets, packed two per word with the
; first in the low octet, as are consecutive strings.  A half full word
; is padded with a zero octet when anything else follows: a label, other
; data, an instruction, alignment, a section switch or the section end.
	.data
o1:	.octet	1, 2, 3
o2:	.octet	4
	.ascii	"ab"
	.octet	5
	.ascii	"c"
	.word	0x1234
	.octet	6
	.p2align 2
	.octet	7
	.section .rodata
	.octet	8
	.data
	.octet	9
	.text
	.octet	10
start:	mov	$r0, 1
	.octet	11
	brk
	.octet	12
