module top(
    input clk,
    input rst,
    output [31:0] pc,
    input [31:0] inst
);
wire [2:0] AluControl;
wire [31:0] result;
wire [31:0] src1;
wire [31:0] src2,imm_data;
wire [4:0] rd,rs1,rs2;
wire [31:0] a_in;
wire [31:0] b_in;
wire [2:0] inst_type;
my_pc u_my_pc(
    .clk    	(clk     ),
    .pc     	(pc      ),
    .rst        (rst     )
);
// output declaration of module idu
idu u_idu(
    .inst       	(inst        ),
    .pc         	(pc          ),
    .rs1        	(rs1         ),
    .rs2        	(rs2         ),
    .rd         	(rd          ),
    .AluControl 	(AluControl  ),
    .inst_type      (inst_type)
);

imm u_imm(
    .inst(inst),
    .imm_data(imm_data)
);
gpr u_gpr(
    .waddr(rd),
    .wdata(result),
    .raddr1(rs1),
    .rdata1(src1),
    .raddr2(rs2),
    .rdata2(src2),
    .clk(clk)
);
assign a_in=src1;
alu u_alu(
    .a(a_in),
    .b(b_in),
    .result(result),
    .AluControl(AluControl)
);
MuxKey #(1, 3, 32) i0(b_in, inst_type, {
    3'b001, imm_data
  });
endmodule