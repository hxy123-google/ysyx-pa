module ysyx_25020052_my_pc(
    input clk,
    input rst,
    output [31:0] pc,
    input [31:0] inst,
    input [31:0] dynextpc
);
wire [31:0] nextpc;
wire [6:0] opcode;
assign opcode=inst[6:0];
//assign nextpc=pc+32'd4;
ysyx_25020052_MuxKeyWithDefault #(2, 7, 32) wwdata(nextpc, inst[6:0],pc+32'd4, {
    7'b1101111, dynextpc,
    7'b1100111, {dynextpc[31:1],1'b0}
  });
ysyx_25020052_Reg #(32, 32'h8000_0000) i1 (clk, rst, nextpc, pc,1'b1);
endmodule