#include "helper.h"
#include "matrix.h"
#include "mpicomm.h"

#include <signal.h> /**< This is for catching SIG_INT and SIG_TERM */

/* if this is compiled for a OpenMP-kernel,
 * somewhere you got to set -DUSE_OMP as compiler flag
 * to include OpenMP-header
 */
#ifdef USE_OMP
 #include <omp.h>
#endif

#define NUM_FUNC 6
//#define INVALID_MEASUREMENT -7.77E7

double d_bi_start_sec; /**< start time of program*/

/**
 * Signal handler for SIGINT
 */
static void sigint_handler (int signum)
{
//    char c='\0';
    int exitcode=0;

    /* ignore further signals while handling */
    signal(signum, SIG_IGN);

    if (rank==0)
    {
        printf("Aborting...\n");fflush(stdout);
        /*ToDo: do result writing and stuff only master does */
        printf("Finishing...\n");fflush(stdout);
    }
    safe_exit(exitcode);
    /* read remaining input */
    if (rank==0) while(fgetc(stdin)!='\n');
    /* reinstall handler */
    signal(SIGINT, sigint_handler);
}



/**
 * Signal handler for SIGTERM,
 */
static void sigterm_handler (int signum) {
    /* ignore further signals as we are going to quit anyway */
    signal(signum, SIG_IGN);
    if (rank==0)
    {
        fflush(stdout);printf("\nReceived SIGTERM, Aborting...\n");
        fflush(stdout);
        printf("Finishing...\n");fflush(stdout);
    }
    safe_exit(1);
}



static double mygettimeofday(void)
{
  struct timeval time;
  gettimeofday( &time, (struct timezone *) 0);
  return (double)time.tv_sec + (double)time.tv_usec * 1.0e-6;
}



static double gettime(void)
{
  struct timeval time;
  gettimeofday( &time, (struct timezone *) 0);
  return (double)(time.tv_sec - d_bi_start_sec) + (double)time.tv_usec * 1.0e-6;
}



