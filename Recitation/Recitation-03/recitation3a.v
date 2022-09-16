module recitation3a(A, B, X);

  input A, B;
  output X;

  assign X = ~(~((A | ~B) | (A & B)) & (A | B));

endmodule
