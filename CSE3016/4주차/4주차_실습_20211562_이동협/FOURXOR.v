`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/19 10:24:48
// Design Name: 
// Module Name: FOURXOR
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


module FOURXOR(
    input a,b,c,d,
    output e,f,g
);

assign e = a^b;
assign f = e^c;
assign g = f^d;


endmodule

