#include "network.h"

void networkUnit::init_regs(
    const vx_uint8 handle,
    vx_uint8 regs_prev_reqs[NUM_PE][PE_MEM_SIZE],
    vx_uint8 regs_reqIN_ptr[NUM_PE],
    vx_uint8 regs_repIN_ptr[NUM_PE],
    vx_uint8 reqOUT_ptr[NUM_PE],
    vx_uint8 repOUT_ptr[NUM_PE],
    vx_uint8& memReq_write_ptr
)
{
    #pragma HLS inline off
    INIT_NETWORK_1: for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
        //vx_uint8 a = i/PE_MEM_SIZE;
        vx_uint8 b = i%PE_MEM_SIZE;
        regs_prev_reqs[handle][b] = 0;
    }
    INIT_NETWORK_2: for (vx_uint8 j = 0; j < NUM_PE; j++) {
        regs_reqIN_ptr[j] = 0;
        regs_repIN_ptr[j] = 0;
        reqOUT_ptr[j] = 0;
        repOUT_ptr[j] = 0;
    }
    memReq_write_ptr = 0;
}

void networkUnit::init_memTable(
    const vx_uint8 handle,
    vx_uint8 memTable[NUM_PE*PE_MEM_SIZE]
)
{
    #pragma HLS inline off
    //INIT_NETWORK_2: for (vx_uint8 j = 0; j < NUM_BLOCKS*BLOCK_SIZE*PE_MEM_SIZE; j++) {
    //    memTable[j] = 0;
    //}
    INIT_NETWORK_3: for (vx_uint8 j = 0; j < PE_MEM_SIZE && handle != NUM_PE; j++) {
        memTable[handle*PE_MEM_SIZE+j] = 0;
    }
}

void networkUnit::init_countTable(
    const vx_uint8 handle,
    const vx_uint8 initCount[ROW_LEN],
    vx_uint8 batchCountTable[ROW_LEN]
)
{
    #pragma HLS inline off

    vx_uint8 count = 0;
    vx_uint8 index = 0;
    INIT_NETWORK_4: for (vx_uint8 k = 0; k < COUNT_INIT; k++) {
        #pragma HLS pipeline off
        index = handle*COUNT_INIT+k;
        if (index < ROW_LEN) {
            count = initCount[index];
            batchCountTable[index] = count;
        }
    }
}

