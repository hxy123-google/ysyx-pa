module ysyx_25020052_alu(
    input [31:0] a,
    input [31:0] b,
    output [31:0] result,
    input [2:0] AluControl
);
wire of,zf,cf,nf;
wire [31:0] bb;
wire [31:0] r_add;
assign bb=(b)^{{32{AluControl[0]}}};
assign {cf,r_add}={1'b0,bb}+{1'b0,a}+{31'b0,AluControl[0]};
assign of= (bb[31]==a[31])&&(a[31]!=r_add[31]);
ysyx_25020052_MuxKey #(1, 3, 32) i0(result, AluControl, {
    3'b000, r_add
  });
endmodule