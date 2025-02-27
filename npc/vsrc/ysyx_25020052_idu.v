module ysyx_25020052_idu(
    input [31:0] inst,
    input [31:0] pc,
    output  [4:0] rs1,
    output  [4:0] rs2,
    output  [4:0] rd,
    output reg [2:0] inst_type,
    output reg [2:0] AluControl,
    output reg [1:0] is_pc,
    output reg is_imm
);
import "DPI-C" function void npc_trap();
import "DPI-C" function void set_npc_state(int state,input int pc,int halt_ret); 
wire [6:0] opcode;
wire [2:0] f3;
wire [6:0] f7;
reg [4:0] drs1,drs2,drrd;
reg isdefault;
assign opcode=inst[6:0];
assign f3=inst[14:12];
assign rs1=(isdefault==0)?inst[19:15]:drs1;
assign rs2=(isdefault==0)?inst[24:20]:drs2;
assign rd=(isdefault==0)?inst[11:7]:drrd;
// output declaration of module ysyx_25020052_MuxKey
always@(*) begin
    case (opcode)
        7'b1110011: begin npc_trap();inst_type=3'b111;isdefault=0; end//N
        7'b0010011: begin //I
            inst_type=3'b001;
            is_pc=2'b0;
            is_imm=1;
            isdefault=0;
            case(f3) 
                3'b000: AluControl=f3;
                default:;
            endcase 
            end
        7'b0010111: begin//auipc
            is_pc=2'b11;is_imm=1;AluControl=3'b000;isdefault=0;
        end
        7'b1101111:begin//J
            is_pc=2'b11;is_imm=1;AluControl=3'b000;isdefault=0;
        end
        7'b0110111:begin
            AluControl=3'b000;is_imm=1;is_pc=2'b01;isdefault=0;
        end
        7'b1100111:begin//jalr(I)
            AluControl=3'b000;is_imm=1;is_pc=2'b0;isdefault=0;
        end
        default: begin isdefault=1;AluControl=3'b000;is_imm=0;drs1=5'b0;drs2=5'b0;drrd=5'b0;is_pc=2'b0;end//set_npc_state(32'd3,pc,32'b1);
    endcase    
end

endmodule