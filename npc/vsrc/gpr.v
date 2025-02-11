module gpr(
    input [4:0] waddr,
    input [31:0] wdata,
    output [31:0] rdata,
    input [4:0] raddr,
    input  clk
);
RegisterFile #(5,32) i0(clk,wdata,waddr,rdata,raddr,1'b1);
endmodule
 