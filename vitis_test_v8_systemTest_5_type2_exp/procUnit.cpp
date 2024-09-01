#include "procUnit.h"

#ifndef __SYNTHESIS__
void procUnit::printMemTable(vx_uint8 name, vx_uint8* memTable)
{
	PRINT("\n");
    for (vx_uint8 i=0; i < PE_MEM_SIZE; i++){
        PRINT("\n NAME: %i | memTable[%i] = %i", name, i, memTable[i]);
    }
    PRINT("\n");
}

void procUnit::print_row(vx_uint8 name, procVecType row, vx_uint8 num){
    PRINT("\n NAME: %i | row[%i] = ", name, num);
    for (vx_uint8 i = 0; i < row.len; i++) {
        PRINT("(%i, %i), ", INSERT_VECdata(row, i), INSERT_VECindex(row, i));
    }
}

void procUnit::print_mem(vx_uint8 name, const procVecType* mem)
{
    PRINT("\n Memory:");
    for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
        PRINT("\n NAME: %i | MemB[%i] = ", name, i);
        for (vx_uint8 j = 0; j < ROW_LEN; j++) {
            if (j < mem[i].len) {
                PRINT("(%i, %i), ", INSERT_VECdata(mem[i], j), INSERT_VECindex(mem[i], j));
            }
        }
    }
}
#endif

void procUnit::update_count(
    #ifndef __SYNTHESIS__
    const vx_uint8 aux_memTable[MEMTABLE_SIZE],
    const vx_uint8 memTable[MEMTABLE_SIZE],
    #endif
    const bool init,
    const vx_uint8 regs_name,
    const vx_uint8 req_row,
    vx_uint8 countTable[PE_MEM_SIZE],
    const vx_uint8 ext_count,
    const vx_uint8 count_set,
    const vx_uint8 count_val,
    const vx_uint8 fetch_count,
    vx_uint8& out_write_ptr,
    countUnit_regs& countRegs,
    req_t ext_reqOUT[EXT_FIFO_SIZE]
)
{
    #pragma HLS inline off
    bool write_req = false;
    bool block_loc = false;
    vx_uint8 req_loc = PE_MEM_SIZE;
    vx_uint8 weight[PE_MEM_SIZE];
    vx_uint8 row_cur_ptr =  (!init) ? countRegs.regs_availRows_cur : 0;
    vx_uint8 row_prev_ptr = (!init) ? countRegs.regs_availRows_prev : 0;
    vx_uint8 write_ptr = (!init) ? out_write_ptr : 0;
    req_t tmp_req = {0};
    INSERT_reqPe(tmp_req) = regs_name;
    tmp_req.net_auto_row = 0;
    tmp_req.net_dst = NUM_PE+1;
    tmp_req.net_auto = false;
    #pragma HLS array_partition variable=weight type=complete dim=0

    //PRINT("\n NAME: %i | UPDATE_COUNT:", regs_name);

    //#ifndef __SYNTHESIS__
    //PRINT("\n COUNTTABLE_OLD: \n");
    //for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
    //    PRINT("\n counttable[%i] = %i | memTable[%i] = %i", i, countTable[i], i, aux_memTable[i]);
    //}
    //#endif

    if (req_row != 0) {
        //PRINT("\n NAME: %i | Pushing req_row %i into fifo", regs_name, req_row);
        countRegs.regs_availRows_fifo[row_cur_ptr] = req_row;
        row_cur_ptr++;
    }
    CPY_COUNT: for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
        weight[i] = (!init) ? countTable[i] : 0;
    }
    UP_COUNT_1: for (vx_uint8 i = 0; i < 3; i++) {
        if (i == 0 && count_set != PE_MEM_SIZE+1) {
            vx_uint8 tmp = count_set;
            vx_uint8 tval = count_val;
            if (tmp < PE_MEM_SIZE) {
                weight[tmp] = tval;
                //PRINT("\n NAME: %i | Setting countTable[%i] = %i", regs_name, tmp, weight[tmp]);
            }
        }
        else if (i == 1 && ext_count != PE_MEM_SIZE) {
            weight[ext_count]--;
        }
        else if (i == 2 && fetch_count != PE_MEM_SIZE) {
            weight[fetch_count]--;
        }
    }
    SET_COUNT: for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
        //#pragma HLS pipeline II=1
        //#pragma HLS dependence variable=block_loc type=intra true
        //PRINT("\n NAME: %i | weight[%i] = %i", regs_name, i, weight[i]);
        if (row_cur_ptr != row_prev_ptr && weight[i] == 0 && req_loc == PE_MEM_SIZE) {
            req_loc = i;
            block_loc = true;
        }
        countTable[i] = (!block_loc) ? weight[i] : NUM_PE+1;
        block_loc = false;
        //PRINT("\n NAME: %i | new_countTable[%i] = %i", regs_name, i, countTable[i]);
    }
    if (row_cur_ptr != row_prev_ptr) {
        //PRINT("\n NAME: %i | requesting row %i at = %i", regs_name, countRegs.regs_availRows_fifo[row_prev_ptr], req_loc);
        INSERT_reqAt(tmp_req) = req_loc;
        INSERT_reqRow(tmp_req) = countRegs.regs_availRows_fifo[row_prev_ptr];
        row_prev_ptr++;
        //ext_reqOUT.write(tmp_req);
        ext_reqOUT[write_ptr] = tmp_req;
        write_ptr++;
    }
    out_write_ptr = (write_ptr == EXT_FIFO_SIZE) ? 0 : write_ptr;
    countRegs.regs_availRows_cur = (row_cur_ptr != FIFO_SIZE) ? row_cur_ptr : 0;
    countRegs.regs_availRows_prev = (row_prev_ptr != FIFO_SIZE) ? row_prev_ptr : 0;
    //#ifndef __SYNTHESIS__
    //PRINT("\n COUNTTABLE_NEW: \n");
    //for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
    //    PRINT("\n counttable[%i] = %i | memTable[%i] = %i", i, countTable[i], i, memTable[i]);
    //}
    //#endif
}

