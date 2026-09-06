`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/19 10:35:11
// Design Name: 
// Module Name: FOURAOI
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


module FOURAOI(
    input a,b,c,d,
    output e,f,g
);

assign e = a&b;
assign f = c&d;
assign g = ~(e|f);


endmodule

