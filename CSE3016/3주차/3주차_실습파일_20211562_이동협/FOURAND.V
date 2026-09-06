`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/16 22:31:40
// Design Name: 
// Module Name: FOURAND
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


module FOURAND(
    input a,b,c,d,
    output e,f,g
    );
    
assign e = a&b;
assign f = e&c;
assign g = f&d;

endmodule