void procUnit::update_memory(
    #ifndef __SYNTHESIS__
    const vx_uint8 regs_name,
    #endif
    const bool init,
    const vx_uint8 mem_repIN_read_ptr,
    const vx_uint8 ext_repIN_read_ptr,
    const vx_uint8 aux_indA_scpd_0,
    vx_uint8 memTable[MEMTABLE_SIZE],
    procVecType memB[MEMTABLE_SIZE],
    rep_t mem_repIN[MEM_FIFO_SIZE],
    rep_t ext_repIN[EXT_FIFO_SIZE],
    vx_uint8& count_set,
    vx_uint8& count_val,
    memUnit_regs& memRegs
)
{
    #pragma HLS inline off
    bool tmp_rep_full[FIFO_SIZE] = {0};
    bool tmp_mem_full = (!init) ? memRegs.regs_mem_full : false;
    bool apply_write = false;
    vx_uint8 mem_read_ptr = (!init) ? memRegs.regs_mem_read_ptr : 0;
    vx_uint8 ext_read_ptr = (!init) ? memRegs.regs_ext_read_ptr : 0;
    vx_uint8 rep_loc = (!init) ? memRegs.regs_rep_loc : 0;
    vx_uint8 write_loc = MEMTABLE_SIZE+1;
    vx_uint8 write_num = 0;
    vx_uint8 write_count = 0;
    vx_uint8 tmp_count_set = PE_MEM_SIZE;
    vx_uint8 tmp_count_val = 0;
    vx_uint8 stream_sel = 0;
    vx_uint8 rep_sel = 0;
    const procVecType zero_row = {0};
    procVecType write_row;
    rep_t tmp_rep[FIFO_SIZE];
    rep_t tmp_mem = memRegs.regs_tmp_mem;
    rep_t tmp_ext = ext_repIN[ext_read_ptr];

    #pragma HLS array_partition variable=tmp_rep_full type=complete dim=1
    #pragma HLS array_partition variable=tmp_rep type=complete dim=1

    INIT_FIFOS: for (vx_uint8 y = 0; y < FIFO_SIZE; y++) {
        //#pragma HLS pipeline II=1
        tmp_rep_full[y] = (!init) ? memRegs.regs_rep_full[y] : false;
        tmp_rep[y] = memRegs.regs_tmp_rep[y];
        memTable[y] = 0;
        memB[y] = zero_row;
    }
    READ_REPS: for (vx_uint8 stream_sel = 0; stream_sel < 2; stream_sel++) {
        if (stream_sel == 0 && mem_read_ptr != mem_repIN_read_ptr && !tmp_mem_full && !init) {
            tmp_mem = mem_repIN[mem_read_ptr];
            mem_read_ptr++;
            tmp_mem_full = true;
        }
        else if (stream_sel == 1 && ext_read_ptr != ext_repIN_read_ptr && !tmp_rep_full[rep_loc] && !init) {
            tmp_rep[rep_loc] = tmp_ext;
            //PRINT("\n NAME: %i | Reading ext_reply for row[%i] at %i | ext_read_ptr = %i, repIN_read_ptr = %i", regs_name, tmp_rep[rep_loc].num, tmp_rep[rep_loc].at, ext_read_ptr, ext_repIN_read_ptr);
            ext_read_ptr++;
            tmp_rep_full[rep_loc] = true;
            rep_loc++;
        }
    }
    //#ifndef __SYNTHESIS__
    //PRINT("\n NAME: %i | tmp_mem row = %i, at = %i | tmp_mem_full = %i", regs_name, INSERT_repNum(tmp_mem), INSERT_repAt(tmp_mem), tmp_mem_full);
    //for (vx_uint8 i = 0; i < FIFO_SIZE; i++) {
    //    PRINT("\n NAME: %i | tmp_rep[%i] row = %i, at = %i | tmp_rep_full[%i] = %i", regs_name, i, tmp_rep[i].num, tmp_rep[i].at, i, tmp_rep_full[i]);
    //}
    //#endif
    UPDATE_MEM: for (vx_uint8 i = 0; i < FIFO_SIZE+1; i++) {
        if (i == FIFO_SIZE) {
            if (tmp_mem_full) {
                write_loc = INSERT_repAt(tmp_mem);
                write_num = INSERT_repNum(tmp_mem);
                write_count = INSERT_repCount(tmp_mem);
                write_row = INSERT_repRow(tmp_mem);
                tmp_count_set = (write_loc != PE_MEM_SIZE) ? write_loc : PE_MEM_SIZE;
                tmp_count_val = write_count;
                tmp_mem_full = (write_loc == PE_MEM_SIZE && write_num != aux_indA_scpd_0) ? true : false;
                if (!(write_loc == PE_MEM_SIZE && write_num != aux_indA_scpd_0)) {
                    //PRINT("\n NAME: %i | Writing row[%i] to memB[%i] with count = %i | count_set = %i, count_val = %i", regs_name, write_num, write_loc, write_count, write_loc, write_count);
                    memB[write_loc] = write_row;
                    memTable[write_loc] = write_num;
                }
            }
        }
        else {
            if (tmp_rep_full[rep_sel] && rep_sel < FIFO_SIZE) {
                write_loc = PE_MEM_SIZE;
                write_num = INSERT_repNum(tmp_rep[rep_sel]);
                write_count = INSERT_repCount(tmp_rep[rep_sel]);
                write_row = INSERT_repRow(tmp_rep[rep_sel]);
                tmp_rep_full[rep_sel] = (write_num == aux_indA_scpd_0) ? false : true;
                if (write_num == aux_indA_scpd_0) {
                    //PRINT("\n NAME: %i | Writing row[%i] to memB[%i] with count = %i | count_set = %i, count_val = %i", regs_name, write_num, write_loc, write_count, write_loc, write_count);
                    memB[write_loc] = write_row;
                    memTable[write_loc] = write_num;
                }
            }
            rep_sel++;
        }
    }
    WRITEBACK: for (vx_uint8 w = 0; w < FIFO_SIZE; w++) {
        //#pragma HLS pipeline II=1
        memRegs.regs_rep_full[w] = tmp_rep_full[w];
        memRegs.regs_tmp_rep[w] = tmp_rep[w];
    }
    memRegs.regs_mem_read_ptr = (mem_read_ptr == MEM_FIFO_SIZE) ? 0 : mem_read_ptr;
    memRegs.regs_ext_read_ptr = (ext_read_ptr == EXT_FIFO_SIZE) ? 0 : ext_read_ptr;
    memRegs.regs_mem_full = tmp_mem_full;
    memRegs.regs_tmp_mem = tmp_mem;
    memRegs.regs_rep_loc = (rep_loc == FIFO_SIZE) ? 0 : rep_loc;
    count_set = tmp_count_set;
    count_val = tmp_count_val;
}

