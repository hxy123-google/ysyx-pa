module ysyx_25020052_RegisterFile #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
  input clk,
  input [DATA_WIDTH-1:0] wdata,
  input [ADDR_WIDTH-1:0] waddr,
  output [DATA_WIDTH-1:0] rdata1,
  input [ADDR_WIDTH-1:0]  raddr1,
  output [DATA_WIDTH-1:0] rdata2,
  input [ADDR_WIDTH-1:0]  raddr2,
  input wen
);
//  always @(*) begin
//     rf[0] = 32'b0;
//   end
import "DPI-C" function void get_reg(input logic [DATA_WIDTH-1:0] a[] ); 
assign rdata1 = raddr1 == {ADDR_WIDTH{1'b0}} ? 0 : rf[raddr1];
assign rdata2 = raddr2 == {ADDR_WIDTH{1'b0}} ? 0 : rf[raddr2];
reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];
  always @(posedge clk) begin
    if (wen) rf[waddr] <= wdata;
  end
   initial begin 
        get_reg(rf);
    end

    
  
endmodule