lw $n, array_size($0)

repeat:
	movi $swapped, 0
	movi $i, 1

	for:
		jeq $i, $n, endfor
		addi $1, 1

		addi $i, -1
		lw $tmp1, array($i)
		addi $i, 1
		lw $tmp2, array($i)

		slt $cmp_res, $tmp1, $tmp2
		jeq $cmp_res, $0, dont_swap

		addi $i, -1
		sw $tmp2, array($i)
		addi $i, 1
		sw $tmp1, array($i)

		movi $swapped, 1
		dont_swap: j for

	endfor:
		jeq $swapped, 0, end
		j repeat

end:
	halt

array_size:
	.fill 10

array:
	.fill 5
	.fill 1
	.fill 4
	.fill 2
	.fill 9
	.fill 6
	.fill 5
	.fill 8
	.fill 0
	.fill 7


ram[0] = 16'b1000000010010100;
ram[1] = 16'b0010000100000000;
ram[2] = 16'b0010000110000001;
ram[3] = 16'b1100110010001101;
ram[4] = 16'b0010010010000001;
ram[5] = 16'b0010110111111111;
ram[6] = 16'b1000111000010101;
ram[7] = 16'b0010110110000001;
ram[8] = 16'b1000111010010101;
ram[9] = 16'b0001001011100100;
ram[10] = 16'b1101100000000101;
ram[11] = 16'b0010110111111111;
ram[12] = 16'b1010111010010101;
ram[13] = 16'b0010110110000001;
ram[14] = 16'b1010111000010101;
ram[15] = 16'b0010000100000001;
ram[16] = 16'b0100000000000011;
ram[17] = 16'b1100100000000001;
ram[18] = 16'b0100000000000001;
ram[19] = 16'b0100000000010011;
ram[20] = 16'b0000000000001010;
ram[21] = 16'b0000000000000101;
ram[22] = 16'b0000000000000001;
ram[23] = 16'b0000000000000100;
ram[24] = 16'b0000000000000010;
ram[25] = 16'b0000000000001001;
ram[26] = 16'b0000000000000110;
ram[27] = 16'b0000000000000101;
ram[28] = 16'b0000000000001000;
ram[29] = 16'b0000000000000000;
ram[30] = 16'b0000000000000111;