void procUnit::service_ext_request(
    #ifndef __SYNTHESIS__
    vx_uint8 regs_name,
    #endif
    const bool init,
    const vx_uint8 in_read_ptr,
    vx_uint8& out_write_ptr,
    const vx_uint8 service_memTable[MEMTABLE_SIZE],
    const procVecType service_memB[MEMTABLE_SIZE],
    req_t ext_reqIN[EXT_FIFO_SIZE],
    rep_t ext_repOUT[EXT_FIFO_SIZE],
    vx_uint8& ext_count,
    servExtUnit_regs& extRegs
)
{
    #pragma HLS inline off
    bool memFound = false;
    vx_uint8 tmp_count = PE_MEM_SIZE;
    vx_uint8 rowB = 0;
    vx_uint8 tmp_reset_loc = PE_MEM_SIZE;
    vx_uint8 prev_req = (!init) ? extRegs.regs_prev_req : 0;
    vx_uint8 prev_pe = (!init) ? extRegs.regs_prev_pe : 0;
    req_t tmp_in = extRegs.regs_serv_tmp;
    rep_t tmp_out;
    bool serv_ext_full = (!init) ? extRegs.regs_serv_ext_full : false;
    vx_uint8 read_ptr = (!init) ? extRegs.regs_fifo_read_ptr : 0;
    vx_uint8 write_ptr = (!init) ? out_write_ptr : 0;
    bool write_rep = false;
    req_t tmp_ext = ext_reqIN[read_ptr];

    //PRINT("\n NAME: %i | serv_ext_full = %i, in_read_ptr = %i, read_ptr = %i, write_ptr = %i | prev_req = %i, prev_pe = %i", regs_name, serv_ext_full, in_read_ptr, read_ptr, write_ptr, prev_req, prev_pe);
    if (!serv_ext_full && read_ptr != in_read_ptr && !init) {
        tmp_in = tmp_ext;
        read_ptr++;
        serv_ext_full = (INSERT_reqRow(tmp_in) == prev_req && INSERT_reqPe(tmp_in) == prev_pe) ? false : true;
    }
    //PRINT("\n NAME: %i | serv_ext_full = %i", regs_name, serv_ext_full);
    if (serv_ext_full) {
        //PRINT("\n NAME: %i | Servicing external request from PE[%i], row[%i] at %i | req_fail = %i", regs_name, INSERT_reqPe(tmp_in), INSERT_reqRow(tmp_in), INSERT_reqAt(tmp_in), tmp_in.req_fail);
        SERVICE_EXTERNAL_LOOP: for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
            if (INSERT_reqRow(tmp_in) == service_memTable[i]) {
                rowB = i;
                memFound = true;
                //break;
            }
        }
        if (memFound) {
            //PRINT("\n NAME: %i | Row found in PE[%i] mem[%i]", regs_name, regs_name, rowB);
            INSERT_repRow(tmp_out) = service_memB[rowB];
            INSERT_repNum(tmp_out) = INSERT_reqRow(tmp_in);
            INSERT_repAt(tmp_out) = PE_MEM_SIZE;
            INSERT_repCount(tmp_out) = INSERT_reqAt(tmp_in);
            INSERT_repTo(tmp_out) = INSERT_reqPe(tmp_in);
            tmp_out.no_store = false;
            tmp_out.rep_fail = false;
            tmp_count = rowB;
            write_rep = true;
            prev_req = INSERT_reqRow(tmp_in);
            prev_pe = INSERT_reqPe(tmp_in);
        }
    }
    if (write_rep) {
        ext_repOUT[write_ptr] = tmp_out;
        write_ptr++;
        serv_ext_full = false;
    }
    out_write_ptr = (write_ptr == EXT_FIFO_SIZE) ? 0 : write_ptr;
    extRegs.regs_fifo_read_ptr = (read_ptr == EXT_FIFO_SIZE) ? 0 : read_ptr;
    extRegs.regs_serv_tmp = tmp_in;
    extRegs.regs_serv_ext_full = serv_ext_full;
    extRegs.regs_prev_req = prev_req;
    extRegs.regs_prev_pe = prev_pe;
    ext_count = (!serv_ext_full) ? tmp_count : PE_MEM_SIZE;
    //PRINT("\n NAME: %i | ext_count = %i", regs_name, ext_count);
}

