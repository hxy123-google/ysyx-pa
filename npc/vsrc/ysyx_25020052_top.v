module ysyx_25020052_top(
    input clk,
    input rst,
    output [31:0] pc,
    output reg [31:0] inst
);
import "DPI-C" function int unsigned paddr_read(int unsigned addr, int len);
// import "DPI-C" function void get_inst(input logic [31:0] inst);
wire [2:0] AluControl;
wire [31:0] result;
wire [31:0] src1;
wire [31:0] src2,imm_data;
wire [4:0] rd,rs1,rs2;
wire [31:0] a_in;
wire [31:0] b_in;
wire [2:0] inst_type;
wire [31:0] d_nextpc;
wire [31:0] wdata;
wire [1:0] is_pc;//判断操作数1是否为pc的值
wire is_imm;//判断操作数2是否为imm的值
ysyx_25020052_my_pc u_my_pc(
    .clk    	(clk     ),
    .pc     	(pc      ),
    .rst        (rst     ),
    .dynextpc   (d_nextpc),
    .inst       (inst)
);
// output declaration of module idu
ysyx_25020052_idu u_idu(
    .inst       	(inst        ),
    .pc         	(pc          ),
    .rs1        	(rs1         ),
    .rs2        	(rs2         ),
    .rd         	(rd          ),
    .AluControl 	(AluControl  ),
    .inst_type      (inst_type),
    .is_pc          (is_pc),
    .is_imm         (is_imm)
);
always@(pc) begin
    inst=paddr_read(pc,32'b0100);
    //$display("pc:%h,inst:%h",pc,inst);
end
ysyx_25020052_imm u_imm(
    .inst(inst),
    .imm_data(imm_data)
);
ysyx_25020052_gpr u_gpr(
    .waddr(rd),
    .wdata(wdata),
    .raddr1(rs1),
    .rdata1(src1),
    .raddr2(rs2),
    .rdata2(src2),
    .clk(clk)
);
ysyx_25020052_alu u_alu(
    .a(a_in),
    .b(b_in),
    .result(result),
    .AluControl(AluControl)
);
ysyx_25020052_MuxKeyWithDefault #(2, 2, 32) ia(a_in,is_pc, 32'b0,{
    2'b00, src1,
    2'b11, pc
  });
ysyx_25020052_MuxKey #(2, 1, 32) ib(b_in, is_imm, {
    1'b1, imm_data,
    1'b0, src2
  });
// ysyx_25020052_MuxKey #(2, 1, 32) ib(b_in, is_imm, {
//     1'b1, imm_data,
//     1'b0, src2
//   });
// ysyx_25020052_MuxKey #(2, 1, 32) ib(b_in, is_imm, {
//     1'b1, imm_data,
//     1'b0, src2
//   });
ysyx_25020052_MuxKeyWithDefault #(2, 7, 32) wwdata(wdata, inst[6:0],result, {
    7'b1101111, pc+32'd4,
    //7'b0100011, 1'b0
    7'b1100111, pc+32'd4
  });
assign d_nextpc=result;
endmodule