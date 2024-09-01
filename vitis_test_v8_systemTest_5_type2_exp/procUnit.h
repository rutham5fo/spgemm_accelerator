#ifndef _PROC_UNIT_H_
#define _PROC_UNIT_H_

#include "custom.h"
#include "hls_stream.h"

typedef procVecType proc_vt;
typedef procType proc_t;
typedef procTvec procVec;
typedef memRep rep_t;
typedef memReq req_t;

struct memUnit_regs
{
    rep_t regs_tmp_mem;
    rep_t regs_tmp_rep[FIFO_SIZE];
    vx_uint8 regs_mem_read_ptr;
    vx_uint8 regs_ext_read_ptr;
    vx_uint8 regs_rep_loc;
    bool regs_rep_full[FIFO_SIZE];
    bool regs_mem_full;
};

struct countUnit_regs
{
    vx_uint8 regs_availRows_fifo[FIFO_SIZE];
    vx_uint8 regs_availRows_cur;
    vx_uint8 regs_availRows_prev;
};

struct reqUnit_regs
{
    req_t regs_req_backup;
    vx_uint8 regs_reqRow_fifo[FIFO_SIZE];
    vx_uint8 regs_reqRow_cur;
    vx_uint8 regs_reqRow_prev;
    bool regs_backup_full;
};

struct servExtUnit_regs
{
    rep_t regs_rep_backup;
    req_t regs_serv_tmp;
    vx_uint8 regs_fifo_read_ptr;
    vx_uint8 regs_prev_req;
    vx_uint8 regs_prev_pe;
    bool regs_serv_ext_full;
    bool regs_backup_full;
};

struct compUnit_regs
{
    proc_t memPC0[ROW_LEN];
    proc_t memPC1[ROW_LEN];
    proc_t regs_val1;
    proc_t regs_val2;
    vx_uint8 regs_buf0_write;
    vx_uint8 regs_buf0_read;
    vx_uint8 regs_buf1_write;
    vx_uint8 regs_buf1_read;
    bool regs_sel_reBuf;
    bool regs_sel_curBuf;
    bool regs_val1_none;
    bool regs_val2_none;
    bool regs_lastA;
    bool regs_switch_cond1;
    bool regs_switch_cond2;
};

struct procUnit_regs
{
    procVecType aux_memB_1[MEMTABLE_SIZE];
    procVecType aux_memB_2[MEMTABLE_SIZE];
    procVecType memB[MEMTABLE_SIZE];
    proc_t fetch_data[PE_MEM_SIZE];
    /* --- TABLES --- */
    vx_uint8 memTable[MEMTABLE_SIZE];
    vx_uint8 aux_memTable_1[MEMTABLE_SIZE];
    vx_uint8 aux_memTable_2[MEMTABLE_SIZE];
    vx_uint8 countTable[PE_MEM_SIZE];
    /* --- COMMON REGS --- */
    vx_uint8 regs_prev_req_row[PE_MEM_SIZE];
    vx_uint8 aux_indA_0;
    vx_uint8 regs_fetch_ptr;
    bool regs_last_elemA;
};

class procUnit
{
    public:
        #ifndef __SYNTHESIS__
        void printMemTable(vx_uint8, vx_uint8*);
        void print_row(vx_uint8, procVecType, vx_uint8);
        void print_mem(vx_uint8, const procVecType*);
        #endif

        void update_count(
            #ifndef __SYNTHESIS__
            const vx_uint8[MEMTABLE_SIZE],
            const vx_uint8[MEMTABLE_SIZE],
            #endif
            const bool,
            const vx_uint8,
            const vx_uint8,
            vx_uint8[PE_MEM_SIZE],
            const vx_uint8,
            const vx_uint8,
            const vx_uint8,
            const vx_uint8,
            vx_uint8&,
            countUnit_regs&,
            req_t[EXT_FIFO_SIZE]
        );
        
        void update_memory(
            #ifndef __SYNTHESIS__
            const vx_uint8,
            #endif
            const bool,
            const vx_uint8,
            const vx_uint8,
            const vx_uint8,
            vx_uint8[PE_MEM_SIZE+1],
            procVecType[PE_MEM_SIZE+1],
            rep_t[MEM_FIFO_SIZE],
            rep_t[EXT_FIFO_SIZE],
            vx_uint8&,
            vx_uint8&,
            memUnit_regs&
        );