void procUnit::fetchNext (
    #ifndef __SYNTHESIS__
    vx_uint8 regs_name,
    #endif
    const bool init,
    const bool pu_active,
    bool& output_valid,
    bool& regs_last_elemA,
    vx_uint8& regs_cur_ptr,
    vx_uint8& shft_indA,
    vx_uint8& req_row,
    vx_uint8& fetch_count,
	const vx_uint8 aux_memTable[MEMTABLE_SIZE],
    proc_t fetch_data[PE_MEM_SIZE],
    vx_uint8 regs_prev_req_row[PE_MEM_SIZE],
    const procVecType aux_memB[MEMTABLE_SIZE],
    proc_t& data_out,
    proc_vt& memB_chnl,
    const procVec inA
)
{
    #pragma HLS inline off

    vx_uint8 fetch_indA_0 = INSERT_index(fetch_data[0]);
    vx_uint8 fetch_indA_1 = INSERT_index(fetch_data[1]);
    vx_uint8 mult_datA_0 = INSERT_data(fetch_data[0]);

    bool last_elemA = (!init) ? regs_last_elemA : false;
    bool tmp_flush = false;
    bool found_in_mem = false;
    bool second_found = false;
    bool prev_req_first = false;
    bool prev_req_second = false;
    bool tmp_output_valid = false;
    vx_uint8 tmp_req_row = 0;
    vx_uint8 tmp_sel = MEMTABLE_SIZE;
    vx_uint8 tmp_count = PE_MEM_SIZE;
    vx_uint8 cur_ptr = (!init) ? regs_cur_ptr : 0;
    proc_vt tmp_memB;
    proc_t tmp_data;
    proc_t tmp_inA = {0};

    //PRINT("\n NAME: %i | FetchNext():", regs_name);
    //PRINT("\n NAME: %i | ACCESS(row, 0) = %i , ACCESS(row, 1) = %i | dataA_0 = %i | cur_ptr = %i", regs_name, fetch_indA_0, fetch_indA_1, mult_datA_0, cur_ptr);

    PREV_REQ_FIND: for (vx_uint8 i = 0; i < PE_MEM_SIZE; i++) {
        if (fetch_indA_0 == regs_prev_req_row[i]) prev_req_first = true;
        if (fetch_indA_1 == regs_prev_req_row[i]) prev_req_second = true;
    }

    if (fetch_indA_0 != 0) {
        // Check updated mem table
        FETCH_LOOP_1: for (vx_uint8 i = 0; i < PE_MEM_SIZE+1; i++){
            if (fetch_indA_0 == aux_memTable[i]) {
                tmp_sel = i;
                found_in_mem = true;
                tmp_count = i;
                //PRINT("\n NAME: %i | Found row[%i] at memTable[%i] | fetch_count = %i", regs_name, fetch_indA_0, i, tmp_count);
            }
            if (fetch_indA_1 == aux_memTable[i]) {
                second_found = true;
            }
        }
        
        tmp_flush = !found_in_mem;

        if (!found_in_mem && !prev_req_first){
            tmp_req_row = fetch_indA_0;
            //PRINT("\n NAME: %i | row[%i] not found in local mem", regs_name, fetch_indA_0);
        }
        else if (!second_found && !prev_req_second) {
            tmp_req_row = fetch_indA_1;
        }
        //PRINT("\n NAME: %i | Global flush = %i", regs_name, tmp_flush);
    }
    PREV_REQ_UP: for (vx_uint8 i = 0; i < PE_MEM_SIZE; i+=2) {
        if (tmp_req_row != 0 || init) {
            regs_prev_req_row[i] = (!init) ? regs_prev_req_row[i+1] : 0;
            regs_prev_req_row[i+1] = (!init) ? tmp_req_row : 0;
        }
    }
    MEMB_CHNL_SEL: for (vx_uint8 k = 0; k < PE_MEM_SIZE+1; k++) {
        #pragma HLS pipeline II=1
        tmp_memB = aux_memB[k];
        if (k == tmp_sel) {
            tmp_data = fetch_data[0];
            tmp_output_valid = true;
            //PRINT("\n NAME: %i | Fetch(): Pushing memB[%i] and fetch_ind = %i into channel | data_valid = %i", regs_name, k, INSERT_index(fetch_data[0]), tmp_output_valid);
            break;
        }
    }
    if (!tmp_flush) {
        if (pu_active && !last_elemA && !init) {
            tmp_inA = inA.elems[cur_ptr];
            last_elemA = INSERT_last(tmp_inA);
            cur_ptr++;
        }
        SHIFT_SCPD: for (vx_uint8 i = 0; i < PE_MEM_SIZE-1; i++) {
            fetch_data[i] = fetch_data[i+1];
        }
        fetch_data[PE_MEM_SIZE-1] = tmp_inA;
        //PRINT("\n NAME: %i | Setup_fetch(): fetch_indA_0 = %i, fetch_indA_1 = %i, mult_dataA_0 = %i, mult_dataA_1 = %i | cur_ptr = %i", regs_name, INSERT_index(fetch_data[0]), INSERT_index(fetch_data[1]), INSERT_data(fetch_data[0]), INSERT_data(fetch_data[1]), cur_ptr);
    }
    output_valid = tmp_output_valid;
    data_out = tmp_data;
    memB_chnl = tmp_memB;
    req_row = tmp_req_row;
    shft_indA = (!tmp_flush) ? fetch_indA_1 : fetch_indA_0;
    fetch_count = tmp_count;
    regs_cur_ptr = cur_ptr;
    regs_last_elemA = last_elemA;
}

