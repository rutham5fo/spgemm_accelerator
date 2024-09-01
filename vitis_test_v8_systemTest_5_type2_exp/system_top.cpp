#include "system_top.h"

#ifndef __SYNTHESIS__
void print_reqQ(
    bool init,
    vx_uint8 cur_ptr,
    req_t reqMemB[MEM_FIFO_SIZE],
    bool diff_mode = true
)
{
    static vx_uint8 prev_ptr = 0;
    bool cond = true;
    req_t tmp_req;
    if (init) prev_ptr = 0;
    if (diff_mode) {
        if (cur_ptr != prev_ptr) cond = true;
        else cond = false;
    }
    printf("\n TOP: cur_ptr = %i | diff_mode = %i | request memory queue vals:", cur_ptr, diff_mode);
    for (vx_uint8 i = 0; i < MEM_FIFO_SIZE && cond; i++) {
        tmp_req = reqMemB[i];
        printf("\n TOP: ptr = %i | pu = %i, row = %i, at = %i, auto_req = %i, auto_row = %i", i, tmp_req.pe_num, tmp_req.row, tmp_req.write_at, tmp_req.net_auto, tmp_req.net_auto_row);
    }
    prev_ptr = cur_ptr;
}
#endif

void sim_rep_delay(
    const bool init,
    const vx_uint8 rep_write_ptr[NUM_PE],
    vx_uint8 delayed_rep_write_ptr[NUM_PE]
)
{
    static vx_uint8 rep_delay[NUM_PE][REP_DELAY];
    #pragma HLS array_partition variable=rep_delay type=complete dim=1

    REP_DELAY_LOOP1: for (vx_uint8 i = 0; i < NUM_PE; i++) {
        REP_DELAY_LOOP2: for (vx_uint8 j = 0; j < REP_DELAY-1; j++) {
            rep_delay[i][j] = (!init) ? rep_delay[i][j+1] : 0;
            //PRINT("\n TOP: delayed rep_ptr[%i][%i] = %i", i, j, rep_delay[i][j]);
        }
        rep_delay[i][REP_DELAY-1] = (!init) ? rep_write_ptr[i] : 0;
        //PRINT("\n TOP: delayed rep_ptr[%i][%i] = %i", i, REP_DELAY-1, rep_delay[i][REP_DELAY-1]);
        delayed_rep_write_ptr[i] = rep_delay[i][0];
    }
}

void access_memB(
    const bool core_init,
    const vx_uint8 memReq_read_ptr,
    vx_uint8 memRep_write_ptr[NUM_PE],
    const procVecType matB_mem[COL_LEN],
    vx_uint8 matB_count[ROW_LEN],
    req_t net_req[MEM_FIFO_SIZE],
    rep_t net_rep[NUM_PE][MEM_FIFO_SIZE],
    uint32_t& caching_count,
    uint32_t& access_count
)
{
    //#pragma HLS array_partition variable=net_rep type=complete dim=1
    //#pragma HLS interface bram port=matB_mem
    //#pragma HLS interface bram port=net_req
    //#pragma HLS interface bram port=net_rep[0], net_rep[1], net_rep[2], net_rep[3]

    #pragma HLS inline off

    static vx_uint8 regs_read_ptr;

    bool repStream_full = false;
    bool write_reply = false;

    bool auto_req = 0;
    vx_uint8 get_index = ROW_LEN;
    vx_uint8 mem_count = 0;
    vx_uint8 dest_pu = NUM_PE;
    vx_uint8 tmp_read_ptr = (!core_init) ? regs_read_ptr : 0;
    vx_uint8 tmp_write_ptr[NUM_PE];
    uint32_t tmp_caching_count = caching_count;
    uint32_t tmp_access_count = access_count;
    rep_t tmp_rep = {0};
    req_t tmp_req = {0};

    INIT_REGS: for (vx_uint8 k = 0; k < NUM_PE; k++) {
        tmp_write_ptr[k] = (!core_init) ? memRep_write_ptr[k] : 0;
    }
    ACCESS_MEMB_LOOP: for (vx_uint8 i = 0; i < NUM_PE; i++) {
        if (tmp_read_ptr != memReq_read_ptr && !core_init) {
            PRINT("\n MEMORY: Servicing network requests | memReq_read_ptr = %i, regs_read_ptr = %i", memReq_read_ptr, tmp_read_ptr);
            tmp_req = net_req[tmp_read_ptr];
            tmp_read_ptr = (tmp_read_ptr == MEM_FIFO_SIZE-1) ? 0 : tmp_read_ptr+1;
            auto_req = tmp_req.net_auto;
            dest_pu = INSERT_reqPe(tmp_req);
            write_reply = true;
            PRINT("\n MEMORY: CURRENT_INFO: requesting pu[%i], req_row = [%i], req_at = [%i] | auto_req = %i, auto_row = %i", INSERT_reqPe(tmp_req), INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req), tmp_req.net_auto, tmp_req.net_auto_row);
            if (auto_req) {
                get_index = tmp_req.net_auto_row-1;
            }
            else get_index = tmp_req.row-1;
            PRINT("\n MEMORY: old_count = %i", matB_count[get_index]);
            mem_count = (INSERT_reqAt(tmp_req) == PE_MEM_SIZE) ? matB_count[get_index]-1 : 0;
            PRINT("\n MEMORY: new_count = %i", mem_count);
            INSERT_repRow(tmp_rep) = matB_mem[get_index];
            INSERT_repNum(tmp_rep) = get_index+1;
            INSERT_repAt(tmp_rep) = INSERT_reqAt(tmp_req);
            INSERT_repCount(tmp_rep) = matB_count[get_index];
            INSERT_repTo(tmp_rep) = dest_pu;
            tmp_rep.rep_fail = false;
            tmp_rep.no_store = false;
            PRINT("\n MEMORY: Forwarding reply from MemB for row[%i] to dest_pu[%i] at %i with count %i | repfail = %i", get_index+1, INSERT_repTo(tmp_rep), INSERT_repAt(tmp_rep), INSERT_repCount(tmp_rep), false);
            matB_count[get_index] = mem_count;
            #ifndef __SYNTHESIS__
            tmp_access_count++;
            if (INSERT_reqAt(tmp_req) != PE_MEM_SIZE) {
                tmp_caching_count++;
                PRINT("\n MEMORY: pu main_mem caching count = %i", tmp_caching_count);
            }
            PRINT("\n MEMORY: abs access count = %i", tmp_access_count);
            #endif
            if (write_reply && !repStream_full) {
                net_rep[dest_pu][tmp_write_ptr[dest_pu]] = tmp_rep;
                tmp_write_ptr[dest_pu] = (tmp_write_ptr[dest_pu] == MEM_FIFO_SIZE-1) ? 0 : tmp_write_ptr[dest_pu]+1;
                write_reply = false;
            }
        }
    }
    REGS_WRITEBACK: for (vx_uint8 j = 0; j < NUM_PE; j++) {
        memRep_write_ptr[j] = tmp_write_ptr[j];
    }
    regs_read_ptr = tmp_read_ptr;
    caching_count = tmp_caching_count;
    access_count = tmp_access_count;
}

