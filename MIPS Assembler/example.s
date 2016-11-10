	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.text
main:
	and	$t0, $t0, $zero
	and	$t1, $t1, $zero
	la	$s0, data1
	la	$s1, data2
	and	$t2, $t2, $zero
lab1:
	and	$t3, $t3, $zero
lab2:
	addiu	$t0, $t0, 0x1
	addiu	$t3, $t3, 0x1
	or	$s1, $s1, $zero
	bne	$t3, $s0, lab2
lab3:
	addiu	$t1, $t1, 0x2
	addiu	$t3, $t3, 1
	sll	$t1, $t0, 1
	srl	$t0, $t1, 1
	and	$t4, $t0, $t1
	bne	$t3, $s1, lab3
lab4:
	addu	$t5, $t5, $ra
	nor	$t7, $t0, $t1
	beq	$t2, $s0, lab5
	j	lab1
lab5:
	ori	$t7, $t7, 0xf0f0
