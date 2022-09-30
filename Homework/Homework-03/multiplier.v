/*          OPCODE SRC  DST  IMMDATA */
myROM[0] = {movi, RXX, Rg0, 4'b0111};
myROM[1] = {movi, RXX, Rg1, 4'b0010};
myROM[2] = {movi, RXX, Rg2, 4'b0000}; // Rg2: Result
myROM[3] = {movi, RXX, Rg3, 4'b0000}; // Rg3: Loop Counter
myROM[4] = {add,  Rg0, Rg2, 4'b0000}; // LOOP BEGINS
myROM[5] = {addi, RXX, Rg3, 4'b0001};
myROM[6] = {cmp,  Rg3, Rg1, 4'b0000};
myROM[7] = {jz,   RXX, RXX, 4'b0010};
myROM[8] = {jmp,  RXX, RXX, 4'b1100}; // LOOP ENDS
myROM[9] = {jmp,  RXX, RXX, 4'b0000};
myROM[10] = {jmp,  RXX, RXX, 4'b0000};
myROM[11] = {jmp,  RXX, RXX, 4'b0000};
myROM[12] = {jmp,  RXX, RXX, 4'b0000};
myROM[13] = {jmp,  RXX, RXX, 4'b0000};
myROM[14] = {jmp,  RXX, RXX, 4'b0000};
myROM[15] = {jmp,  RXX, RXX, 4'b0000};

/*
7 * 2 = 14
Created a loop that does 2 (Rg1) times of addition of 7 (Rg0).
*/