void scheduler(
    //const bool run,
    bool& init,
    bool pus_active[NUM_PE],
    //const vx_uint8 batch,
    vx_uint8& batch,
    vx_uint8& pus_done,
    vx_uint8& done_count,
    const procVecType matrixA[COL_LEN],
    vx_uint8 batch_row_count[ROW_LEN],
    procVec streamA[NUM_PE]
)
{
    #pragma HLS inline off

    bool tmp_init = true;
    bool inc = false;
    bool activate_pu = false;
    vx_uint8 tmp_batch = batch;
    //vx_uint8 tmp_row_count[ROW_LEN];
    vx_uint8 tmp_pus_done = 0;
    vx_uint8 tmp_done_count = 0;
    vx_uint8 index;
    vx_uint8 rowA;
    proc_t tmpA;
    procVec tmp_vec_out;
    proc_vt tmp_matA_row;
    //#pragma HLS array_partition variable=tmp_row_count type=complete dim=1

    ROWB_COUNT_ZERO: for (vx_uint8 i = 0; i < ROW_LEN; i++) {
        batch_row_count[i] = 0;
    }
    LOAD_STREAMA_1: for (vx_uint8 pu_num = 0; pu_num < NUM_PE; pu_num++) {
        activate_pu = false;
        rowA = (tmp_batch*NUM_BLOCKS*BLOCK_SIZE)+pu_num;
        if (rowA < COL_LEN) {
            tmp_matA_row = matrixA[rowA];
            if (tmp_matA_row.len != 0) {
                tmp_done_count++;
                LOAD_STREAMA_2: for (vx_uint8 l = 0; l < ROW_LEN; l++) {
                    #pragma HLS pipeline II=1
                    #pragma HLS dependence variable=matrixA type=inter false
                    if (l < tmp_matA_row.len) {
                        index = INSERT_VECindex(tmp_matA_row, l);
                        inc = true;
                        INSERT_data(tmpA) = INSERT_VECdata(tmp_matA_row, l);
                        INSERT_index(tmpA) = INSERT_VECindex(tmp_matA_row, l)+1;
                        INSERT_last(tmpA) = (l == tmp_matA_row.len-1) ? true : false;
                        INSERT_extra(tmpA) = false;
                        tmp_vec_out.elems[l] = tmpA;
                    }
                    if (inc) {
                        batch_row_count[index]++;
                        inc = false;
                    }
                }
                streamA[pu_num] = tmp_vec_out;
                activate_pu = true;
            }
        }
        if (activate_pu) {
            pus_active[pu_num] = true;
            PRINT("\n LOADER: row %i assigned to pu[%i] | row_len = %i | done_count = %i", rowA, pu_num, tmp_matA_row.len, tmp_done_count);
        }
        else pus_active[pu_num] = false;
    }
    pus_done = tmp_pus_done;
    done_count = tmp_done_count;
    batch = tmp_batch+1;
    init = tmp_init;
}

