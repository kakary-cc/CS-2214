/*          OPCODE SRC  DST  IMMDATA */
myROM[0] = {movi, RXX, Rg0, 4'b0111};  // Mov 7 to Rg0
myROM[1] = {movi, RXX, Rg1, 4'b0010};  // Mov 2 to Rg1
myROM[2] = {movi, RXX, Rg3, 4'b1111};  // Mov MAX to Rg3
myROM[3] = {cmp,  Rg0, Rg1, 4'b0000};  // Loop Begins Here
myROM[4] = {jz,   RXX, RXX, 4'b0110};  // Jump to: Output True
myROM[5] = {cmp,  Rg0, Rg3, 4'b0000};
myROM[6] = {addi, RXX, Rg0, 4'b0001};
myROM[7] = {jnz,  RXX, RXX, 4'b1100};
myROM[8] = {movi, RXX, Rg2, 4'b0000};  // Output False
myROM[9] = {jmp,  RXX, RXX, 4'b0000};  // False Halt
myROM[10] = {movi, RXX, Rg2, 4'b0001};  // Output True
myROM[11] = {jmp,  RXX, RXX, 4'b0000};  // True Halt
myROM[12] = {jmp,  RXX, RXX, 4'b0000};
myROM[13] = {jmp,  RXX, RXX, 4'b0000};
myROM[14] = {jmp,  RXX, RXX, 4'b0000};
myROM[15] = {jmp,  RXX, RXX, 4'b0000};
