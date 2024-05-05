main:
	movi $1, 0 				# sum
	movi $2, head 			# next_ptr
	movi $3, 0 				# tmp
	loop:
		lw $3, 0($2) 		# load data
		lw $2, 1($2) 		# load next_ptr
		add $1, $1, $3
		jeq $2, $0, done 	# terminate if next_ptr == 0
		j loop
	done:
		halt

chain4:
	.fill 100
	.fill chain5
	.fill 300
chain7:
	.fill 16384
	.fill 909
	.fill 0
chain1:
	.fill 23
	.fill chain2
head: 						# beginning of linked list
	.fill 34
	.fill chain1
	.fill 82
	.fill 10
chain3:
	.fill 4
	.fill chain4
	.fill 229
	.fill 449
chain2:
	.fill 0
	.fill chain3
chain5:
	.fill 12
	.fill 0 				# end of linked list
	.fill 9999
chain6:
	.fill 99
	.fill 49