        void service_ext_request(
            #ifndef __SYNTHESIS__
            vx_uint8,
            #endif
            const bool,
            const vx_uint8,
            vx_uint8&,
            const vx_uint8[MEMTABLE_SIZE],
            const procVecType[MEMTABLE_SIZE],
            req_t[EXT_FIFO_SIZE],
            rep_t[EXT_FIFO_SIZE],
            vx_uint8&,
            servExtUnit_regs&
        );

        void fetchNext(
            #ifndef __SYNTHESIS__
            vx_uint8,
            #endif
            const bool,
            const bool,
            bool&,
            bool&,
            vx_uint8&,
            vx_uint8&,
            vx_uint8&,
            vx_uint8&,
	        const vx_uint8[MEMTABLE_SIZE],
            proc_t[PE_MEM_SIZE],
            vx_uint8[PE_MEM_SIZE],
            const procVecType[MEMTABLE_SIZE],
            proc_t&,
            proc_vt&,
            const procVec
        );

        void multiply(
            #ifndef __SYNTHESIS__
            vx_uint8,
            #endif
            const bool,
            bool&,
            bool&,
            bool&,
            const bool,
            const proc_t,
            const proc_vt,
            procVec&
        );

        void compare(
            #ifndef __SYNTHESIS__
            vx_uint8,
            #endif
            const bool,
            const bool,
            bool&,
            const bool,
            bool&,
            bool&,
            const procVec,
            //procVec&,
            proc_t[ROW_LEN],
            compUnit_regs&
        );

        void output_writeback(
            #ifndef __SYNTHESIS__
            vx_uint8,
            #endif
            const bool,
            const bool,
            const bool,
            const bool,
            bool&,
            bool&,
            bool&,
            proc_t[ROW_LEN],
            proc_vt&
        );
        
        void sync_aux(
            #ifndef __SYNTHESIS__
            vx_uint8,
            #endif
            const bool,
            const vx_uint8,
            const vx_uint8[MEMTABLE_SIZE],
            const procVecType[MEMTABLE_SIZE],
            vx_uint8&,
            vx_uint8[MEMTABLE_SIZE],
            vx_uint8[MEMTABLE_SIZE],
            vx_uint8[MEMTABLE_SIZE],
            procVecType[MEMTABLE_SIZE],
            procVecType[MEMTABLE_SIZE],
            procVecType[MEMTABLE_SIZE]
        );
        
        void update(
            #ifndef __SYNTHESIS__
            vx_uint8,
            #endif
            const bool,
            bool&,
            const bool,
            bool&,
            bool&,
            vx_uint8&,
            vx_uint8&,
            vx_uint8&,
            vx_uint8&,
	        const vx_uint8[MEMTABLE_SIZE],
            proc_t[PE_MEM_SIZE],
            vx_uint8[PE_MEM_SIZE],
            const procVecType[MEMTABLE_SIZE],
            proc_t&,
            proc_vt&,
            const procVec,

            const vx_uint8,
            const vx_uint8,
            const vx_uint8,
            vx_uint8[MEMTABLE_SIZE],
            procVecType[MEMTABLE_SIZE],
            rep_t[MEM_FIFO_SIZE],
            rep_t[EXT_FIFO_SIZE],
            vx_uint8&,
            vx_uint8&,
            memUnit_regs&,

            const vx_uint8,
            vx_uint8&,
            const vx_uint8[PE_MEM_SIZE+1],
            const procVecType[PE_MEM_SIZE+1],
            req_t[EXT_FIFO_SIZE],
            rep_t[EXT_FIFO_SIZE],
            vx_uint8&,
            servExtUnit_regs&
        );

        void reflect(
            #ifndef __SYNTHESIS__
            const vx_uint8[MEMTABLE_SIZE],
            #endif
            const bool,
            bool&,
            const vx_uint8,
            const vx_uint8,
            vx_uint8[PE_MEM_SIZE],
            const vx_uint8,
            const vx_uint8,
            const vx_uint8,
            const vx_uint8,
            vx_uint8&,
            countUnit_regs&,
            req_t[EXT_FIFO_SIZE],

            const vx_uint8[MEMTABLE_SIZE],
            const procVecType[MEMTABLE_SIZE],
            const vx_uint8,
            vx_uint8&,
            vx_uint8[MEMTABLE_SIZE],
            vx_uint8[MEMTABLE_SIZE],
            vx_uint8[MEMTABLE_SIZE],
            procVecType[MEMTABLE_SIZE],
            procVecType[MEMTABLE_SIZE],
            procVecType[MEMTABLE_SIZE]
        );
};

#endif
