module ysyx_25020052_my_pc(
    input clk,
    input rst,
    output [31:0] pc
);
wire [31:0] nextpc;
assign nextpc=pc+32'd4;
ysyx_25020052_Reg #(32, 32'h8000_0000) i1 (clk, rst, nextpc, pc,1'b1);
endmodule