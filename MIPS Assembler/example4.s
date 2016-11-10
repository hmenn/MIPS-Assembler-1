	.data
	.text
main:
	addu	$s2, $t4, $t5
	addu	$s2, $t6, $t7
	subu	$s0, $3, $s2
lab1:
	and	$s4, $s4, $zero
	addiu	$s1, $s1, 0x1
	or	$t6, $t6, $zero
	jal	lab3
lab3:
	sll	$t7, $t6, 2
	srl	$t5, $t4, 2
	sltiu	$s0, $s1, 100
	beq	$s0, $zero, lab4
	jr	$ra
lab4:
	sltu	$t4, $s2, $3
	bne	$t4, $zero, lab5
	j	lab1
lab5:
	ori	$s6, $s6, 0xf0f0
