# $1 = n
# $2 = swapped
# $3 = i
# $4 = tmp1
# $5 = tmp2
# $6 = cmp_res

lw $1, array_size($0)

repeat:
    movi $2, 0
    movi $3, 1

    for:
        jeq $3, $1, endfor
        addi $1, $1, 1

        addi $3, $3, -1
        lw $4, array($3)
        addi $3, $3, 1
        lw $5, array($3)

        slt $6, $5, $4
        jeq $6, $0, dont_swap

        addi $3, $3, -1
        sw $5, array($3)
        addi $3, $3, 1
        sw $4, array($3)

        movi $2, 1
        dont_swap:
            j for

    endfor:
        jeq $2, $0, end
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