void procUnit::multiply(
    #ifndef __SYNTHESIS__
    vx_uint8 regs_name,
    #endif
    const bool init,
    bool& init_fwd,
    bool& mult_out_valid,
    bool& force_finish,
    const bool data_valid,
    const proc_t data_in,
    const proc_vt row_in,
    procVec& mult_out
)
{
    #pragma HLS inline off

    bool tmp_out_valid = false;
    bool tmp_init = init;
    bool tmp_force_finish = false;
    proc_vt tmp_row = row_in;
    procVec tmp_mult_out;
    proc_t tmp_out;
    proc_t tmp_data = data_in;
    vx_uint8 tmp_mul_res;
    #pragma HLS bind_op variable=tmp_mul_res op=mul impl=dsp

    if (data_valid) {
        //PRINT("\n NAME: %i | Multiply: row_len = %i", regs_name, tmp_row.len);
        MULT_LOOP: for (vx_uint8 i = 0; i < ROW_LEN; i++) {
            #pragma HLS pipeline II=1
            if (i < tmp_row.len) {
                tmp_mul_res = INSERT_VECdata(tmp_row, i) * INSERT_data(tmp_data);
                INSERT_data(tmp_out) = tmp_mul_res;
                INSERT_index(tmp_out) = INSERT_VECindex(tmp_row, i);
                INSERT_last(tmp_out) = (i == tmp_row.len-1) ? true : false;
                INSERT_extra(tmp_out) = INSERT_last(tmp_data);
                tmp_mult_out.elems[i] = tmp_out;
                //PRINT("\n NAME: %i | Multiply: mult_dataA = %i, mult_indA = %i, mult_dataB = %i, mult_indB = %i | result = (%i, %i) | lastB = %i, lastA = %i", regs_name, INSERT_data(tmp_data), INSERT_index(tmp_data), INSERT_VECdata(tmp_row, i), INSERT_VECindex(tmp_row, i), INSERT_data(tmp_out), INSERT_index(tmp_out), INSERT_last(tmp_out), INSERT_extra(tmp_out));
            }
        }
        if (tmp_row.len != 0) tmp_out_valid = true;
        else if (tmp_row.len == 0 && INSERT_last(tmp_data)) tmp_force_finish = true;
    }
    mult_out_valid = tmp_out_valid;
    mult_out = tmp_mult_out;
    init_fwd = tmp_init;
    force_finish = tmp_force_finish;
}

