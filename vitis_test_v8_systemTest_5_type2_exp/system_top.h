#ifndef _SYSTEM_TOP_H_
#define _SYSTEM_TOP_H_

#include "core_top.h"

void sim_rep_delay(
    const bool,
    const vx_uint8[NUM_PE],
    vx_uint8[NUM_PE]
);

void access_memB(
    const bool,
    const vx_uint8,
    vx_uint8[NUM_PE],
    const procVecType[COL_LEN],
    vx_uint8[ROW_LEN],
    req_t[MEM_FIFO_SIZE],
    rep_t[NUM_PE][MEM_FIFO_SIZE],
    uint32_t&,
    uint32_t&
);

void scheduler(
    bool&,
    bool[NUM_PE],
    vx_uint8&,
    vx_uint8&,
    vx_uint8&,
    const procVecType[COL_LEN],
    vx_uint8[ROW_LEN],
    procVec[NUM_PE]
);

void copy_output(
    const vx_uint8,
    const procVec[NUM_PE],
    procVecType[COL_LEN]
);

void system_top(
    bool,
    bool&,
    uint32_t&,
    uint32_t&,
    const procVecType[COL_LEN],
    const procVecType[COL_LEN],
    procVecType[COL_LEN]
);

#endif