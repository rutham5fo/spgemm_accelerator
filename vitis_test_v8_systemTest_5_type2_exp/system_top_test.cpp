#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "system_top.h"

#include "system_top.cpp"
#include "core_top.cpp"
#include "procUnit.cpp"
#include "network.cpp"

using namespace std;

void print_row(procVecType row, vx_uint8 row_num)
{
    printf ("\n\n MAIN: Row[%i] | len = %i | (val, index): \n [ ", row_num, row.len);
    for (vx_uint8 i=0; i < row.len; i++) {
        printf ("(%i, %i), ", INSERT_VECdata(row, i), INSERT_VECindex(row, i));
    }
    printf ("] \n\n");
}

void gen_sparse_mat(ifstream& file, vx_uint8 start_loc, procVecType* matA, vx_uint8& lenA, vx_uint8& density, bool& mat_valid)
{
    static vx_uint8 matnum = 0;
    if (file.is_open()) {
        //printf("\n MAIN: Input File opended for reading");
        bool exit = false;
        procVecType row = {0};
        string line;
        vx_uint8 row_num = 1;
        vx_uint8 i = 0;
        vx_uint8 index = 0;
        lenA = 0;
        
        while (!exit) {
            bool row_complete = false;
            bool ignore_line = false;
            bool start = (matnum >= start_loc-1) ? true : false;
            mat_valid = (matnum >= start_loc-1) ? true : false;
            getline(file, line);
            if (line.length() == 0) continue;
            if (line == "--END--") {
                ++matnum;
                if (start) {
                    exit = true;
                    //printf("\n MAIN: Matrix A[%i] complete", matnum);
                }
            }
            else if (start) {
                string::iterator si;
                string str = "";
                //printf ("\n MAIN: Read line = %s", line.c_str());
                // Extract information and populate row
                for (si = line.begin(); si < line.end(); si++) {
                    if (*si == '/') {
                        ignore_line = true;
                        break;
                    }
                    else if (*si == '%') {
                        si += 1;
                        while (si < line.end()) {
                            str += *si;
                            si += 1;
                        }
                        density = (vx_uint8)stoi(str);
                        ignore_line = true;
                        break;
                    }
                    else if (*si == '[' || *si == ' ') continue;
                    else if (*si == '0') {
                        index++;
                        continue;
                    }
                    else if (*si == ']') {
                        row_complete = true;
                        continue;
                    }
                    //printf ("\n MAIN: valid data = %i, index = %i", (vx_uint8)(*si-'0'), index);
                    INSERT_VECdata(row, i) = (vx_uint8)(*si-'0');
                    INSERT_VECindex(row, i) = index;
                    row.len++;
                    index++;
                    i++;
                }
                if (!ignore_line && row_complete) {
                    //print_row(row, row_num);
                    matA[lenA] = row;
                    row.len = 0;
                    i = 0;
                    index = 0;
                    lenA++;
                    row_num++;
                }
            }
        }
    }
}

bool check(ifstream& file, vx_uint8 start_loc, procVecType* in_mat)
{
    static vx_uint8 matnum = 0;
    if (file.is_open()) {
        //printf("\n\n CHECK: Check File opended for reading");
        bool exit = false;
        procVecType row = {0};
        string line;
        vx_uint8 i = 0;
        vx_uint8 index = 0;
        vx_uint8 mat_ind = 0;
        
        while (!exit) {
            bool ignore_line = false;
            bool row_complete = false;
            bool start = (matnum >= start_loc-1) ? true : false;
            getline(file, line);
            if (line.length() == 0) continue;
            if (line == "--END--") {
                ++matnum;
                if (start) {
                    exit = true;
                    //printf("\n MAIN: Matrix C[%i] complete", matnum);
                }
            }
            else if (start) {
                bool read = true;
                string::iterator si;
                string str = "";
                //printf ("\n CHECK: Read line = %s", line.c_str());
                // Extract information and populate row
                for (si = line.begin(); si < line.end(); si++) {
                    //printf ("\n CHECK: Read char = %c", *si);
                    if (*si == '/') {
                        ignore_line = true;
                        break;
                    }
                    else if (*si == '[') continue;
                    else if (*si == ']' || *si == ' ') {
                        if (!read) {
                            INSERT_VECdata(row, i) = (vx_uint8)(stoi(str));
                            INSERT_VECindex(row, i) = index;
                            row.len++;
                            index++;
                            i++;
                            //printf ("\n CHECK: valid data = %i, index = %i, row.len = %i", stoi(str), index, row.len);
                            str = "";
                            read = true;
                        }
                        if (*si == ']') row_complete = true;
                        continue;
                    }
                    else if (*si == '0' && read) {
                        index++;
                        continue;
                    }
                    str += *si;
                    read = false;
                }
                if (str.length() != 0 && (str != " " || str != "[" || str != "]")) {
                    INSERT_VECdata(row, i) = (vx_uint8)(stoi(str));
                    INSERT_VECindex(row, i) = index;
                    row.len++;
                    index++;
                    i++;
                    //printf ("\n CHECK: valid data = %i, index = %i, row.len = %i", stoi(str), index, row.len);
                    str = "";
                    read = true;
                }
                if (!ignore_line && row_complete) {
                    //printf ("\n CHECK: Row = %i | read line row.len = %i | in_mat row.len = %i", mat_ind, row.len, in_mat[mat_ind].len);
                    // Compare row with in_mat
                    //printf ("\n CHECK: File (val, index) =/!= In_mat (val, index)");
                    if (row.len == in_mat[mat_ind].len) {
                        for (vx_uint8 l = 0; l < row.len; l++) {
                            if (row.indices[l] == in_mat[mat_ind].indices[l] && row.values[l] == in_mat[mat_ind].values[l]) {
                                //printf ("\n CHECK: (%i, %i) = (%i, %i)", row.values[l], row.indices[l], in_mat[mat_ind].values[l], in_mat[mat_ind].indices[l]);
                            }
                            else {
                                //printf ("\n CHECK: (%i, %i) != (%i, %i)", row.values[l], row.indices[l], in_mat[mat_ind].values[l], in_mat[mat_ind].indices[l]);
                                return false;
                            }
                        }
                    }
                    else {
                        //printf ("\n CHECK: Row len mismatch");
                        return false;
                    }
                    mat_ind++;
                    i = 0;
                    index = 0;
                    row.len = 0;
                }
            }
        }
    }

    return true;
}

