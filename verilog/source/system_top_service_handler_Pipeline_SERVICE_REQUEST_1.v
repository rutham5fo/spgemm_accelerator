// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Version: 2022.1
// Copyright (C) Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

module system_top_service_handler_Pipeline_SERVICE_REQUEST_1 (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
        ap_idle,
        ap_ready,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_32,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_33,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_34,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_35,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_36,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_37,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_38,
        run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_39,
        tmp_req_row,
        dest_pu_out,
        dest_pu_out_ap_vld
);

parameter    ap_ST_fsm_state1 = 1'd1;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output   ap_ready;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_32;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_33;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_34;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_35;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_36;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_37;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_38;
input  [7:0] run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_39;
input  [7:0] tmp_req_row;
output  [7:0] dest_pu_out;
output   dest_pu_out_ap_vld;

reg ap_idle;
reg dest_pu_out_ap_vld;

(* fsm_encoding = "none" *) reg   [0:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg    ap_block_state1_pp0_stage0_iter0;
wire   [0:0] icmp_ln121_fu_138_p2;
reg    ap_condition_exit_pp0_iter0_stage0;
wire    ap_loop_exit_ready;
reg    ap_ready_int;
reg   [7:0] dest_pu_fu_56;
wire   [7:0] dest_pu_2_fu_195_p3;
wire    ap_loop_init;
reg   [7:0] ap_sig_allocacmp_dest_pu_load_1;
reg   [3:0] i_fu_60;
wire   [3:0] add_ln121_fu_144_p2;
reg   [3:0] ap_sig_allocacmp_i_6;
wire   [7:0] tmp_fu_153_p10;
wire   [1:0] dest_pu_1_fu_181_p4;
wire   [0:0] icmp_ln123_fu_175_p2;
wire   [7:0] zext_ln124_fu_191_p1;
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

system_top_mux_84_8_1_1 #(
    .ID( 1 ),
    .NUM_STAGE( 1 ),
    .din0_WIDTH( 8 ),
    .din1_WIDTH( 8 ),
    .din2_WIDTH( 8 ),
    .din3_WIDTH( 8 ),
    .din4_WIDTH( 8 ),
    .din5_WIDTH( 8 ),
    .din6_WIDTH( 8 ),
    .din7_WIDTH( 8 ),
    .din8_WIDTH( 4 ),
    .dout_WIDTH( 8 ))
mux_84_8_1_1_U4298(
    .din0(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_32),
    .din1(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_33),
    .din2(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_34),
    .din3(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_35),
    .din4(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_36),
    .din5(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_37),
    .din6(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_38),
    .din7(run_bool_bool_const_bool_procUnit_unsigned_char_const_procTvec_const_39),
    .din8(ap_sig_allocacmp_i_6),
    .dout(tmp_fu_153_p10)
);

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
        if ((icmp_ln121_fu_138_p2 == 1'd0)) begin
            dest_pu_fu_56 <= dest_pu_2_fu_195_p3;
        end else if ((ap_loop_init == 1'b1)) begin
            dest_pu_fu_56 <= 8'd5;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        if ((icmp_ln121_fu_138_p2 == 1'd0)) begin
            i_fu_60 <= add_ln121_fu_144_p2;
        end else if ((ap_loop_init == 1'b1)) begin
            i_fu_60 <= 4'd0;
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
    if (((icmp_ln121_fu_138_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
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
        ap_sig_allocacmp_dest_pu_load_1 = 8'd5;
    end else begin
        ap_sig_allocacmp_dest_pu_load_1 = dest_pu_fu_56;
    end
end

always @ * begin
    if (((ap_loop_init == 1'b1) & (1'b1 == ap_CS_fsm_state1))) begin
        ap_sig_allocacmp_i_6 = 4'd0;
    end else begin
        ap_sig_allocacmp_i_6 = i_fu_60;
    end
end

always @ * begin
    if (((icmp_ln121_fu_138_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state1) & (ap_start_int == 1'b1))) begin
        dest_pu_out_ap_vld = 1'b1;
    end else begin
        dest_pu_out_ap_vld = 1'b0;
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

assign add_ln121_fu_144_p2 = (ap_sig_allocacmp_i_6 + 4'd1);

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

always @ * begin
    ap_block_state1_pp0_stage0_iter0 = (ap_start_int == 1'b0);
end

assign ap_loop_exit_ready = ap_condition_exit_pp0_iter0_stage0;

assign dest_pu_1_fu_181_p4 = {{ap_sig_allocacmp_i_6[2:1]}};

assign dest_pu_2_fu_195_p3 = ((icmp_ln123_fu_175_p2[0:0] == 1'b1) ? zext_ln124_fu_191_p1 : ap_sig_allocacmp_dest_pu_load_1);

assign dest_pu_out = dest_pu_fu_56;

assign icmp_ln121_fu_138_p2 = ((ap_sig_allocacmp_i_6 == 4'd8) ? 1'b1 : 1'b0);

assign icmp_ln123_fu_175_p2 = ((tmp_req_row == tmp_fu_153_p10) ? 1'b1 : 1'b0);

assign zext_ln124_fu_191_p1 = dest_pu_1_fu_181_p4;

endmodule //system_top_service_handler_Pipeline_SERVICE_REQUEST_1
