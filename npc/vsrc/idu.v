module idu(
    input [6:0] opcode,
    input [2:0] f3,
    input [6:0] f7,
    output [2:0] AluControl
);
import "DPI-C" function void npc_trap();
assign AluControl=3'b000;
always@(*) begin
    if(opcode==7'b1110011)begin
        npc_trap();
    end
end
endmodule