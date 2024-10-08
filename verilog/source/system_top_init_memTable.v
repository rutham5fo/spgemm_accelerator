// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Version: 2022.1
// Copyright (C) Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

module system_top_init_memTable (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
        ap_idle,
        ap_ready,
        handle,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7_ap_vld,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6_ap_vld,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5_ap_vld,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4_ap_vld,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3_ap_vld,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2_ap_vld,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1_ap_vld,
        run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const,
        run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_ap_vld
);

parameter    ap_ST_fsm_state1 = 1'd1;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output   ap_ready;
input  [1:0] handle;
output  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7;
output   run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7_ap_vld;
output  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6;
output   run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6_ap_vld;
output  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5;
output   run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5_ap_vld;
output  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4;
output   run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4_ap_vld;
output  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3;
output   run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3_ap_vld;
output  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2;
output   run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2_ap_vld;
output  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1;
output   run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1_ap_vld;
output  [7:0] run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const;
output   run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_ap_vld;

reg ap_idle;
reg run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7_ap_vld;
reg run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6_ap_vld;
reg run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5_ap_vld;
reg run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4_ap_vld;
reg run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3_ap_vld;
reg run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2_ap_vld;
reg run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1_ap_vld;
reg run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_ap_vld;

(* fsm_encoding = "none" *) reg   [0:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg    ap_block_state1_pp0_stage0_iter0;
wire   [0:0] icmp_ln37_fu_90_p2;
reg    ap_condition_exit_pp0_iter0_stage0;
wire    ap_loop_exit_ready;
reg    ap_ready_int;
wire   [2:0] add_ln38_fu_106_p2;
reg   [1:0] j_fu_64;
wire   [1:0] j_3_fu_96_p2;
wire    ap_loop_init;
reg   [1:0] ap_sig_allocacmp_j_2;
wire   [2:0] zext_ln38_fu_102_p1;
wire   [2:0] shl_ln_fu_74_p3;
reg    ap_done_reg;
wire    ap_continue_int;
reg    ap_done_int;
reg   [0:0] ap_NS_fsm;
reg    ap_ST_fsm_state1_blk;
wire    ap_start_int;
wire    ap_ce_reg;

// power-on initialization
initial begin
#0 ap_CS_fsm = 1'd1;
#0 ap_done_reg = 1'b0;
end

system_top_flow_control_loop_pipe_sequential_init flow_control_loop_pipe_sequential_init_U(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst),
    .ap_start(ap_start),
    .ap_ready(ap_ready),
    .ap_done(ap_done),
    .ap_start_int(ap_start_int),
    .ap_loop_init(ap_loop_init),
    .ap_ready_int(ap_ready_int),
    .ap_loop_exit_ready(ap_condition_exit_pp0_iter0_stage0),
    .ap_loop_exit_done(ap_done_int),
    .ap_continue_int(ap_continue_int),
    .ap_done_int(ap_done_int)
);

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_state1;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_done_reg <= 1'b0;
    end else begin
        if ((ap_continue_int == 1'b1)) begin
            ap_done_reg <= 1'b0;
        end else if (((ap_loop_exit_ready == 1'b1) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
            ap_done_reg <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        if ((icmp_ln37_fu_90_p2 == 1'd0)) begin
            j_fu_64 <= j_3_fu_96_p2;
        end else if ((ap_loop_init == 1'b1)) begin
            j_fu_64 <= 2'd0;
        end
    end
end

always @ * begin
    if ((ap_start_int == 1'b0)) begin
        ap_ST_fsm_state1_blk = 1'b1;
    end else begin
        ap_ST_fsm_state1_blk = 1'b0;
    end
end

always @ * begin
    if (((icmp_ln37_fu_90_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        ap_condition_exit_pp0_iter0_stage0 = 1'b1;
    end else begin
        ap_condition_exit_pp0_iter0_stage0 = 1'b0;
    end
end

always @ * begin
    if (((ap_loop_exit_ready == 1'b1) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        ap_done_int = 1'b1;
    end else begin
        ap_done_int = ap_done_reg;
    end
end

always @ * begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b0))) begin
        ap_idle = 1'b1;
    end else begin
        ap_idle = 1'b0;
    end
end

always @ * begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        ap_ready_int = 1'b1;
    end else begin
        ap_ready_int = 1'b0;
    end
end

always @ * begin
    if (((ap_loop_init == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        ap_sig_allocacmp_j_2 = 2'd0;
    end else begin
        ap_sig_allocacmp_j_2 = j_fu_64;
    end
end

always @ * begin
    if (((3'd6 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1_ap_vld = 1'b1;
    end else begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1_ap_vld = 1'b0;
    end
end

always @ * begin
    if (((3'd5 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2_ap_vld = 1'b1;
    end else begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2_ap_vld = 1'b0;
    end
end

always @ * begin
    if (((3'd4 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3_ap_vld = 1'b1;
    end else begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3_ap_vld = 1'b0;
    end
end

always @ * begin
    if (((3'd3 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4_ap_vld = 1'b1;
    end else begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4_ap_vld = 1'b0;
    end
end

always @ * begin
    if (((3'd2 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5_ap_vld = 1'b1;
    end else begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5_ap_vld = 1'b0;
    end
end

always @ * begin
    if (((3'd1 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6_ap_vld = 1'b1;
    end else begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6_ap_vld = 1'b0;
    end
end

always @ * begin
    if (((3'd0 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7_ap_vld = 1'b1;
    end else begin
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7_ap_vld = 1'b0;
    end
end

always @ * begin
    if (((3'd7 == add_ln38_fu_106_p2) & (icmp_ln37_fu_90_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_ap_vld = 1'b1;
    end else begin
        run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_ap_vld = 1'b0;
    end
end

always @ * begin
    case (ap_CS_fsm)
        ap_ST_fsm_state1 : begin
            ap_NS_fsm = ap_ST_fsm_state1;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign add_ln38_fu_106_p2 = (zext_ln38_fu_102_p1 + shl_ln_fu_74_p3);

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

always @ * begin
    ap_block_state1_pp0_stage0_iter0 = (ap_start_int == 1'b0);
end

assign ap_loop_exit_ready = ap_condition_exit_pp0_iter0_stage0;

assign icmp_ln37_fu_90_p2 = ((ap_sig_allocacmp_j_2 == 2'd2) ? 1'b1 : 1'b0);

assign j_3_fu_96_p2 = (ap_sig_allocacmp_j_2 + 2'd1);

assign run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_1 = 8'd0;

assign run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_2 = 8'd0;

assign run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_3 = 8'd0;

assign run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_4 = 8'd0;

assign run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_5 = 8'd0;

assign run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_6 = 8'd0;

assign run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_7 = 8'd0;

assign run_mulmulmulmulmulbool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const = 8'd0;

assign shl_ln_fu_74_p3 = {{handle}, {1'd0}};

assign zext_ln38_fu_102_p1 = ap_sig_allocacmp_j_2;

endmodule //system_top_init_memTable