void procUnit::compare(
    #ifndef __SYNTHESIS__
    vx_uint8 regs_name,
    #endif
    const bool init,
    const bool force_finish,
    bool& init_fwd,
    const bool mult_out_valid,
    bool& output_valid,
    bool& output_stream_empty,
    const procVec mult_out,
    proc_t writeback_stream[ROW_LEN],
    compUnit_regs& compRegs
)
{
    #pragma HLS inline off
    
    bool tmp_init = init;
    bool stream_last = false;
    bool rebuf_last = false;
    bool val1_none =        (!init) ? compRegs.regs_val1_none    : true;
    bool val2_none =        (!init) ? compRegs.regs_val2_none    : true;
    bool switch_cond1 =     (!init) ? compRegs.regs_switch_cond1 : false;
    bool switch_cond2 =     (!init) ? compRegs.regs_switch_cond2 : false;
    bool lastA =            (!init) ? compRegs.regs_lastA        : false;
    bool sel_curBuf =       (!init) ? compRegs.regs_sel_curBuf   : false;
    bool sel_reBuf =        (!init) ? compRegs.regs_sel_reBuf    : true;
    bool sel_curBuf_noswitch = sel_curBuf;
    bool sel_reBuf_noswitch = sel_reBuf;
    bool write_buf = false;
    bool tmp_output_valid = false;
    bool tmp_writeback_stream_empty = false;
    vx_uint8 mult_ptr = 0;
    vx_uint8 buf0_write =   (!init) ? compRegs.regs_buf0_write  : 0;
    vx_uint8 buf0_read =    (!init) ? compRegs.regs_buf0_read   : 0;
    vx_uint8 buf1_write =   (!init) ? compRegs.regs_buf1_write  : 0;
    vx_uint8 buf1_read =    (!init) ? compRegs.regs_buf1_read   : 0;
    vx_uint8 curBuf_write = (sel_curBuf) ? buf1_write : buf0_write;
    vx_uint8 curBuf_read = (sel_curBuf) ? buf1_read : buf0_read;
    vx_uint8 reBuf_write = (sel_reBuf) ? buf1_write : buf0_write;
    vx_uint8 reBuf_read = (sel_reBuf) ? buf1_read : buf0_read;
    procVec tmp_mult_out = mult_out;
    proc_t val1 = compRegs.regs_val1;
    proc_t val2 = compRegs.regs_val2;
    proc_t result;
    COMP_OUT_LOOP: for (vx_uint8 j = 0; j < ROW_LEN+1; j++) {
        #pragma HLS pipeline II=1
        #pragma HLS dependence variable=compRegs.memPC0 type=inter false
        #pragma HLS dependence variable=compRegs.memPC1 type=inter false
        //PRINT("\n NAME: %i | Compare(): mult_out.empty() = %i, val1_none = %i, val2_none = %i, lastA = %i | switch_cond1 = %i, switch_cond2 = %i | mult_out_ptr = %i, mult_out.len = %i", regs_name, (mult_ptr == tmp_mult_out.len) ? true : false, val1_none, val2_none, lastA, switch_cond1, switch_cond2, mult_ptr, tmp_mult_out.len);
        if (mult_out_valid && val1_none && !stream_last && !switch_cond1) {
            val1 = tmp_mult_out.elems[mult_ptr];
            val1_none = false;
            stream_last = INSERT_last(val1);
            lastA = INSERT_extra(val1);
            mult_ptr++;
        }
        else if (force_finish) {
            lastA = true;
            stream_last = true;
        }
        //PRINT("\n NAME: %i | Compare(): curBuf_read = %i, curBuf_write = %i | reBuf_read = %i, reBuf_write = %i | stream_last = %i", regs_name, curBuf_read, curBuf_write, reBuf_read, reBuf_write, stream_last);
        if (val2_none && reBuf_read != reBuf_write) {
            val2 = (sel_reBuf) ? compRegs.memPC1[reBuf_read] : compRegs.memPC0[reBuf_read];
            val2_none = false;
            rebuf_last = INSERT_last(val2);
            if (reBuf_read < ROW_LEN) reBuf_read += 1;
        }
        //PRINT("\n NAME: %i | Compare(): lastA = %i, val1_none = %i, val2_none = %i, val1 = (%i, %i), val2 = (%i, %i) | val1_last = %i, val2_last = %i | switch_cond1 = %i", regs_name, lastA, val1_none, val2_none, INSERT_data(val1), INSERT_index(val1), INSERT_data(val2), INSERT_index(val2), stream_last, rebuf_last, switch_cond1);
        if (!val1_none || !val2_none){
            // Perform Comparisons
            if ((!val1_none && !val2_none) && INSERT_index(val1) == INSERT_index(val2)){
                INSERT_data(result) = INSERT_data(val1) + INSERT_data(val2);
                INSERT_index(result) = INSERT_index(val1);
                INSERT_last(result) = (INSERT_last(val1) && INSERT_last(val2)) ? true : false;
                INSERT_extra(result) = false;
                val1_none = true;
                val2_none = true;
                write_buf = true;
                //PRINT("\n NAME: %i | Add and push -> sel_curBuf = %i, sel_reBuf = %i, result = (%i, %i) | last = %i", regs_name, sel_curBuf, sel_reBuf, INSERT_data(result), INSERT_index(result), INSERT_last(result));
            }
            else if (((!val1_none && !val2_none) && INSERT_index(val1) < INSERT_index(val2)) || (!val1_none && val2_none)){
                INSERT_data(result) = INSERT_data(val1);
                INSERT_index(result) = INSERT_index(val1);
                INSERT_last(result) = (val2_none) ? INSERT_last(val1) : false;
                INSERT_extra(result) = false;
                val1_none = true;
                write_buf = true;
                //PRINT("\n NAME: %i | Push val1 -> sel_curBuf = %i, sel_reBuf = %i, result = (%i, %i) | last = %i", regs_name, sel_curBuf, sel_reBuf, INSERT_data(result), INSERT_index(result), INSERT_last(result));
            }
            else {
                INSERT_data(result) = INSERT_data(val2);
                INSERT_index(result) = INSERT_index(val2);
                INSERT_last(result) = (val1_none) ? INSERT_last(val2) : false;
                INSERT_extra(result) = false;
                val2_none = true;
                write_buf = true;
                //PRINT("\n NAME: %i | Push val2 -> sel_curBuf = %i, sel_reBuf = %i, result = (%i, %i) | last = %i", regs_name, sel_curBuf, sel_reBuf, INSERT_data(result), INSERT_index(result), INSERT_last(result));
            }
            //PRINT("\n NAME: %i | Compare(): lastVal1 = %i, lastA = %i, val1_none = %i, val2_none = %i, mult_out.empty() = %i, reBuf.empty() = %i", regs_name, INSERT_last(val1), lastA, val1_none, val2_none, mult_out.empty(), ((sel_reBuf) ? memPC1.empty() : memPC0.empty()));
            if (val1_none && INSERT_last(val1) && !lastA) switch_cond1 = true;
            if (val2_none && reBuf_read == reBuf_write) switch_cond2 = true;
        }
        else{
            if (lastA && stream_last) {
                tmp_output_valid = true;
                tmp_writeback_stream_empty = (curBuf_write == 0) ? true : false;
                //PRINT("\n NAME: %i | FINISHED computation | lastVal1 = %i, lastA = %i, val1_none = %i, val2_none = %i, sel_curBuf = %i, sel_reBuf = %i, mult_out.empty() = %i, reBuf.empty() = %i", regs_name, INSERT_last(val1), lastA, val1_none, val2_none, sel_curBuf, sel_reBuf, !stream_last, (reBuf_read == reBuf_write) ? true : false);
            }
            lastA = false;
            switch_cond1 = false;
            switch_cond2 = false;
            write_buf = false;
        }
        // Write back
        if (write_buf) {
            (sel_curBuf) ? compRegs.memPC1[curBuf_write] = result : compRegs.memPC0[curBuf_write] = result;
            writeback_stream[curBuf_write] = result;
            if (curBuf_write < ROW_LEN) curBuf_write +=1;
        }
        if (switch_cond1 && switch_cond2) {
            sel_curBuf = !sel_curBuf;
            sel_reBuf = !sel_reBuf;
            switch_cond1 = false;
            switch_cond2 = false;
            if (reBuf_read == reBuf_write) {
                reBuf_write = 0;
                reBuf_read = 0;
            }
            //PRINT("\n NAME: %i | Switching buffers | sel_curBuf = %i, sel_reBuf = %i", regs_name, sel_curBuf, sel_reBuf);
        }
    }
    compRegs.regs_buf0_write = (sel_curBuf_noswitch) ? reBuf_write : curBuf_write;
    compRegs.regs_buf0_read = (sel_curBuf_noswitch) ? reBuf_read : curBuf_read;
    compRegs.regs_buf1_write = (sel_curBuf_noswitch) ? curBuf_write : reBuf_write;
    compRegs.regs_buf1_read = (sel_curBuf_noswitch) ? curBuf_read : reBuf_read;
    compRegs.regs_val1_none = val1_none;
    compRegs.regs_val2_none = val2_none;
    compRegs.regs_switch_cond1 = switch_cond1;
    compRegs.regs_switch_cond2 = switch_cond2;
    compRegs.regs_lastA = lastA;
    compRegs.regs_sel_curBuf = sel_curBuf;
    compRegs.regs_sel_reBuf = sel_reBuf;
    compRegs.regs_val1 = val1;
    compRegs.regs_val2 = val2;
    output_valid = tmp_output_valid;
    output_stream_empty = tmp_writeback_stream_empty;
    init_fwd = tmp_init;
}