void copy_output(
    const vx_uint8 batch,
    const proc_vt comp_output[NUM_PE],
    procVecType matrixC[COL_LEN]
)
{
    #pragma HLS pipeline off
    vx_uint8 tmp_batch = batch-1;
    COPY_OUTPUT_MATC: for (vx_uint8 i = 0; i < NUM_PE; i++) {
        matrixC[(tmp_batch*NUM_PE)+i] = comp_output[i];
    }
}

void system_top(
    bool start,
    bool& ready,
    uint32_t& caching_count,
    uint32_t& access_count,
    const procVecType matrixA[COL_LEN],
    const procVecType matrixB[COL_LEN],
    procVecType matrixC[COL_LEN]
)
{
    #pragma HLS interface ap_none port=start
    #pragma HLS interface ap_none port=ready
    #pragma HLS interface ap_none port=caching_count
    #pragma HLS interface ap_none port=access_count
    #pragma HLS interface bram port=matrixA
    #pragma HLS interface bram port=matrixB
    #pragma HLS interface bram port=matrixC

    // STREAMS
    static proc_vt comp_output[NUM_PE];
    static procVec streamA[NUM_PE];
    static req_t reqMemB[MEM_FIFO_SIZE];
    static rep_t repMemB[NUM_PE][MEM_FIFO_SIZE];
    #pragma HLS aggregate variable=comp_output
    #pragma HLS aggregate variable=streamA
    #pragma HLS aggregate variable=reqMemB
    #pragma HLS aggregate variable=repMemB

    // REGS
    static bool pu_active[NUM_PE];
    static bool reg_ready;
    static bool initialize;
    static bool core_run;
    static bool matCopy;
    static vx_uint8 batch;
    static vx_uint8 pus_done;
    static vx_uint8 done_count;
    static vx_uint8 pu_num;
    static vx_uint8 batch_row_count[ROW_LEN];
    static vx_uint8 memReq_write_ptr;
    static vx_uint8 memRep_write_ptr[NUM_PE];
    #pragma HLS array_partition variable=batch_row_count type=complete dim=1
    #pragma HLS array_partition variable=memRep_write_ptr type=complete dim=1

    // Simulation related regs
    static vx_uint8 delayed_memRep_write_ptr[NUM_PE];
    #pragma HLS array_partition variable=delayed_memRep_write_ptr type=complete dim=1

    bool tmp_init               = false;
    bool tmp_ready              = (!start) ? reg_ready : false;
    vx_uint8 tmp_batch          = (!start) ? batch : 0;
    vx_uint8 tmp_pus_done       = (!start) ? pus_done : 0;
    vx_uint8 tmp_done_count     = (!start) ? done_count : 0;
    uint32_t tmp_caching_count  = (!start) ? caching_count : 0;
    uint32_t tmp_access_count   = (!start) ? access_count : 0;

    PRINT("\n SYSTEM: start = %i, init = %i, batch = %i, pus_done = %i, done_count = %i, ready = %i", start, initialize, tmp_batch, tmp_pus_done, tmp_done_count, tmp_ready);
    if (tmp_pus_done == tmp_done_count && tmp_done_count != 0 && !tmp_ready) {
        copy_output(
            batch,
            comp_output,
            matrixC
        );
        tmp_ready = (batch == BATCH_SIZE) ? true : false;
    }
    if (tmp_pus_done == tmp_done_count && tmp_batch < BATCH_SIZE) {
        scheduler(
            tmp_init,
            pu_active,
            tmp_batch,
            tmp_pus_done,
            tmp_done_count,
            matrixA,
            batch_row_count,
            streamA
        );
    }
    else if (tmp_pus_done != tmp_done_count) {
        core_top(
            initialize,
            pu_active,
            tmp_pus_done,
            memReq_write_ptr,
            delayed_memRep_write_ptr,
            batch_row_count,
            streamA,
            comp_output,
            reqMemB,
            repMemB
        );
        #ifndef __SYNTHESIS__
        print_reqQ(
            initialize,
            memReq_write_ptr,
            reqMemB
        );
        #endif
        access_memB(
            initialize,
            memReq_write_ptr,
            memRep_write_ptr,
            matrixB,
            batch_row_count,
            reqMemB,
            repMemB,
            tmp_caching_count,
            tmp_access_count
        );
        sim_rep_delay(
            initialize,
            memRep_write_ptr,
            delayed_memRep_write_ptr
        );
    }
    reg_ready = tmp_ready;
    initialize = tmp_init;
    batch = tmp_batch;
    pus_done = tmp_pus_done;
    done_count = tmp_done_count;
    ready = tmp_ready;
    access_count = tmp_access_count;
    caching_count = tmp_caching_count;
}