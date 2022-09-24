module majority(A, Y);

  input [2:0]A;
  output Y;

  assign Y = (A[0] & A[1]) | (A[1] & A[2]) | (A[0] & A[2]);

endmodule