void networkUnit::service_handler(
    #ifndef __SYNTHESIS__
    const vx_uint8 handle,
    #endif
    const bool init,
    bool& init_fwd,
    const vx_uint8 batchCountTable[ROW_LEN],
    const vx_uint8 memTable[NUM_PE*PE_MEM_SIZE],
    const vx_uint8 reqIN_read_ptr,
    const vx_uint8 repIN_read_ptr,
    vx_uint8& regs_reqIN_ptr,
    vx_uint8& regs_repIN_ptr,
    vx_uint8& count_writeback,
    rep_t& fwd_rep,
    req_t& fwd_req,
    bool& rep_valid,
    bool& req_valid,
    req_t ext_reqIN[EXT_FIFO_SIZE],
    rep_t ext_repIN[EXT_FIFO_SIZE]
)
{
    #pragma HLS inline off
    //#pragma HLS function_instantiate variable=handle
    bool tmp_init = init;
    bool tmp_rep_valid = false;
    bool tmp_req_valid = false;
    bool ignore = false;
    bool auto_req = false;
    vx_uint8 tmp_batchCount = 0;
    vx_uint8 tmp_autoWriteback = 0;
    vx_uint8 tmp_countWriteback = 0;
    vx_uint8 auto_row = 0;
    vx_uint8 dest_pu = NUM_PE+1;
    vx_uint8 tmp_reqIN_ptr = (!init) ? regs_reqIN_ptr : 0;
    vx_uint8 tmp_repIN_ptr = (!init) ? regs_repIN_ptr : 0;
    req_t tmp_req = ext_reqIN[tmp_reqIN_ptr];
    rep_t tmp_rep = ext_repIN[tmp_repIN_ptr];

    if (tmp_repIN_ptr != repIN_read_ptr) {
        tmp_repIN_ptr++;
        tmp_rep_valid = true;
    }
    if (tmp_reqIN_ptr != reqIN_read_ptr) {
        tmp_reqIN_ptr++;
        tmp_req_valid = true;
        SERVICE_FIND_AUTO: for (vx_uint8 j = 0; j < ROW_LEN; j++) {
            tmp_batchCount = batchCountTable[j];
            //PRINT("\n NETWORK: %i | tmp_batchCount[%i] = %i", handle, j, tmp_batchCount);
            if (tmp_batchCount != 0 && auto_row == 0) {
                auto_row = j+1;
                tmp_autoWriteback = tmp_batchCount;
                //break;
            }
            if (INSERT_reqRow(tmp_req)-1 == j) {
                tmp_countWriteback = tmp_batchCount;
            }
        }
        PRINT("\n NETWORK: %i | SERVICE_REQUEST:", handle);
        PRINT("\n NETWORK %i | Current INFO -> Requesting pu = %i, requested row = %i, at %i", handle, INSERT_reqPe(tmp_req), INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req));
        SERVICE_REQUEST_1: for (vx_uint8 i = 0; i < NUM_PE*PE_MEM_SIZE; i++) {
            PRINT("\n NETWORK: %i | memTable[%i] = %i", handle, i, memTable[i]);
            if (INSERT_reqRow(tmp_req) == memTable[i]) {
                dest_pu = i/PE_MEM_SIZE;
                PRINT("\n NETWORK: %i | Found row at pu[%i] | auto_req = %i", handle, dest_pu, (dest_pu != NUM_PE+1 && INSERT_reqAt(tmp_req) != PE_MEM_SIZE) ? true : false);
                //break;
            }
        }
        INSERT_reqNet(tmp_req) = dest_pu;
        auto_req = (dest_pu != NUM_PE+1 && INSERT_reqAt(tmp_req) != PE_MEM_SIZE) ? true : false;
        tmp_req.net_auto_row = auto_row;
        tmp_req.net_auto = auto_req;
    }
    fwd_rep = tmp_rep;
    fwd_req = tmp_req;
    rep_valid = tmp_rep_valid;
    req_valid = tmp_req_valid;
    count_writeback = (auto_req) ? tmp_autoWriteback : tmp_countWriteback;
    regs_reqIN_ptr = (tmp_reqIN_ptr == EXT_FIFO_SIZE) ? 0 : tmp_reqIN_ptr;
    regs_repIN_ptr = (tmp_repIN_ptr == EXT_FIFO_SIZE) ? 0 : tmp_repIN_ptr;
    init_fwd = tmp_init;
    //PRINT("\n NETWORK: %i | HANDLER_INFO: reqPE = %i, reqNet = %i, reqRow = %i, reqAt = %i | dest_pu = %i | auto_req = %i, auto_row = %i | tmp_autoWriteback = %i, tmp_countWriteback = %i, count_writeback = %i", handle, INSERT_reqPe(fwd_req), INSERT_reqNet(fwd_req), INSERT_reqRow(fwd_req), INSERT_reqAt(fwd_req), fwd_req.net_dst, fwd_req.net_auto, fwd_req.net_auto_row, tmp_autoWriteback, tmp_countWriteback, count_writeback);
}

