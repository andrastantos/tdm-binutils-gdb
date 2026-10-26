; Invalid byte pointers.
	.data
x:	.word	0
	.long	%bptr(x)
	.word	%bptr x
	.word	%bptr(x
	.word	%bptr(x) + y
	.word	%bptr(x - y)
	.octet	%bptr(x)
