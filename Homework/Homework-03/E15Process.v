`include "4bit_adder.v"

module E15Process(input clk);

   // Register names
   parameter 
      Rg0 = 2'b00, Rg1 = 2'b01,
      Rg2 = 2'b10, Rg3 = 2'b11, 
      RXX = 2'b00;

   // Opcodes
   parameter
     jmp  = 4'b0000, jz   = 4'b0010,
     movi = 4'b1001, mov  = 4'b1000,
     addi = 4'b1011, add  = 4'b1010,
     subi = 4'b1101, sub  = 4'b1100,
     cmpi = 4'b1111, cmp  = 4'b1110,
     jnz  = 4'b0011;
   

   /*Processor state*/
   reg [3:0] pc;             // Program Counter
   reg       zFlag;          // Zero flag
   reg [3:0] r0, r1, r2, r3; // Registers


   /*Program storage*/
   reg [11:0] myROM [15:0]; // ROM (holds program)

   initial
     begin

        `include "multiplier.v"   // load the program
        
        pc = 4'b0000;           // initialize the program counter
        
     end


   /*Parts of the instruction*/
   wire [3:0] opCode;         // op code
   wire [1:0] src, dst;       // src and dst register
   wire [3:0] immData;        // "Immediate" data


   wire [3:0] pcIncr; // Program Counter Increment
   wire [3:0] pcRes;  // Output of pc ALU
   wire       pcz;    // Unused - zero flag for pc ALU
   
   wire [3:0] storeVal, operand1, operand2;

   /*ALU wires*/
   wire       addNotSub; // Determines if ALU adds or subtracts
   wire [3:0] aluOutput;    // Output (combinational) of ALU
   wire       aluOutputZero;      // Output (combinational) of ALU zero flag


   
   assign {opCode, src, dst, immData} = myROM[pc];
   assign addNotSub = ((opCode == add) | (opCode == addi));

   // pcIncr is a 4-bit value representing the value
   // that is to be added to the program counter
   assign pcIncr = (opCode == jmp) ? immData : (opCode == jz) ? (zFlag ? immData : 4'b0001) : (opCode == jnz) ? (zFlag ? 4'b0001 : immData) : 4'b0001; // Modified

   // storeVal is a 4-bit value representing the value that
   // is to be stored in the destination register, for
   // instructions that write to it
   assign storeVal = (opCode == movi) ? immData : (opCode != mov) ? aluOutput : (src == Rg0) ? r0 : (src == Rg1) ? r1 : (src == Rg2) ? r2 : r3; // Modified

   // operand1 is a 4-bit value represnting the first operand
   // to be passed into the ALU
   assign operand1 = ((opCode & 4'b0001) == 4'b0001) ? immData : (src == Rg0) ? r0 : (src == Rg1) ? r1 : (src == Rg2) ? r2 : r3; // Modified

   // operand1 is a 4-bit value represnting the second operand
   // to be passed into the ALU
   assign operand2 = (dst == Rg0) ? r0 : (dst == Rg1) ? r1 : (dst == Rg2) ? r2 : r3; // Modified

   always @(posedge clk)
      begin 

        // Update zero flag
        case(opCode)
          addi, add, subi, sub, cmpi, cmp:
            begin
               zFlag <= aluOutputZero;
            end
        endcase

        // update destination register
        case(opCode)
          movi, mov, add, addi, sub, subi:
            case(dst)
              Rg0: r0 <= storeVal;
              Rg1: r1 <= storeVal;
              Rg2: r2 <= storeVal;
              Rg3: r3 <= storeVal;
            endcase
        endcase

        // Update program counter
        pc <= pcRes;
 
     end
   
   simpleALU dataALU(addNotSub, operand1, operand2, aluOutputZero, aluOutput);
   simpleALU pcALU(1'b1, pc, pcIncr, pcz, pcRes);

endmodule


module simpleALU(
    input addNotSub,
    input [3:0] src, dst,
    output zFlag,
    output [3:0] res);
    wire Cout;

    fourbit_adder the_adder(dst, addNotSub ? src : ~src, ~addNotSub, res, Cout);

    assign zFlag = !(res);

endmodule
