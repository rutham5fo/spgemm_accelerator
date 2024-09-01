#ifndef _CORE_TOP_H_
#define _CORE_TOP_H_

#include "network.h"

void run(
    const bool,
    const bool[NUM_PE],
    bool[NUM_PE],
    procUnit*,
    const vx_uint8[NUM_PE],
    const procVec[NUM_PE],
    proc_vt[NUM_PE],
    rep_t[NUM_PE][MEM_FIFO_SIZE],
    networkUnit*,
    const vx_uint8[ROW_LEN],
    vx_uint8&,
    req_t[MEM_FIFO_SIZE]
);

void check_pus_done(
    bool[NUM_PE],
    vx_uint8&
);

void dataflow(
    const bool,
    const bool[NUM_PE],
    vx_uint8&,
    vx_uint8&,
    const vx_uint8[NUM_PE],
    networkUnit*,
    procUnit*,
    const vx_uint8[ROW_LEN],
    const procVec[NUM_PE],
    proc_vt[NUM_PE],
    rep_t[NUM_PE][MEM_FIFO_SIZE],
    req_t[MEM_FIFO_SIZE]
);

void core_top(
    bool,
    bool[NUM_PE],
    vx_uint8&,
    vx_uint8&,
    const vx_uint8[NUM_PE],
    const vx_uint8[ROW_LEN],
    const procVec[NUM_PE],
    proc_vt[NUM_PE],
    req_t[MEM_FIFO_SIZE],
    rep_t[NUM_PE][MEM_FIFO_SIZE]
);

#endif