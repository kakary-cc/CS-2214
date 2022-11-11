# $1: product
# $2: multiplicand, also temp
# $3: multiplier
# $4: add loop counter, 0 ~ (multiplier - 1)
# $5: loop flag, break if set to 0
# $6: bit mask, 0b1 ~ 0b1000000
# $7: bit loop counter, 0 ~ (7 - 1)

movi $1, 0
lw $2, multiplicand($0)
lw $3, multiplier($0)
movi $6, 1
movi $7, 0

bit_loop:

	movi $4, 0

	add_loop:
		slt $5, $4, $3				# flag = counter < multiplier
		jeq $5, $0, add_end			# break if loop counter >= product
		and $2, $2, $6				# temp ($2) = multiplicand & bitmask
		add $1, $1, $2				# product += temp
		lw $2, multiplicand($0)		# restore $2
		addi $4, $4, 1 				# counter += 1
		j add_loop
	add_end:

	slti $5, $7, 7			# flag = counter < 7
	jeq $5, $0, bit_end		# break if counter >= 7
	add $6, $6, $6			# left shift by 1 bit
	addi $7, $7, 1			# counter += 1
	j bit_loop
	
bit_end:
	halt

multiplicand:
	.fill 123
multiplier:
	.fill 119