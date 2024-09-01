#ifndef _CUSTOM_H
#define _CUSTOM_H

#include "hls_vector.h"

#define ROW_LEN         31
#define COL_LEN         31

#define MULT_OUT_DEPTH  ROW_LEN+10
#define DELAY_DEPTH     3
#define REP_DELAY       DELAY_DEPTH+1

#define NUM_BLOCKS      1
#define BLOCK_SIZE      4
#define NUM_PE          (NUM_BLOCKS*BLOCK_SIZE)
#define PE_MEM_SIZE     5
#define MEMTABLE_SIZE   PE_MEM_SIZE+1
#define FIFO_SIZE       PE_MEM_SIZE+1
#define EXT_FIFO_SIZE   4*PE_MEM_SIZE
#define MEM_FIFO_SIZE   4*NUM_PE
#if ROW_LEN%NUM_PE == 0
#define BATCH_SIZE      (ROW_LEN/NUM_PE)
#else
#define BATCH_SIZE      (ROW_LEN/NUM_PE+1)
#endif
#define COUNT_INIT      (ROW_LEN/NUM_PE+1)

#define INSERT_VECdata(name, i)     name.values[i]
#define INSERT_VECindex(name, i)    name.indices[i]

#define INSERT_data(name)           name.value
#define INSERT_index(name)          name.index
#define INSERT_last(name)           name.last
#define INSERT_extra(name)           name.extra_flag

#define INSERT_reqRow(name)         name.row
#define INSERT_reqPe(name)          name.pe_num
#define INSERT_reqAt(name)          name.write_at
#define INSERT_reqNet(name)         name.net_dst

#define INSERT_repRow(name)         name.row
#define INSERT_repNum(name)         name.num
#define INSERT_repTo(name)          name.pe_num
#define INSERT_repAt(name)          name.at
#define INSERT_repCount(name)       name.count

#define APPEND(str1, str2)          str1##_##scpd##_##str2 //str1.scpd##_##str2
#define EQUATE(str1, str2)          str1 = str2
#define ACCESS(elem, index)         APPEND(elem, index)

#define ASSIGN_SC(sc, ...)          if (i == 0) EQUATE(ACCESS(sc, 0), __VA_ARGS__);         \
                                    else EQUATE(ACCESS(sc, 1), __VA_ARGS__);
        							//else if (i == 1) EQUATE(ACCESS(sc, 1), __VA_ARGS__);    \
        							//else EQUATE(ACCESS(sc, 2), __VA_ARGS__);

#define SHIFT_SC(sc)                EQUATE(ACCESS(sc, 0), ACCESS(sc, 1));
                                    //if (i == 0) EQUATE(ACCESS(sc, 0), ACCESS(sc, 1));       \
                                    //else EQUATE(ACCESS(sc, 1), ACCESS(sc, 2));

#ifndef __SYNTHESIS__
#define PRINT(expr, ...)		    printf(expr, ##__VA_ARGS__)
#define PRINT_MEM(...)			    printMemTable(__VA_ARGS__)

#define PRINT_INDEX(sc)                                                                             \
                                    if (i == 0) PRINT("\n Fetch_indA_0 = %i", ACCESS(sc, 0));          \
                                    else if (i == 1) PRINT("\n Fetch_indA_1 = %i", ACCESS(sc, 1));
                                    //else if (i == 2) PRINT("\n Fetch_indA_2 = %i", ACCESS(sc, 2));     \

#define PRINT_DATA(sc)                                                                             \
                                    if (i == 0) PRINT("\n Mult_dataA_0 = %i", ACCESS(sc, 0));          \
                                    else if (i == 1) PRINT("\n Mult_dataA_1 = %i", ACCESS(sc, 1));
                                    //else if (i == 2) PRINT("\n Mult_dataA_2 = %i", ACCESS(sc, 2));     \

#define CHECK_MODE                  mode
#define CHECK_NEG_MODE              !mode
#define CHECK_CONT                  cont

#else
#define PRINT(expr, ...)		    /*---- prints here ----*/
#define PRINT_MEM(...)			    /*---- prints here ----*/
#define PRINT_INDEX(sc)                /*---- prints here ----*/
#define PRINT_DATA(sc)                /*---- prints here ----*/

#define CHECK_MODE                  true
#define CHECK_NEG_MODE              true
#define CHECK_CONT                  true

#endif

typedef uint8_t vx_uint8;
typedef hls::vector<vx_uint8, ROW_LEN> csrvec;
//typedef hls::vector<vx_uint8, 1> csr;
typedef std::size_t size_t;

struct procVecType
{
    csrvec values;
    csrvec indices;
    size_t len;
};

struct procType
{
    vx_uint8 value;
    vx_uint8 index;
    bool last;
    bool extra_flag;
};

struct procTvec
{
    procType elems[ROW_LEN];
};

struct memReq
{
    vx_uint8 row;
    vx_uint8 pe_num;
    vx_uint8 write_at;
    vx_uint8 net_dst;
    vx_uint8 net_auto_row;
    bool net_auto;
    bool req_fail;
};

struct memRep
{
    procVecType row;
    vx_uint8 num;
    vx_uint8 pe_num;
    vx_uint8 at;
    vx_uint8 count;
    bool no_store;
    bool rep_fail;
};

#endif
