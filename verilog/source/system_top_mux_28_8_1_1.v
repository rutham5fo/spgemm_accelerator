// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Tool Version Limit: 2022.04
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================

`timescale 1ns/1ps

module system_top_mux_28_8_1_1 #(
parameter
    ID                = 0,
    NUM_STAGE         = 1,
    din0_WIDTH       = 32,
    din1_WIDTH       = 32,
    din2_WIDTH         = 32,
    dout_WIDTH            = 32
)(
    input  [7 : 0]     din0,
    input  [7 : 0]     din1,
    input  [7 : 0]    din2,
    output [7 : 0]   dout);

// puts internal signals
wire [7 : 0]     sel;
// level 1 signals
wire [7 : 0]         mux_1_0;
// level 2 signals
wire [7 : 0]         mux_2_0;
// level 3 signals
wire [7 : 0]         mux_3_0;
// level 4 signals
wire [7 : 0]         mux_4_0;
// level 5 signals
wire [7 : 0]         mux_5_0;
// level 6 signals
wire [7 : 0]         mux_6_0;
// level 7 signals
wire [7 : 0]         mux_7_0;
// level 8 signals
wire [7 : 0]         mux_8_0;

assign sel = din2;

// Generate level 1 logic
assign mux_1_0 = (sel[0] == 0)? din0 : din1;

// Generate level 2 logic
assign mux_2_0 = mux_1_0;

// Generate level 3 logic
assign mux_3_0 = mux_2_0;

// Generate level 4 logic
assign mux_4_0 = mux_3_0;

// Generate level 5 logic
assign mux_5_0 = mux_4_0;

// Generate level 6 logic
assign mux_6_0 = mux_5_0;

// Generate level 7 logic
assign mux_7_0 = mux_6_0;

// Generate level 8 logic
assign mux_8_0 = mux_7_0;

// output logic
assign dout = mux_8_0;

endmodule
