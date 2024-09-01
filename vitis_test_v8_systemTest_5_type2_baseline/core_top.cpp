#include "core_top.h"

#ifndef __SYNTHESIS__
void print_row_top(procVecType row, vx_uint8 num){
    PRINT("\n TOP | row[%i] = ", num);
    for (vx_uint8 i = 0; i < row.len; i++) {
        PRINT(" (%i, %i), ", INSERT_VECdata(row, i), INSERT_VECindex(row, i));
    }
}
#endif

void run(
    const bool init,
    const bool pu_active[NUM_PE],
    bool result_ready[NUM_PE],
    procUnit* pu,
    const vx_uint8 memIN_read_ptr[NUM_PE],
    const procVec streamA[NUM_PE],
    proc_vt comp_out[NUM_PE],
    rep_t repMemB[NUM_PE][MEM_FIFO_SIZE],
    networkUnit* net,
    const vx_uint8 initCountTable[ROW_LEN],
    vx_uint8& memOUT_write_ptr,
    req_t reqMemB[MEM_FIFO_SIZE]
)
{
    static memUnit_regs memRegs[NUM_PE];
    static countUnit_regs countRegs[NUM_PE];
    static compUnit_regs compRegs[NUM_PE];
    static servExtUnit_regs extRegs[NUM_PE];
    static procUnit_regs procRegs[NUM_PE];
    static bool comp_writeback_done[NUM_PE];
    //#pragma HLS aggregate variable=memRegs
    //#pragma HLS aggregate variable=countRegs
    //#pragma HLS aggregate variable=compRegs
    //#pragma HLS aggregate variable=extRegs
    //#pragma HLS aggregate variable=procRegs

    // Streams
    static rep_t ext_repIN[NUM_PE][EXT_FIFO_SIZE];
    static req_t ext_reqIN[NUM_PE][EXT_FIFO_SIZE];
    static rep_t ext_repOUT[NUM_PE][EXT_FIFO_SIZE];
    static req_t ext_reqOUT[NUM_PE][EXT_FIFO_SIZE];
    #pragma HLS aggregate variable=ext_repIN
    #pragma HLS aggregate variable=ext_reqIN
    #pragma HLS aggregate variable=ext_repOUT
    #pragma HLS aggregate variable=ext_reqOUT
    
    static vx_uint8 reqIN_write_ptr[NUM_PE];
    static vx_uint8 repIN_write_ptr[NUM_PE];
    static vx_uint8 reqOUT_write_ptr[NUM_PE];
    static vx_uint8 repOUT_write_ptr[NUM_PE];

    static rep_t net_fwd_rep;
    static req_t net_fwd_req;
    static vx_uint8 net_regs_prev_reqs[NUM_PE][PE_MEM_SIZE];
    static vx_uint8 net_regs_reqIN_ptr[NUM_PE];
    static vx_uint8 net_regs_repIN_ptr[NUM_PE];
    static vx_uint8 net_memTable[NUM_PE*PE_MEM_SIZE];
    static vx_uint8 net_batchCountTable[ROW_LEN];
    static vx_uint8 net_count_writeback[NUM_PE];
    static bool net_rep_valid;
    static bool net_req_valid;
    #pragma HLS array_partition variable=net_regs_prev_reqs type=complete dim=0
    #pragma HLS array_partition variable=net_memTable type=complete dim=0
    #pragma HLS array_partition variable=net_batchCountTable type=complete dim=0

    static vx_uint8 count_set;
    static vx_uint8 count_val;
    static vx_uint8 avail_req;
    static vx_uint8 ext_count;
    static vx_uint8 fetch_count;
    static vx_uint8 shft_indA_0;
    static vx_uint8 req_row;
    static vx_uint8 update_memTable[MEMTABLE_SIZE];
    static procVecType update_memB[MEMTABLE_SIZE];

    static bool init_fwd[NUM_PE][6];
    static bool data_valid[NUM_PE];
    static bool mult_out_valid[NUM_PE];
    static bool force_finish[NUM_PE];
    static proc_t data_out[NUM_PE];
    static proc_vt memB_channel[NUM_PE];
    static procVec mult_out[NUM_PE];
    #pragma HLS aggregate variable=data_out
    #pragma HLS aggregate variable=memB_channel
    #pragma HLS aggregate variable=mult_out
    #pragma HLS array_partition variable=init_fwd type=complete dim=1

    static bool comp_out_valid;
    static bool comp_out_empty;
    static proc_t comp_data_out[ROW_LEN];
    #pragma HLS aggregate variable=comp_data_out

    RUN_COMPUTE_1: for (vx_uint8 pu_num = 0; pu_num < NUM_PE; pu_num++) {
        #pragma HLS unroll
        pu->update(
            #ifndef __SYNTHESIS__
            pu_num,
            #endif
            init,
            init_fwd[pu_num][0],
            pu_active[pu_num],
            data_valid[pu_num],
            procRegs[pu_num].regs_last_elemA,
            procRegs[pu_num].regs_fetch_ptr,
            shft_indA_0,
            req_row,
            fetch_count,
            procRegs[pu_num].memTable,
            procRegs[pu_num].fetch_data,
            procRegs[pu_num].regs_prev_req_row,
            procRegs[pu_num].memB,
            data_out[pu_num],
            memB_channel[pu_num],
            streamA[pu_num],

            memIN_read_ptr[pu_num],
            repIN_write_ptr[pu_num],
            procRegs[pu_num].aux_indA_0,
            update_memTable,
            update_memB,
            repMemB[pu_num],
            ext_repIN[pu_num],
            count_set,
            count_val,
            memRegs[pu_num],

            reqIN_write_ptr[pu_num],
            repOUT_write_ptr[pu_num],
            procRegs[pu_num].aux_memTable_2,
            procRegs[pu_num].aux_memB_2,
            ext_reqIN[pu_num],
            ext_repOUT[pu_num],
            ext_count,
            extRegs[pu_num]
        );
        pu->reflect(
            #ifndef __SYNTHESIS__
            procRegs[pu_num].aux_memTable_1,
            #endif
            init_fwd[pu_num][0],
            init_fwd[pu_num][1],
            pu_num,
            req_row,
            procRegs[pu_num].countTable,
            ext_count,
            count_set,
            count_val,
            fetch_count,
            reqOUT_write_ptr[pu_num],
            countRegs[pu_num],
            ext_reqOUT[pu_num],

            update_memTable,
            update_memB,
            shft_indA_0,
            procRegs[pu_num].aux_indA_0,
            procRegs[pu_num].memTable,
            procRegs[pu_num].aux_memTable_1,
            procRegs[pu_num].aux_memTable_2,
            procRegs[pu_num].memB,
            procRegs[pu_num].aux_memB_1,
            procRegs[pu_num].aux_memB_2
        );
        net->service_handler(
            #ifndef __SYNTHESIS__
            pu_num,
            #endif
            init_fwd[pu_num][1],
            init_fwd[pu_num][5],
            net_batchCountTable,
            net_memTable,
            reqOUT_write_ptr[pu_num],
            repOUT_write_ptr[pu_num],
            net_regs_reqIN_ptr[pu_num],
            net_regs_repIN_ptr[pu_num],
            net_count_writeback[pu_num],
            net_fwd_rep,
            net_fwd_req,
            net_rep_valid,
            net_req_valid,
            ext_reqOUT[pu_num],
            ext_repOUT[pu_num]
        );
        net->crossbar(
            pu_num,
            init_fwd[pu_num][5],
            net_rep_valid,
            net_req_valid,
            net_count_writeback[pu_num],
            initCountTable,
            memOUT_write_ptr,
            reqIN_write_ptr,
            repIN_write_ptr,
            net_batchCountTable,
            net_memTable,
            net_regs_prev_reqs,
            net_fwd_rep,
            net_fwd_req,
            ext_reqIN,
            ext_repIN,
            reqMemB
        );
    }
    RUN_COMPUTE_2: for (vx_uint8 pu_num = 0; pu_num < NUM_PE; pu_num++) {
        #pragma HLS unroll
        pu->multiply(
            #ifndef __SYNTHESIS__
            pu_num,
            #endif
            init_fwd[pu_num][0],
            init_fwd[pu_num][2],
            mult_out_valid[pu_num],
            force_finish[pu_num],
            data_valid[pu_num],
            data_out[pu_num],
            memB_channel[pu_num],
            mult_out[pu_num]
        );
    }
    RUN_COMPUTE_3: for (vx_uint8 pu_num = 0; pu_num < NUM_PE; pu_num++) {
        #pragma HLS unroll
        pu->compare(
            #ifndef __SYNTHESIS__
            pu_num,
            #endif
            init_fwd[pu_num][2],
            force_finish[pu_num],
            init_fwd[pu_num][3],
            mult_out_valid[pu_num],
            comp_out_valid,
            comp_out_empty,
            mult_out[pu_num],
            comp_data_out,
            compRegs[pu_num]
        );
        pu->output_writeback(
            #ifndef __SYNTHESIS__
            pu_num,
            #endif
            init_fwd[pu_num][3],
            pu_active[pu_num],
            comp_out_valid,
            comp_out_empty,
            init_fwd[pu_num][4],
            comp_writeback_done[pu_num],
            result_ready[pu_num],
            comp_data_out,
            comp_out[pu_num]
        );
    }
}

