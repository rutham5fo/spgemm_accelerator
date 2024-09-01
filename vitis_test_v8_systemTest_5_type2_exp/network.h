#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "procUnit.h"

class networkUnit
{
    public:

        void init_regs(
            const vx_uint8,
            vx_uint8[NUM_PE][PE_MEM_SIZE],
            vx_uint8[NUM_PE],
            vx_uint8[NUM_PE],
            vx_uint8[NUM_PE],
            vx_uint8[NUM_PE],
            vx_uint8&
        );

        void init_memTable(
            const vx_uint8,
            vx_uint8[NUM_PE*PE_MEM_SIZE]
        );

        void init_countTable(
            const vx_uint8,
            const vx_uint8[ROW_LEN],
            vx_uint8[ROW_LEN]
        );

        void service_handler(
            #ifndef __SYNTHESIS__
            const vx_uint8,
            #endif
            const bool,
            bool&,
            const vx_uint8[ROW_LEN],
            const vx_uint8[NUM_PE*PE_MEM_SIZE],
            const vx_uint8,
            const vx_uint8,
            vx_uint8&,
            vx_uint8&,
            vx_uint8&,
            rep_t&,
            req_t&,
            bool&,
            bool&,
            req_t[EXT_FIFO_SIZE],
            rep_t[EXT_FIFO_SIZE]
        );

        void crossbar(
            const vx_uint8,
            const bool,
            const bool,
            const bool,
            const vx_uint8,
            const vx_uint8[ROW_LEN],
            vx_uint8&,
            vx_uint8[NUM_PE],
            vx_uint8[NUM_PE],
            vx_uint8[ROW_LEN],
            vx_uint8[NUM_PE*PE_MEM_SIZE],
            vx_uint8[NUM_PE][PE_MEM_SIZE],
            const rep_t,
            const req_t,
            req_t[NUM_PE][EXT_FIFO_SIZE],
            rep_t[NUM_PE][EXT_FIFO_SIZE],
            req_t[MEM_FIFO_SIZE]
        );
};

#endif