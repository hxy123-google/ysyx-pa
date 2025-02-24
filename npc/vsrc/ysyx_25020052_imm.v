module ysyx_25020052_imm(
    input [31:0] inst,
    output [31:0] imm_data
);
assign imm_data={{20{inst[31]}},inst[31:20]};
endmodule