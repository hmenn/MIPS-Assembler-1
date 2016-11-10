	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.word	0x12341
	.text
main:
	lui	$s3, 0x1000
	lw	$s5, 0($s3)
	lw	$t0, 4($s3)
	lw	$t1, 8($s3)
	lw	$t5, 12($s3)
	addiu	$s5, $s5, 24
	addiu	$s6, $zero, 124
	addu	$s7, $s5, $s6
	sw	$s5, 16($s3)
	sw	$s6, 20($s3)
	sw	$s7, 24($s3)
	addiu	$s3, $s3, 12
	lw	$t2, -4($s3)
	lw	$t3, -8($s3)
	lw	$t4, -12($s3)
