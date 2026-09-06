`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/26 09:10:22
// Design Name: 
// Module Name: bitcom
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


module bitcom(
input a,b,
output c,d,e,f
);

assign c = ~(a^b);
assign d = (a^b);
assign e = a&(~b);
assign f = (~a)&b;

endmodule
