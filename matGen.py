import numpy as np
import scipy.sparse as sp
import sys

"""
Sparse Matrix Generator
"""
def matGen(dim, dense, form ='coo'):

    # The random distribution can be changed by setting the data_rvs attribute.
    # Since the focus is to test the memory cost and not MACs,
    # we randomize the matrix with 'data_rvs=np.ones'.
    return sp.random(dim, dim, density =dense, format =form, dtype =np.int8, data_rvs =np.ones)

"""
Main function
"""
def main():
    while True:
        fa = open("matA_out.txt", "w")
        fc = open("matC_out.txt", "w")
        dimension = int(input("\nEnter array dimension (20 - 32): "))
        #density = float(input("Enter density (0 - 1): "))
        density = [0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
        for d in density:
            # Generate matrix
            spmat_A = matGen(dimension, d, form ='csr')
            """
            # Check for zero rows/cols
            print (" Checking no. of cols in matA -> nrows = ", spmat_A.indptr)
            regen = False
            for x in range(len(spmat_A.indptr)-1):
                if spmat_A.indptr[x] == spmat_A.indptr[x+1]:
                    print ("\n Matrix contains row full of zeroes, generate another matrix")
                    regen = True
            if regen: continue
            """
            # Print sparse matrix A
            print ("\nGenerated Sparse matrix A:\n", spmat_A.toarray(order='C'))
            print ("\n\n Indices: ", spmat_A.indices, " Indexptr: ", spmat_A.indptr, " Data: ", spmat_A.data)
            fa.write("// Density = "+str(d)+"\n")
            fa.write("%"+str(d*100)+"\n")
            fa.write(np.array2string(spmat_A.toarray(order='C')))
            fa.write("\n--END--\n\n")
            print ("\nInner product AxA:\n", spmat_A.dot(spmat_A).toarray(order='C'))
            fc.write("// Density = "+str(d)+"\n")
            fc.write(np.array2string(spmat_A.dot(spmat_A).toarray(order='C')))
            fc.write("\n--END--\n\n")

        excode = input("\nEnter 'q' to terminate program (anything else to continue): ")

        if excode == 'q':
            break

if __name__ == "__main__":
    main()