void networkUnit::crossbar(
    const vx_uint8 handle,
    const bool init,
    const bool rep_valid,
    const bool req_valid,
    const vx_uint8 count_writeback,
    const vx_uint8 initCountTable[ROW_LEN],
    vx_uint8& mem_reqOUT_ptr,
    vx_uint8 out_reqOUT_ptr[NUM_PE],
    vx_uint8 out_repOUT_ptr[NUM_PE],
    vx_uint8 batchCountTable[ROW_LEN],
    vx_uint8 memTable[NUM_PE*PE_MEM_SIZE],
    vx_uint8 regs_prev_reqs[NUM_PE][PE_MEM_SIZE],
    const rep_t fwd_rep,
    const req_t fwd_req,
    req_t ext_reqOUT[NUM_PE][EXT_FIFO_SIZE],
    rep_t ext_repOUT[NUM_PE][EXT_FIFO_SIZE],
    req_t reqMemB[MEM_FIFO_SIZE]
)
{
    #pragma HLS inline off

    //PRINT("\n NETWORK: %i | CORSS_INFO: reqPE = %i, reqNet = %i, reqRow = %i, reqAt = %i | dest_pu = %i | auto_req = %i, auto_row = %i", handle, INSERT_reqPe(fwd_req), INSERT_reqNet(fwd_req), INSERT_reqRow(fwd_req), INSERT_reqAt(fwd_req), fwd_req.net_dst, fwd_req.net_auto, fwd_req.net_auto_row);

    req_t tmp_req = fwd_req;
    rep_t tmp_rep = fwd_rep;
    vx_uint8 rep_dest_pu = tmp_rep.pe_num;
    bool write_table = false;
    bool write_memTable = false;
    bool pu_fwd = false;
    bool mem_fwd = false;
    bool auto_req = tmp_req.net_auto;
    bool fwd_req_fail = false;
    bool reply_fwd = false;
    #ifndef __SYNTHESIS__
    bool valid = false;
    #endif

    vx_uint8 req_dest_pu = tmp_req.net_dst;
    vx_uint8 auto_row = tmp_req.net_auto_row;
    vx_uint8 tmp_writeback = count_writeback;
    vx_uint8 zero_index = (auto_req) ? ((auto_row != 0) ? auto_row-1 : 0) : ((INSERT_reqRow(tmp_req) != 0) ? INSERT_reqRow(tmp_req)-1 : 0);
    vx_uint8 memLoc = (tmp_req.pe_num*PE_MEM_SIZE)+tmp_req.write_at;
    vx_uint8 tmp_reqOUT_ptr[NUM_PE];
    vx_uint8 tmp_repOUT_ptr[NUM_PE];
    vx_uint8 tmp_memOUT_ptr = (!init) ? mem_reqOUT_ptr : 0;
    //vx_uint8 tmp_reqOUT_ptr_val;
    //vx_uint8 tmp_repOUT_ptr_val;
    #pragma HLS array_partition variable=tmp_reqOUT_ptr type=complete dim=0
    #pragma HLS array_partition variable=tmp_repOUT_ptr type=complete dim=0

    //PRINT("\n NETWORK: %i | CORSS_INFO: reqPE = %i, reqNet = %i, reqRow = %i, reqAt = %i | dest_pu = %i | auto_req = %i, auto_row = %i", handle, INSERT_reqPe(tmp_req), INSERT_reqNet(tmp_req), INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req), req_dest_pu, auto_req, auto_row);

    for (vx_uint8 i = 0; i < NUM_PE; i++) {
        tmp_reqOUT_ptr[i] = (!init) ? out_reqOUT_ptr[i] : 0;
        tmp_repOUT_ptr[i] = (!init) ? out_repOUT_ptr[i] : 0;
    }
    if (init) {
        init_memTable(
            handle,
            memTable
        );
        init_countTable(
            handle,
            initCountTable,
            batchCountTable
        );
        INIT_NETWORK_1: for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
            //vx_uint8 a = i/PE_MEM_SIZE;
            vx_uint8 b = i%PE_MEM_SIZE;
            regs_prev_reqs[handle][b] = 0;
        }
    }
    if (rep_valid && !init) {
        PRINT("\n NETWORK: %i | CROSS_INFO: forwarding reply for row[%i] from pu[%i] to dest_pu[%i]", handle, tmp_rep.num, handle, tmp_rep.pe_num);
        ext_repOUT[rep_dest_pu][tmp_repOUT_ptr[rep_dest_pu]] = tmp_rep;
        tmp_repOUT_ptr[rep_dest_pu]++;
    }
    if (req_valid && !init) {
        #ifndef __SYNTHESIS__
        valid = true;
        #endif
        PRINT("\n NETWORK: %i | CORSS_INFO: reqPE = %i, reqNet = %i, reqRow = %i, reqAt = %i | dest_pu = %i | auto_req = %i, auto_row = %i", handle, INSERT_reqPe(tmp_req), INSERT_reqNet(tmp_req), INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req), req_dest_pu, auto_req, auto_row);
        SERVICE_REQUEST_3: for (vx_uint8 i = 0; i < NUM_PE*PE_MEM_SIZE; i++) {
            vx_uint8 a = i/PE_MEM_SIZE;
            vx_uint8 b = i%PE_MEM_SIZE;
            //PRINT("\n NETWORK: %i | prev_reqs[%i][%i] = %i | prev_repsAt[%i][%i] = %i", handle, a, b, regs_prev_reqs[a][b], a, b, regs_prev_repAt[a][b]);
            if (INSERT_reqNet(tmp_req) == NUM_PE+1 && regs_prev_reqs[a][b] == INSERT_reqRow(tmp_req)) {
                PRINT("\n NETWORK %i | Request for row[%i] previously made by pu[%i], ignoring request by pu[%i]", handle, INSERT_reqRow(tmp_req), a, INSERT_reqPe(tmp_req));
                if (INSERT_reqAt(tmp_req) != PE_MEM_SIZE) {
                    auto_req = true;
                    PRINT("\n NETWORK: %i | Generating auto-req for pu[%i], row[%i], at %i | auto_req = %i", handle, INSERT_reqPe(tmp_req), auto_row, INSERT_reqAt(tmp_req), auto_req);
                }
                req_dest_pu = a;
                //break;
            }
        }
        if (INSERT_reqAt(tmp_req) != PE_MEM_SIZE && (req_dest_pu == NUM_PE+1 || (auto_req && auto_row != 0))) {
            for (vx_uint8 k = 0; k < PE_MEM_SIZE; k++) {
                if (k == PE_MEM_SIZE-1) regs_prev_reqs[INSERT_reqPe(tmp_req)][k] = (auto_req) ?  auto_row : INSERT_reqRow(tmp_req);
                else regs_prev_reqs[INSERT_reqPe(tmp_req)][k] = regs_prev_reqs[INSERT_reqPe(tmp_req)][k+1];
            }
        }
        //zero_index = (auto_req) ? auto_row-1 : INSERT_reqRow(tmp_req)-1;
        fwd_req_fail = (req_dest_pu == INSERT_reqPe(tmp_req)) ? true : false;
        pu_fwd = (req_dest_pu == NUM_PE+1 || fwd_req_fail) ? false : true;
        mem_fwd = (req_dest_pu == NUM_PE+1 || (auto_req && auto_row != 0)) ? true : false;
    }
    //puReqStream_full = ext_reqOUT[req_dest_pu].full();
    tmp_req.net_auto = auto_req;
    if (pu_fwd && !init) {
        tmp_req.req_fail = fwd_req_fail;
        PRINT("\n NETWORK: %i | Forwarding request from pu[%i] for row[%i] at %i to dest_pu[%i] | auto_req = %i | req_fail = %i | reqOUT_ptr = %i", handle, INSERT_reqPe(tmp_req), INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req), req_dest_pu, auto_req, tmp_req.req_fail, tmp_reqOUT_ptr[req_dest_pu]+1);
        ext_reqOUT[req_dest_pu][tmp_reqOUT_ptr[req_dest_pu]] = tmp_req;
        tmp_reqOUT_ptr[req_dest_pu]++;
    }
    if (mem_fwd && !init) {
        if (auto_req || INSERT_reqAt(tmp_req) != PE_MEM_SIZE) {
            tmp_writeback = 0;
            write_memTable = true;
        }
        else tmp_writeback--;
        tmp_req.req_fail = false;
        reqMemB[tmp_memOUT_ptr] = tmp_req;
        tmp_memOUT_ptr++;
        write_table = true;
        PRINT("\n NETWORK: %i | Forwarding request from pu[%i] for row[%i] at %i to MemB | auto_req = %i | req_fail = %i | aux_countTable[%i] = %i | memReqOUT_ptr = %i", handle, INSERT_reqPe(tmp_req), (auto_req) ? auto_row : INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req), auto_req, tmp_req.req_fail, zero_index, tmp_writeback, tmp_memOUT_ptr);
    }
    //req_full = (pu_write_fail || mem_write_fail) ? true : false;
    //#ifndef __SYNTHESIS__
    //if ((!pu_fwd || !mem_fwd) && valid) {
    //    if (!mem_fwd) PRINT("\n NETWORK: %i | Dropping mem request from pu[%i] for row[%i] at %i | dest_pu[%i]", handle, INSERT_reqPe(tmp_req), INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req), req_dest_pu);
    //    if (!pu_fwd) PRINT("\n NETWORK: %i | Dropping pu request from pu[%i] for row[%i] at %i | dest_pu[%i]", handle, INSERT_reqPe(tmp_req), INSERT_reqRow(tmp_req), INSERT_reqAt(tmp_req), req_dest_pu);
    //}
    //#endif
    //}
    if (write_table && !init) {
        batchCountTable[zero_index] = tmp_writeback;
        if (write_memTable) memTable[memLoc] = (auto_req) ? auto_row : tmp_req.row;
    }
    for (vx_uint8 j = 0; j < NUM_PE; j++) {
        //tmp_reqOUT_ptr_val = tmp_reqOUT_ptr[j];
        out_reqOUT_ptr[j] = (tmp_reqOUT_ptr[j] == EXT_FIFO_SIZE) ? 0 : tmp_reqOUT_ptr[j];
        out_repOUT_ptr[j] = (tmp_repOUT_ptr[j] == EXT_FIFO_SIZE) ? 0 : tmp_repOUT_ptr[j];
    }
    mem_reqOUT_ptr = (tmp_memOUT_ptr == MEM_FIFO_SIZE) ? 0 : tmp_memOUT_ptr;
}