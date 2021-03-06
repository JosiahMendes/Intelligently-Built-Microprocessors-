module ALUDecoder3(
input [15:0] INSTR, input CARRY, input [15:0] Rn, input [15:0] Rm, input [15:0] Rx, 
output Shift_in, output Shift_Right, output [3:0] SN, 
output [2:0] RnSelect, output [2:0] RmSelect, output [1:0] RxSelect, 
output CINadd_sub, output add_sub, output [1:0] ASSelect, output [1:0] OPSel, output [2:0] COUTSel
);

wire A, B,C, D, E, F, G, H, I, J, K, L, M, N, O, P;

assign A = INSTR[15];
assign B = INSTR[14];
assign C = INSTR[13];
assign D = INSTR[12];
assign E = INSTR[11];
assign F = INSTR[10];
assign G = INSTR[9];
assign H = INSTR[8];
assign I = INSTR[7];
assign J = INSTR[6];
assign K = INSTR[5];
assign L = INSTR[4];
assign M = INSTR[3];
assign N = INSTR[2];
assign O = INSTR[1];
assign P = INSTR[0];


wire  adr, adm, adi, sbr, sbm, sbi, mlr, xsl, xsr, bbo, stk, ldr, sti, jmr;


assign adr = ~A & ~B & ~C & ~D &  E; 
assign adm = ~A & ~B & ~C &  D;
assign adi = ~A & ~B &  C & ~D & ~E;
assign sbr = ~A & ~B &  C & ~D &  E;
assign sbm = ~A & ~B &  C &  D;
assign sbi = ~A &  B & ~C & ~D & ~E;    
assign mlr = ~A &  B & ~C & ~D &  E;
assign xsl = ~A &  B & ~C &  D & ~E; 
assign xsr = ~A &  B & ~C &  D &  E;
assign bbo = ~A &  B &  C & ~D & ~E;

assign stk = ~A &  B &  C & ~D &  E;
assign ldr = ~A &  B &  C &  D & ~E;
assign sti = ~A &  B &  C &  D &  E;

assign jmr =  A &  B &  C & ~D & ~E;


assign RnSelect[2] = stk&G;
assign RnSelect[1] = ((adr|sbr|mlr|bbo|jmr)&M)|((adi|sbi)&F)|((ldr|sti)&I)|stk&H;
assign RnSelect[0] = ((adr|sbr|mlr|bbo|jmr)&N)|((adi|sbi)&G)|((ldr|sti)&J)|((adm|sbm)&E)|stk&I;

assign RmSelect[2] = (adm|sbm|adi|sbi)|((ldr|sti)&~H)|stk;
assign RmSelect[1] = ((adr|sbr|mlr|bbo|xsl|xsr)&O)|((ldr|sti)&K)|((ldr|sti)&~H)|stk;
assign RmSelect[0] = ((adr|sbr|mlr|bbo|xsl|xsr)&P)|((ldr|sti)&L)|(adi|sbi);

assign RxSelect[1] = ((adr|sbr|mlr|jmr)&K);
assign RxSelect[0] = ((adr|sbr|mlr|jmr)&L);



assign Shift_in = (xsl|xsr)&((~G&H)|(G&~H&CARRY)|(G&H&Rm[15]));

assign Shift_Right = (xsr)|((adr|sbr|mlr) & I & J);

assign SN[3] = ((xsl|xsr)&I)|( (adr|sbr|mlr) & I & Rx[3])|( (ldr|sti)&H&M);
assign SN[2] = ((xsl|xsr)&J)|( (adr|sbr|mlr) & I & Rx[2])|( (ldr|sti)&H&N);
assign SN[1] = ((xsl|xsr)&K)|( (adr|sbr|mlr) & I & Rx[1])|( (ldr|sti)&H&O);
assign SN[0] = ((xsl|xsr)&L)|( (adr|sbr|mlr) & I & Rx[0])|( (ldr|sti)&H&P);


assign CINadd_sub = ((adr|mlr)&((~G&H)|(G&~H&CARRY)|(G&H&Rm[15]))) | (sbr&((~G&~H)|(G&~H&~CARRY)|(G&H&~Rm[15]))) | (sbm|sbi|(stk&J));

assign add_sub = !(sbr|sbm|sbi|(stk&J));

assign ASSelect[1] = bbo;
assign ASSelect[0] = mlr;

assign OPSel[1] = xsl|xsr;
assign OPSel[0] = ((adr|sbr|mlr)&~I&J);

assign COUTSel[2] = (mlr&~I&J)|(sbi|sbm|sbr);
assign COUTSel[1] = xsl|xsr|(mlr&~(~I&J))|(sbr&~I&J);
assign COUTSel[0] = (adr&~I&J)|((mlr|sbr)&~(~I&J))|(sbm|sbi);


endmodule