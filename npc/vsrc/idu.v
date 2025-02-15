module idu(
    input [31:0] inst,
    input [31:0] pc,
    output reg [2:0] AluControl
);
import "DPI-C" function void npc_trap();
import "DPI-C" function void set_npc_state(int state,input int pc,int halt_ret); 
wire[6:0] opcode;
wire [2:0] f3;
wire [6:0] f7;
assign opcode=inst[6:0];
always@(*) begin
    case (opcode)
        7'b1110011: npc_trap();
        7'b0010011: AluControl=3'b000;
        default: set_npc_state(32'd3,pc,32'b1);
    endcase    
end
endmodule