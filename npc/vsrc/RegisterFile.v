module RegisterFile #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
  input clk,
  input [DATA_WIDTH-1:0] wdata,
  input [ADDR_WIDTH-1:0] waddr,
  output [DATA_WIDTH-1:0] rdata,
  input [ADDR_WIDTH-1:0]  raddr,
  input wen
);
 always @(*) begin
    rf[0] = 32'b0;
  end
assign rdata=rf[raddr];
reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];
  always @(posedge clk) begin
    if (wen) rf[waddr] <= wdata;
  end
endmodule