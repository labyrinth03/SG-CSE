`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/23 15:51:00
// Design Name: 
// Module Name: dmg1_a_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module dmg1_a_tb;

reg aa, bb;

wire cc;

dmg1_a u_test(

    .a(aa),
    .b(bb),
    
    .c(cc)
    
);

initial begin
aa = 1'b0;
bb = 1'b0;
end

always@(aa or bb) begin
aa <= #50 ~aa;
bb <= #100 ~bb;
end

initial begin
#1000
$finish;
end

endmodule

