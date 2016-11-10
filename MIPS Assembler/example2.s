	.data
array:	.word	3
	.word	123
	.word	4346
array2:	.word	0x11111111
	.text
main:
	addiu	$t2, $zero, 1024
	addu	$t3, $t2, $t2
	or	$s4, $t3, $t2
	sll	$t6, $s5, 16
	addiu	$t7, $t6, 9999
	subu	$t8, $t7, $t2
	nor	$t0, $s4, $t3
	ori	$s0, $t2, 255
	srl	$s1, $t6, s5
	la	$s4, array2
	and	$s2, $s1, $s5
	andi	$s6, $s4, 100
	lui	$s7, 100
	addiu	$t2, $zero, 0xa