void procUnit::output_writeback(
    #ifndef __SYNTHESIS__
    const vx_uint8 regs_name,
    #endif
    const bool init,
    const bool pu_active,
    const bool data_valid,
    const bool data_stream_empty,
    bool& init_fwd,
    bool& writeback_done,
    bool& result_ready,
    proc_t data[ROW_LEN],
    proc_vt& result
)
{
    #pragma HLS inline off

    bool tmp_init = init;
    bool tmp_done = (!init) ? writeback_done : false;
    bool tmp_ready = false;
    bool data_last = false;
    //vx_uint8 read_ptr = 0;
    proc_t tmp_data;
    proc_vt tmp_result;
    if (data_valid && !data_stream_empty && pu_active) {
        WRITEBACK_LOOP: for (vx_uint8 k = 0; k < ROW_LEN; k++) {
            if (!data_last) {
                tmp_data = data[k];
                data_last = INSERT_last(tmp_data);
                INSERT_VECdata(tmp_result, k) = INSERT_data(tmp_data);
                INSERT_VECindex(tmp_result, k) = INSERT_index(tmp_data);
                tmp_result.len = k+1;
                //PRINT("\n NAME: %i | COMPARE_OUT: out_val = (%i, %i) | len = %i | data_last = %i", regs_name, INSERT_VECdata(tmp_result, k), INSERT_VECindex(tmp_result, k), tmp_result.len, data_last);
            }
        }
        result = tmp_result;
        tmp_done = true;
        tmp_ready = true;
    }
    else if (data_valid && data_stream_empty && pu_active) {
        //PRINT("\n NAME: %i | COMPARE_OUT: force_finish, reseting result.len -> 0", regs_name);
        tmp_result.len = 0;
        result = tmp_result;
        tmp_done = true;
        tmp_ready = true;
    }
    else if (!pu_active) {
        //PRINT("\n NAME: %i | COMPARE_OUT: pu_deactivated, reseting result.len -> 0", regs_name);
        tmp_result.len = 0;
        result = tmp_result;
        tmp_done = false;
        tmp_ready = false;
    }
    init_fwd = tmp_init;
    writeback_done = tmp_done;
    result_ready = tmp_ready;
}

void procUnit::sync_aux(
    #ifndef __SYNTHESIS__
    vx_uint8 regs_name,
    #endif
    const bool init,
    const vx_uint8 shft_indA,
    const vx_uint8 update_memTable[MEMTABLE_SIZE],
    const procVecType update_memB[MEMTABLE_SIZE],
    vx_uint8& aux_indA_0,
    vx_uint8 memTable[MEMTABLE_SIZE],
    vx_uint8 aux_memTable_1[MEMTABLE_SIZE],
    vx_uint8 aux_memTable_2[MEMTABLE_SIZE],
    procVecType memB[MEMTABLE_SIZE],
    procVecType aux_memB_1[MEMTABLE_SIZE],
    procVecType aux_memB_2[MEMTABLE_SIZE]
)
{
    #pragma HLS inline off
    //#pragma HLS interface bram port=memB storage_type=ram_2p

    vx_uint8 table_val = 0;
    vx_uint8 table_write = 0;
    procVecType mem_val = {0};
    procVecType mem_write = {0};

    SYNC_1: for (vx_uint8 i = 0; i < MEMTABLE_SIZE; i++) {
        #pragma HLS pipeline off
        table_val = update_memTable[i];
        mem_val = update_memB[i];
        if (init) {
            memTable[i] = 0;
            aux_memTable_1[i] = 0;
            aux_memTable_2[i] = 0;
        }
        else if (table_val != 0) {
            memTable[i] = table_val;
            aux_memTable_1[i] = table_val;
            aux_memTable_2[i] = table_val;
            memB[i] = mem_val;
            aux_memB_1[i] = mem_val;
            aux_memB_2[i] = mem_val;
        }
    }
    aux_indA_0 = shft_indA;
}