void check_pus_done(
    bool result_ready[NUM_PE],
    vx_uint8& tmp_pus_done
)
{
    #pragma HLS inline off
    vx_uint8 tmp_result = tmp_pus_done;
    CHECK_PUS_DONE: for (vx_uint8 pu_num = 0; pu_num < NUM_PE; pu_num++) {
        //#pragma HLS pipeline II=1
        #pragma HLS unroll
        if (result_ready[pu_num]) {
            tmp_result++;
            PRINT("\n RUN_PU: pu[%i] finished computation | pus_done = %i", pu_num, tmp_result);
        }
    }
    tmp_pus_done = tmp_result;
}

void dataflow(
    const bool init,
    const bool pu_active[NUM_PE],
    vx_uint8& tmp_pus_done,
    vx_uint8& memOUT_write_ptr,
    const vx_uint8 memIN_read_ptr[NUM_PE],
    networkUnit* net,
    procUnit* pu,
    const vx_uint8 initCountTable[ROW_LEN],
    const procVec streamA[NUM_PE],
    proc_vt comp_out[NUM_PE],
    rep_t repMemB[NUM_PE][MEM_FIFO_SIZE],
    req_t reqMemB[MEM_FIFO_SIZE]
)
{
    #pragma HLS inline off
    bool result_ready[NUM_PE];
    vx_uint8 tmp_result = tmp_pus_done;
    run(
        init,
        pu_active,
        result_ready,
        pu,
        memIN_read_ptr,
        streamA,
        comp_out,
        repMemB,
        net,
        initCountTable,
        memOUT_write_ptr,
        reqMemB
    );
    check_pus_done(
        result_ready,
        tmp_result
    );
    tmp_pus_done = tmp_result;
}

