module ysyx_25020052_idu(
    input [31:0] inst,
    input [31:0] pc,
    output [4:0] rs1,
    output [4:0] rs2,
    output [4:0] rd,
    output reg [2:0] inst_type,
    output reg [2:0] AluControl
);
import "DPI-C" function void npc_trap();
import "DPI-C" function void set_npc_state(int state,input int pc,int halt_ret); 
wire [6:0] opcode;
wire [2:0] f3;
wire [6:0] f7;
assign opcode=inst[6:0];
assign f3=inst[14:12];
assign rs1=inst[19:15];
assign rs2=inst[24:20];
assign rd=inst[11:7];
always@(*) begin
    case (opcode)
        7'b1110011: begin npc_trap();inst_type=3'b111; end
        7'b0010011: begin AluControl=3'b000; inst_type=3'b001; end
        default: set_npc_state(32'd3,pc,32'b1);
    endcase    
end

endmodule