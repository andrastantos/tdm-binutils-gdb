#objdump: -d
#name: TDM instructions
#source: insns.s

.*: +file format elf32-tdm
Disassembly of section \.text:
00000000 <\.text>:
   0:	1003 	or	\$pc, \[\$pc\+3\]	; 0x3
   1:	117c 	or	\$sp, \[\$sp-4\]
   2:	1280 	or	\$r0, \[\$r0\]
   3:	13d1 	or	\$r1, \$mr17
   4:	1405 	or	\$pc, \$pc\+5	; 0x9
   5:	157a 	or	\$sp, \$sp-6
   6:	1680 	or	\$r0, \$r0
   7:	17f9 	or	\$r1, -7
   8:	187f 	or	\[\$pc-1\], \$sp	; 0x7
   9:	199f 	or	\[\$sp\+31\], \$r0
   a:	1ae0 	or	\[\$r0-32\], \$r1
   b:	1b3f 	or	\[-1\], \$pc
   c:	2003 	and	\$pc, \[\$pc\+3\]	; 0xf
   d:	217c 	and	\$sp, \[\$sp-4\]
   e:	2280 	and	\$r0, \[\$r0\]
   f:	23d1 	and	\$r1, \$mr17
  10:	2405 	and	\$pc, \$pc\+5	; 0x15
  11:	257a 	and	\$sp, \$sp-6
  12:	2680 	and	\$r0, \$r0
  13:	27f9 	and	\$r1, -7
  14:	287f 	and	\[\$pc-1\], \$sp	; 0x13
  15:	299f 	and	\[\$sp\+31\], \$r0
  16:	2ae0 	and	\[\$r0-32\], \$r1
  17:	2b3f 	and	\[-1\], \$pc
  18:	3003 	xor	\$pc, \[\$pc\+3\]	; 0x1b
  19:	317c 	xor	\$sp, \[\$sp-4\]
  1a:	3280 	xor	\$r0, \[\$r0\]
  1b:	33d1 	xor	\$r1, \$mr17
  1c:	3405 	xor	\$pc, \$pc\+5	; 0x21
  1d:	357a 	xor	\$sp, \$sp-6
  1e:	3680 	xor	\$r0, \$r0
  1f:	37f9 	xor	\$r1, -7
  20:	387f 	xor	\[\$pc-1\], \$sp	; 0x1f
  21:	399f 	xor	\[\$sp\+31\], \$r0
  22:	3ae0 	xor	\[\$r0-32\], \$r1
  23:	3b3f 	xor	\[-1\], \$pc
  24:	5003 	add	\$pc, \[\$pc\+3\]	; 0x27
  25:	517c 	add	\$sp, \[\$sp-4\]
  26:	5280 	add	\$r0, \[\$r0\]
  27:	53d1 	add	\$r1, \$mr17
  28:	5405 	add	\$pc, \$pc\+5	; 0x2d
  29:	557a 	add	\$sp, \$sp-6
  2a:	5680 	add	\$r0, \$r0
  2b:	57f9 	add	\$r1, -7
  2c:	587f 	add	\[\$pc-1\], \$sp	; 0x2b
  2d:	599f 	add	\[\$sp\+31\], \$r0
  2e:	5ae0 	add	\[\$r0-32\], \$r1
  2f:	5b3f 	add	\[-1\], \$pc
  30:	6003 	sub	\$pc, \[\$pc\+3\]	; 0x33
  31:	617c 	sub	\$sp, \[\$sp-4\]
  32:	6280 	sub	\$r0, \[\$r0\]
  33:	63d1 	sub	\$r1, \$mr17
  34:	6405 	sub	\$pc, \$pc\+5	; 0x39
  35:	657a 	sub	\$sp, \$sp-6
  36:	6680 	sub	\$r0, \$r0
  37:	67f9 	sub	\$r1, -7
  38:	787f 	sub	\[\$pc-1\], \$sp	; 0x37
  39:	799f 	sub	\[\$sp\+31\], \$r0
  3a:	7ae0 	sub	\[\$r0-32\], \$r1
  3b:	7b3f 	sub	\[-1\], \$pc
  3c:	7003 	isub	\$pc, \[\$pc\+3\]	; 0x3f
  3d:	717c 	isub	\$sp, \[\$sp-4\]
  3e:	7280 	isub	\$r0, \[\$r0\]
  3f:	73d1 	isub	\$r1, \$mr17
  40:	7405 	isub	\$pc, \$pc\+5	; 0x45
  41:	757a 	isub	\$sp, \$sp-6
  42:	7680 	isub	\$r0, \$r0
  43:	77f9 	isub	\$r1, -7
  44:	687f 	isub	\[\$pc-1\], \$sp	; 0x43
  45:	699f 	isub	\[\$sp\+31\], \$r0
  46:	6ae0 	isub	\[\$r0-32\], \$r1
  47:	6b3f 	isub	\[-1\], \$pc
  48:	8003 	mov	\$pc, \[\$pc\+3\]	; 0x4b
  49:	817c 	mov	\$sp, \[\$sp-4\]
  4a:	8280 	mov	\$r0, \[\$r0\]
  4b:	83d1 	mov	\$r1, \$mr17
  4c:	8405 	mov	\$pc, \$pc\+5	; 0x51
  4d:	857a 	mov	\$sp, \$sp-6
  4e:	8680 	mov	\$r0, \$r0
  4f:	87f9 	mov	\$r1, -7
  50:	887f 	mov	\[\$pc-1\], \$sp	; 0x4f
  51:	899f 	mov	\[\$sp\+31\], \$r0
  52:	8ae0 	mov	\[\$r0-32\], \$r1
  53:	8b3f 	mov	\[-1\], \$pc
  54:	083f 	swap	\$pc, \[\$pc-1\]	; 0x53
  55:	095f 	swap	\$sp, \[\$sp\+31\]
  56:	0aa0 	swap	\$r0, \[\$r0-32\]
  57:	0bff 	swap	\$r1, \[-1\]
  58:	0982 	swap	\$r0, \[\$sp\+2\]
  59:	003f 	swapi	\$pc, \[\$pc-1\]	; 0x58
  5a:	015f 	swapi	\$sp, \[\$sp\+31\]
  5b:	02a0 	swapi	\$r0, \[\$r0-32\]
  5c:	03ff 	swapi	\$r1, \[-1\]
  5d:	0182 	swapi	\$r0, \[\$sp\+2\]
  5e:	c580 	if_eq	\$r0, \$sp
  5f:	c1c1 	if_eq	\$r1, \[\$sp\+1\]
  60:	c74c 	if_eq	\$sp, 12
  61:	c2c2 	if_eq	\$r1, \[\$r0\+2\]
  62:	cd80 	if_neq	\$r0, \$sp
  63:	c9c1 	if_neq	\$r1, \[\$sp\+1\]
  64:	cf4c 	if_neq	\$sp, 12
  65:	cac2 	if_neq	\$r1, \[\$r0\+2\]
  66:	d580 	if_ltu	\$r0, \$sp
  67:	d1c1 	if_ltu	\$r1, \[\$sp\+1\]
  68:	d74c 	if_ltu	\$sp, 12
  69:	dd80 	if_geu	\$r0, \$sp
  6a:	d9c1 	if_geu	\$r1, \[\$sp\+1\]
  6b:	df4c 	if_geu	\$sp, 12
  6c:	e580 	if_lts	\$r0, \$sp
  6d:	e1c1 	if_lts	\$r1, \[\$sp\+1\]
  6e:	e74c 	if_lts	\$sp, 12
  6f:	fac2 	if_gts	\$r1, \[\$r0\+2\]
  70:	ed80 	if_ges	\$r0, \$sp
  71:	e9c1 	if_ges	\$r1, \[\$sp\+1\]
  72:	ef4c 	if_ges	\$sp, 12
  73:	f2c2 	if_les	\$r1, \[\$r0\+2\]
  74:	f580 	if_les	\$r0, \$sp
  75:	f1c1 	if_les	\$r1, \[\$sp\+1\]
  76:	f74c 	if_les	\$sp, 12
  77:	eac2 	if_ges	\$r1, \[\$r0\+2\]
  78:	fd80 	if_gts	\$r0, \$sp
  79:	f9c1 	if_gts	\$r1, \[\$sp\+1\]
  7a:	ff4c 	if_gts	\$sp, 12
  7b:	e2c2 	if_lts	\$r1, \[\$r0\+2\]
  7c:	9700 	istat	\$pc
  7d:	9740 	istat	\$sp
  7e:	9780 	istat	\$r0
  7f:	97c0 	istat	\$r1
  80:	983f 	istat	\[\$pc-1\]	; 0x7f
  81:	991f 	istat	\[\$sp\+31\]
  82:	9a20 	istat	\[\$r0-32\]
  83:	9b3f 	istat	\[-1\]
  84:	a700 	ror	\$pc
  85:	a740 	ror	\$sp
  86:	a780 	ror	\$r0
  87:	a7c0 	ror	\$r1
  88:	a83f 	ror	\[\$pc-1\]	; 0x87
  89:	a91f 	ror	\[\$sp\+31\]
  8a:	aa20 	ror	\[\$r0-32\]
  8b:	ab3f 	ror	\[-1\]
  8c:	b700 	rol	\$pc
  8d:	b740 	rol	\$sp
  8e:	b780 	rol	\$r0
  8f:	b7c0 	rol	\$r1
  90:	b83f 	rol	\[\$pc-1\]	; 0x8f
  91:	b91f 	rol	\[\$sp\+31\]
  92:	ba20 	rol	\[\$r0-32\]
  93:	bb3f 	rol	\[-1\]
  94:	87a0 	mov	\$r0, -32
  95:	879f 	mov	\$r0, 31
  96:	87bf 	mov	\$r0, -1
  97:	83a0 	mov	\$r0, \[-32\]
  98:	8241 	mov	\$sp, \[\$r0\+1\]
  99:	54c0 	add	\$r1, \$pc	; 0x99