void core_top(
    bool initialize,
    bool pu_active[NUM_PE],
    vx_uint8& pus_done,
    vx_uint8& memOUT_write_ptr,
    const vx_uint8 memIN_read_ptr[NUM_PE],
    const vx_uint8 initCountTable[ROW_LEN],
    const procVec streamA[NUM_PE],
    proc_vt comp_out[NUM_PE],
    req_t reqMemB[MEM_FIFO_SIZE],
    rep_t repMemB[NUM_PE][MEM_FIFO_SIZE]
)
{
    #pragma HLS array_partition variable=pu_active type=complete dim=1
    #pragma HLS array_partition variable=repMemB type=complete dim=1
    
    #pragma HLS interface s_axilite port=initialize
    #pragma HLS interface s_axilite port=pu_active[0]
    #pragma HLS interface s_axilite port=pu_active[1]
    #pragma HLS interface s_axilite port=pu_active[2]
    #pragma HLS interface s_axilite port=pu_active[3]
    #pragma HLS interface s_axilite port=pus_done
    #pragma HLS interface s_axilite port=memOUT_write_ptr
    #pragma HLS interface bram port=streamA->elems
    #pragma HLS interface bram port=reqMemB
    #pragma HLS interface bram port=repMemB[0], repMemB[1], repMemB[2], repMemB[3]

    static procUnit pu;
    static networkUnit net;
    static vx_uint8 tmp_pus_done = 0;

    if (initialize) tmp_pus_done = 0;
    dataflow(
        initialize,
        pu_active,
        tmp_pus_done,
        memOUT_write_ptr,
        memIN_read_ptr,
        &net,
        &pu,
        initCountTable,
        streamA,
        comp_out,
        repMemB,
        reqMemB
    );
    pus_done = tmp_pus_done;
}
