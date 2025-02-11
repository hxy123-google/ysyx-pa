module idu(
    input [6:0] opcode,
    input [2:0] f3,
    input [6:0] f7,
    output [2:0] AluControl
);
assign AluControl=3'b000;
endmodule