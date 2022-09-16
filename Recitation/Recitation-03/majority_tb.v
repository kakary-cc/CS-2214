// Verilog test bench for E15 homework 5 problem 1.

`timescale 1ns/100ps
`include "majority.v"

module majority_tb;

   wire [2:0] A;
   wire       Y;
   integer k=0;

   assign A = k;
   majority the_circuit(A, Y);

   initial begin

      $dumpfile("majority.vcd");
      $dumpvars(0, majority_tb);

      for (k=0; k<8; k=k+1)
        #10 $display("done testing case %d", k);

      $finish;

   end 

endmodule