void procUnit::update(
    #ifndef __SYNTHESIS__
    vx_uint8 regs_name,
    #endif
    const bool init,
    bool& fwd_init,
    const bool pu_active,
    bool& data_valid,
    bool& last_elemA,
    vx_uint8& regs_fetch_ptr,
    vx_uint8& shft_indA,
    vx_uint8& req_row,
    vx_uint8& fetch_count,
	const vx_uint8 aux_memTable[MEMTABLE_SIZE],
    proc_t fetch_data[PE_MEM_SIZE],
    vx_uint8 prev_req_rows[PE_MEM_SIZE],
    const procVecType aux_memB[MEMTABLE_SIZE],
    proc_t& data_out,
    proc_vt& memB_chnl,
    const procVec inA,

    const vx_uint8 mem_repIN_read_ptr,
    const vx_uint8 ext_repIN_read_ptr,
    const vx_uint8 aux_indA_0,
    vx_uint8 memTable[MEMTABLE_SIZE],
    procVecType memB[MEMTABLE_SIZE],
    rep_t mem_repIN[MEM_FIFO_SIZE],
    rep_t ext_repIN[EXT_FIFO_SIZE],
    vx_uint8& count_set,
    vx_uint8& count_val,
    memUnit_regs& memRegs,
    
    const vx_uint8 in_read_ptr,
    vx_uint8& out_write_ptr,
    const vx_uint8 service_memTable_2[PE_MEM_SIZE+1],
    const procVecType service_memB_2[PE_MEM_SIZE+1],
    req_t ext_reqIN[EXT_FIFO_SIZE],
    rep_t ext_repOUT[EXT_FIFO_SIZE],
    vx_uint8& ext_count,
    servExtUnit_regs& extRegs
)
{
    #pragma HLS inline off
    bool tmp_init = init;
    fetchNext(
        #ifndef __SYNTHESIS__
        regs_name,
        #endif
        init,
        pu_active,
        data_valid,
        last_elemA,
        regs_fetch_ptr,
        shft_indA,
        req_row,
        fetch_count,
        aux_memTable,
        fetch_data,
        prev_req_rows,
        aux_memB,
        data_out,
        memB_chnl,
        inA
    );
    update_memory(
        #ifndef __SYNTHESIS__
        regs_name,
        #endif
        init,
        mem_repIN_read_ptr,
        ext_repIN_read_ptr,
        aux_indA_0,
        memTable,
        memB,
        mem_repIN,
        ext_repIN,
        count_set,
        count_val,
        memRegs
    );
    service_ext_request(
        #ifndef __SYNTHESIS__
        regs_name,
        #endif
        init,
        in_read_ptr,
        out_write_ptr,
        service_memTable_2,
        service_memB_2,
        ext_reqIN,
        ext_repOUT,
        ext_count,
        extRegs
    );
    fwd_init = tmp_init;
}

void procUnit::reflect(
    #ifndef __SYNTHESIS__
    const vx_uint8 aux_memTable[PE_MEM_SIZE+1],
    #endif
    const bool init,
    bool& fwd_init,
    const vx_uint8 regs_name,
    const vx_uint8 req_row,
    vx_uint8 countTable[PE_MEM_SIZE],
    const vx_uint8 ext_count,
    const vx_uint8 count_set,
    const vx_uint8 count_val,
    const vx_uint8 fetch_count,
    vx_uint8& reqOUT_write_ptr,
    countUnit_regs& countRegs,
    req_t ext_reqOUT[EXT_FIFO_SIZE],

    const vx_uint8 update_memTable[MEMTABLE_SIZE],
    const procVecType update_memB[MEMTABLE_SIZE],
    const vx_uint8 shft_indA,
    vx_uint8& aux_indA_0,
    vx_uint8 memTable[MEMTABLE_SIZE],
    vx_uint8 aux_memTable_1[MEMTABLE_SIZE],
    vx_uint8 aux_memTable_2[MEMTABLE_SIZE],
    procVecType memB[MEMTABLE_SIZE],
    procVecType aux_memB_1[MEMTABLE_SIZE],
    procVecType aux_memB_2[MEMTABLE_SIZE]
)
{
    #pragma HLS inline off

    bool tmp_init = init;
    update_count(
        #ifndef __SYNTHESIS__
        aux_memTable,
        update_memTable,
        #endif
        init,
        regs_name,
        req_row,
        countTable,
        ext_count,
        count_set,
        count_val,
        fetch_count,
        reqOUT_write_ptr,
        countRegs,
        ext_reqOUT
    );
    sync_aux(
        #ifndef __SYNTHESIS__
        regs_name,
        #endif
        init,
        shft_indA,
        update_memTable,
        update_memB,
        aux_indA_0,
        memTable,
        aux_memTable_1,
        aux_memTable_2,
        memB,
        aux_memB_1,
        aux_memB_2
    );
    fwd_init = tmp_init;
}