int main()
{
    // Generate sparse matrix in CSR format with random data, of size NxN
    vx_uint8 begin_mat_num = 1;
    vx_uint8 end_mat_num = 11;
    uint32_t cycle_count = 0;
    uint32_t caching_count = 0;
    uint32_t access_count = 0;
    vx_uint8 density = 0;
    vx_uint8 lenA = 0;
    vx_uint8 lenC = 0;
    procVecType matA[COL_LEN];
    procVecType matC[COL_LEN];
    bool system_start = false;
    bool system_ready = false;
    bool mat_valid = false;

    // Open the file where matrix A is
    // Matrix A is generated by a python script "matGen.py" and manually stored in "matA.txt"
    //ifstream fileA("matA_20x20.txt");
    //ifstream fileC("matC_20x20.txt");
    //ofstream fileO("metrics_20x20.txt");

    ifstream fileA("matA_32x32.txt");
    ifstream fileC("matC_32x32.txt");
    ofstream fileO("metrics_32x32.txt");

    for (vx_uint8 mats = begin_mat_num-1; mats < end_mat_num; mats++) {
        gen_sparse_mat(fileA, begin_mat_num, matA, lenA, density, mat_valid);
        system_start = true;
        system_ready = !mat_valid;
        cycle_count = 0;
        //printf("\n MAIN: starting system");
        auto t1 = chrono::high_resolution_clock::now();
        while (!system_ready) {
            system_top(
                system_start,
                system_ready,
                caching_count,
                access_count,
                matA,
                matA,
                matC
            );
            cycle_count++;
            //PRINT("\n MAIN: Cycles = %i | caching_count = %i | access_count = %i", cycle_count, caching_count, access_count);
            system_start = false;
        }
        auto t2 = chrono::high_resolution_clock::now();
        auto runtime = chrono::duration_cast<chrono::milliseconds>(t2-t1);
        //printf ("\n\n MAIN: OUTPUT MatrixC:");
        //for (vx_uint8 i = 0; i < ROW_LEN; i++) {
        //    print_row(matC[i], i);
        //}
        // Check computed output with control
        bool valid = check(fileC, begin_mat_num, matC);
        //printf ("\n\n MAIN: Output[%i] | density = %i | Valid = %i | runtime (mili_sec) = %i \n\n", mats+1, density, valid, runtime.count());
        // Output metrics to file
        string out_str = "Mat_num = " + to_string(mats+1) + "\t\t" + "| Density(%) = " + to_string(density) + "\t\t" + "| Valid(bool) = " + to_string(valid) + "\t\t" + "| cycles = " + to_string(cycle_count) + "\t\t" + "| runtime(mili_sec) = " + to_string(runtime.count()) + "\t\t" + "| memory_access_count = " + to_string(access_count) + "\t\t" + "| caching_count = " + to_string(caching_count) + "\n";
        printf ("\n Metrics: %s", out_str.c_str());
        fileO << out_str;
        if (!valid) return 1;
    }
    return 0;
}
