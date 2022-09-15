`include "four_bit_adder.v"

module alu(A, B, Op, S);

   input [3:0]  A;
   input [3:0]  B;
   output [3:0] S;
   input [2:0]  Op;

   assign S = 0;

endmodule
