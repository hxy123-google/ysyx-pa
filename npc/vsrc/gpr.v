module gpr(
    input [4:0] waddr,
    input [31:0] wdata,
    output [31:0] rdata1,
    input [4:0] raddr1,
    output [31:0] rdata2,
    input [4:0] raddr2,
    input  clk
);
RegisterFile #(5,32) i0(clk,wdata,waddr,rdata1,
raddr1,rdata2,raddr2,1'b1);
endmodule
 