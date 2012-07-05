#ifndef MATRIX_H
#define MATRIX_H

#include "helper.h"

extern DT **MatxMat(DT * matrixS, int *mS, int *nS, DT * matrixA, int *mA,
                    int *nA, DT * matrixB, int *mB, int *nB,
                    GridPosition gridpos, int cpu_x, int cpu_y,
                    void (*ptr_to_perm) (DT * matrixS, DT * matrixA,
                                         DT * matrixB, int mA, int nA, int nB));

extern void mult_ijk(DT * matrixS, DT * matrixA, DT * matrixB, int mA, int nA,
                     int nB);
extern void mult_ikj(DT * matrixS, DT * matrixA, DT * matrixB, int mA, int nA,
                     int nB);
extern void mult_jik(DT * matrixS, DT * matrixA, DT * matrixB, int mA, int nA,
                     int nB);
extern void mult_jki(DT * matrixS, DT * matrixA, DT * matrixB, int mA, int nA,
                     int nB);
extern void mult_kij(DT * matrixS, DT * matrixA, DT * matrixB, int mA, int nA,
                     int nB);
extern void mult_kji(DT * matrixS, DT * matrixA, DT * matrixB, int mA, int nA,
                     int nB);

/*
extern DT ** pruef_MatxMat( int mA, int nA, int mB, int nB, DT ** matrixA, int _mA, int _nA, DT ** matrixB, int _mB, int _nB, GridPosition gridpos, int cpu_x, int cpu_y );
extern DT ** gatherMatrix( int m, int n, DT ** matrix, int _m, int _n, GridPosition gridpos, int cpu_x, int cpu_y );
*/

extern DT *createMatrix(int m, int n);
extern void clearMatrix(DT * matrix);
extern void printMatrix(DT * matrix, int m, int n);
extern void initZERO(DT * matrix, int m, int n);
extern void initRandomMatrix(DT * matrix, int m, int n);

int iMax(int, int);
int iMin(int, int);
#endif                                 // MATRIX_H

