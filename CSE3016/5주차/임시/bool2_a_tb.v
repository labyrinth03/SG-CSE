`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/26 09:27:53
// Design Name: 
// Module Name: bool2_a_tb
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


module bool2_a_tb;

reg aa, bb, cc;

wire dd;

bool2_a u_test(

    .a(aa),
    .b(bb),
    .c(cc),
    
    .d(dd)
    
);

initial begin
aa = 1'b0;
bb = 1'b0;
cc = 1'b0;
end

always@(aa or bb or cc) begin
aa <= #50 ~aa;
bb <= #100 ~bb;
cc <= #150 ~cc;
end

initial begin
#1000
$finish;
end

endmodule