module ysyx_25020052_imm(
    input [31:0] inst,
    output [31:0] imm_data
);
wire [6:0] opcode;
assign opcode=inst[6:0];
wire [31:0] imm_I,imm_U,imm_J;
assign imm_U = {inst[31:12], 12'b0 };
assign imm_I={{20{inst[31]}},inst[31:20]};
assign imm_J = {{11{inst[31]}}, inst[31],inst[19:12],inst[20],inst[30:21], 1'b0};
ysyx_25020052_MuxKey #(4, 7, 32) ib(imm_data, opcode, {
    7'b0010011, imm_I,
    7'b0010111, imm_U,
    7'b1101111, imm_J,
    7'b1100111, imm_I
  });
endmodule