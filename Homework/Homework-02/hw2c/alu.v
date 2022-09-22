`include "four_bit_adder.v"

module alu(A, B, Op, S);

   input [3:0]  A;
   input [3:0]  B;
   output [3:0] S;
   input [2:0]  Op;
   
   wire [3:0] selected_B;
   assign selected_B = Op[2] ? ~B : B;
   
   wire [3:0] AND;
   assign AND = A & selected_B;
   
   wire [3:0] OR;
   assign OR = A | selected_B;
   
   wire [3:0] SUM;
   wire dispose;
   four_bit_adder FA1(A, selected_B, Op[2], SUM, dispose);
   
   wire [3:0] SLT;
   assign SLT = SUM[3];
   
   assign S = Op[1] ? (Op[0] ? SLT : SUM) : (Op[0] ? OR : AND);

endmodule
