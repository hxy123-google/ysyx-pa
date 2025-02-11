module top(
    input clk,
    input rst,
    output [31:0] pc,
    input [31:0] inst
);
wire [2:0] AluControl;
wire [31:0] b_in;
wire [31:0] a_in;
wire [31:0] result;
my_pc u_my_pc(
    .clk    	(clk     ),
    .pc     	(pc      ),
    .rst        (rst     )
);

idu u_idu(
    .opcode(inst[6:0]),
    .f7(inst[31:25]),
    .f3(inst[14:12]),
    .AluControl(AluControl)
);
imm u_imm(
    .inst(inst),
    .imm_data(a_in)
);
gpr u_gpr(
    .waddr(inst[11:7]),
    .wdata(result),
    .rdata(b_in),
    .raddr(inst[19:15]),
    .clk(clk)
);
alu u_alu(
    .a(a_in),
    .b(b_in),
    .result(result),
    .AluControl(AluControl)
);
endmodule