int main(int argc, char** argv)
{
    int output = 0;
    char *p = 0;

    void (*ptr_to_perm[]) (DT * matrixS, DT * matrixA, DT * matrixB, int mA,
                          int nA, int nB) = {mult_ijk, mult_ikj, mult_jik,
                                             mult_jki, mult_kij, mult_kji};

    int mA, nA, mB, nB, ii, problemsize;
    int _mA, _nA, _mB, _nB, _mS, _nS, temp, i, j, k;
    int cpu_x = 0, cpu_y = 0;
    float fac;
    double dstart[NUM_FUNC], dend[NUM_FUNC], dtime[NUM_FUNC];
    double dres = 0.0, dresults[NUM_FUNC+1];
    DT *matrixA, *matrixB, *matrixS;
    DT **pointer;
    GridPosition gridpos;
    int temprank, resultsender;

    d_bi_start_sec = (double)((long long)mygettimeofday());

    MPI_Status status;
//    IDL(INFO,printf("MPI_Init()..."));
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
//    fprintf(stderr".%d.", rank); fflush(stderr);
    MPI_Barrier(MPI_COMM_WORLD);
//    IDL(INFO,printf(" [OK]\n"));

    /* setup signalhandlers */
    signal(SIGINT,sigint_handler);
    signal(SIGTERM,sigterm_handler);

    p = sec_getenv("PROBLEMLIST", 1);
    parselist(p);
/*
    p = sec_getenv("OUTPUT", 1);
    output = atoi(p);
*/    
    output = (int) int_getenv("OUTPUT", 1);

    MPI_Comm_rank(MPI_COMM_WORLD, &globalrank);
    MPI_Comm_size(MPI_COMM_WORLD, &globalsize);

    /* initialize the random generator for each processor */
    srand((unsigned int)globalrank * 111);

    /* big loop over the different Problemsizes defined in $PROBLEMLIST */
    for (ii=0;ii<listsize;ii++)
    {
        problemsize = (int)round(get_list_element(ii));
        IDL(INFO, printf("\nproblemsize: %d\n", problemsize));
        mA = problemsize;
        nA = problemsize;
        mB = problemsize;
        nB = problemsize;

        /* there are max. mA*nB processors, and the number squared */
        createMyMPIComm(globalsize, mA, nB);

        /* x*y cpu, x per row, y per column */
        fac = (float)mA / (float)nB;
        IDL(INFO, printf("\nfactor: %e", fac));
        cpu_x = (int) ceil(sqrt(size) * fac);
        cpu_y = (int) ceil(sqrt(size) / fac);
        IDL(INFO, printf("\n1. cpu per row: %i", cpu_x));
        IDL(INFO, printf("\n1. cpu per column: %i", cpu_y));
        
        while (1) {
          if (cpu_y < cpu_x) {
             if (cpu_x * cpu_y <= size) {
                break;
             } else {
                cpu_x--;
             }
          } else {
             if (cpu_x * cpu_y <= size) {
                break;
             } else {
                cpu_y--;
             }
          }
        }

        IDL(INFO, printf("\n2. cpu per row: %i", cpu_x));
        IDL(INFO, printf("\n2. cpu per column: %i", cpu_y));

        temp = 0;
        for (i = 0; i < cpu_y; i++) {
          for (j = 0; j < cpu_x; j++) {
             if (rank == temp) {
                gridpos.row = j;
                gridpos.column = i;
             }
             temp++;
          }
        }

        _mA = (int) ceil(mA / (float)cpu_x);
        if ((gridpos.row % cpu_x) == cpu_x - 1) {
            _mA = mA - (cpu_x - 1) * _mA;
        }

        _nA = (int) ceil(nA / (float)cpu_y);
        if ((gridpos.column % cpu_y) == cpu_y - 1) {
            _nA = nA - (cpu_y - 1) * _nA;
        }

        _mB = (int) ceil(mB / (float)cpu_x);
        if ((gridpos.row % cpu_x) == cpu_x - 1) {
            _mB = mB - (cpu_x - 1) * _mB;
        }

        _nB = (int) ceil(nB / (float)cpu_y);
        if ((gridpos.column % cpu_y) == cpu_y - 1) {
            _nB = nB - (cpu_y - 1) * _nB;
        }

        IDL(INFO,
            printf("\nrank=%i  _mA=%i, _nA=%i  _mB=%i, _nB=%i  gridpos=(%i, %i)",
              rank, _mA, _nA, _mB, _nB, gridpos.row, gridpos.column));

   /* get the actual time do the measurement / your algorythm get the actual
    * time */
        if (rank != MPI_UNDEFINED) {
            matrixA = createMatrix(_mA, _nA);
            matrixB = createMatrix(_mB, _nB);

            IDL(INFO, MPI_Barrier(mycomm));
            initRandomMatrix(matrixA, _mA, _nA);
            IDL(INFO, MPI_Barrier(mycomm));
            IDL(INFO, printf("Random MatrixA:\n"));
            IDL(INFO, printMatrix(matrixA, _mA, _nA));

            IDL(INFO, MPI_Barrier(mycomm));
            initRandomMatrix(matrixB, _mB, _nB);
            IDL(INFO, MPI_Barrier(mycomm));
            IDL(INFO, printf("Random MatrixB:\n"));
            IDL(INFO, printMatrix(matrixB, _mB, _nB));

            pointer =
             initialShift(matrixA, &(_mA), &(_nA), matrixB, &(_mB), &(_nB), gridpos,
                          cpu_x, cpu_y);
            if (pointer[0] != NULL)
             matrixA = pointer[0];
            if (pointer[1] != NULL)
             matrixB = pointer[1];
            free(pointer);

            for (k = 0; k < NUM_FUNC; k++) {
             _mS = _mA;
             _nS = _nB;
             matrixS = createMatrix(_mS, _nS);
             initZERO(matrixS, _mS, _nS);

             MPI_Barrier(mycomm);
             dstart[k] = gettime();
             pointer =
                MatxMat(matrixS, &(_mS), &(_nS), matrixA, &(_mA), &(_nA), matrixB,
                        &(_mB), &(_nB), gridpos, cpu_x, cpu_y, &(*ptr_to_perm[k]));
             MPI_Barrier(mycomm);
             dend[k] = gettime();
             if (pointer[0] != NULL)
                matrixA = pointer[0];
             if (pointer[1] != NULL)
                matrixB = pointer[1];
             free(pointer);

             clearMatrix(matrixS);
            }
            pointer =
             undoShift(matrixA, &(_mA), &(_nA), matrixB, &(_mB), &(_nB), gridpos,
                       cpu_x, cpu_y);
            if (pointer[0] != NULL)
             matrixA = pointer[0];
            if (pointer[1] != NULL)
             matrixB = pointer[1];
            free(pointer);

            clearMatrix(matrixA);
            clearMatrix(matrixB);
        }

        /* sometimes globalrank 0 is not rank 0 and so the dstart/dend are invalid */
        if (globalrank == 0) 
        {
            for (k = 1; k < globalsize; k++) 
            {
                MPI_Recv(&temprank, 1, MPI_INT, k, tag, MPI_COMM_WORLD, &status);
                if (temprank == 0) resultsender = k;
            }
        } 
        else 
        {
            MPI_Send(&rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
        }

        if (globalrank == 0 && rank != 0) 
        {
            MPI_Recv(dstart, NUM_FUNC, MPI_DOUBLE, resultsender, tag, MPI_COMM_WORLD,
                   &status);
            MPI_Recv(dend, NUM_FUNC, MPI_DOUBLE, resultsender, tag, MPI_COMM_WORLD,
                   &status);
        }

        if (rank == 0 && globalrank != 0) 
        {
            MPI_Send(dstart, NUM_FUNC, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
            MPI_Send(dend, NUM_FUNC, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        }

        if (globalrank == 0) 
        {
            /* calculate the FLOPS */
            dres = 2 * (1.0 * mA) * (1.0 * mA) * (1.0 * mA);

            /* calculate the used time */
            for (k = 0; k < NUM_FUNC; k++) 
            {
                dtime[k] = dend[k] - dstart[k];
            }

            /* store the results in results[1], results[2], ... * [1] for the first
            * function, [2] for the second function * and so on ... * the index 0
            * always keeps the value for the x axis */
            dresults[0] = (double)problemsize;
            if (output) {
                for (k = 0; k < NUM_FUNC; k++) 
                {
                    dresults[k + 1] = dtime[k];
                }
            } 
            else 
            {
                for (k = 0; k < NUM_FUNC; k++)
                {
                    dresults[k + 1] = dres / dtime[k];
                }
            }
        }

        if (rank==0){
            printf("\n");
            for (k = 0; k <= NUM_FUNC; k++)
            {
                printf("%.3f\t", dresults[k]);
            }
        }

    freeMyMPIComm();

    }

//    printf("Finishing...\n"); fflush(stdout);

    safe_exit(0);
    exit(0); /*to eliminate compiler-warning */
}

