#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "matmul.h"

#define NUM_FUNC 6

void init_( fds *myfds, int *size )
{
  register int x, y;
  long index;

  IDL( 3, printf( "field size: %ld bytes", ( long ) * size * ( *size ) * sizeof( DT ) ) );
  for ( x = 0; x < *size; x++ )
    for ( y = 0; y < *size; y++ )
    {
      index = x * ( *size ) + y;
      IDL( 5, printf( "%ld\n", index ) );
      /* Feld voller Zahlen zwisch 0 und 100 */
      myfds->feld1[ index ] = 30;
      /* Ein Feld voller Zahlen kleiner 0 */
      myfds->feld2[ index ] = 0.01;
      myfds->feld3[ index ] = 0.0;
    }
  IDL( 3, printf( "init fertig\n" ) );
}

void useversion_( int *version )
{
  switch ( *version )
  {
    case 0:
      entry1 = multaijk_;
      break;
    case 1:
      entry1 = multaikj_;
      break;
    case 2:
      entry1 = multajik_;
      break;
    case 3:
      entry1 = multajki_;
      break;
    case 4:
      entry1 = multakij_;
      break;
    case 5:
      entry1 = multakji_;
      break;
  }
}

void entry_( void *ptr, int *size )
{
  fds * myfds = ptr;
  double *f1 = myfds->feld1, *f2 = myfds->feld2, *f3 = myfds->feld3;
  if ( *size == 0 )
    return ;
  else
    entry1( f1, f2, f3, size );
}

double count_( int *version, int *size )
{
  double ulSize = 1.0 * *size;
  switch ( *version )
  {
    default:
      return 2.0 * ( ulSize ) * ( ulSize ) * ( ulSize );
  }
}

int main(int argc, char** argv)
{
    int output = 0;
    int ii, problemsize, k;
    char *p = 0;
    double dstart[NUM_FUNC], dend[NUM_FUNC], dtime[NUM_FUNC];
    double dres = 0.0, dresults[NUM_FUNC+1];
    fds *myfds;

    d_bi_start_sec = (double)((long long)mygettimeofday());

    /* setup signalhandlers */
    signal(SIGINT,sigint_handler);
    signal(SIGTERM,sigterm_handler);
    
    /* get and parse the list of matrix sizes */
    p = sec_getenv("PROBLEMLIST", 1);
    parselist(p);

    output = (int) int_getenv("OUTPUT", 1);

    /* initialize the random generator */
    srand((unsigned int)111);
    myfds = (fds *)malloc( sizeof( fds ) );

    /* big loop over the different Problemsizes defined in $PROBLEMLIST */
    for (ii=0;ii<listsize;ii++)
    {
        problemsize = (int)round(get_list_element(ii));
        IDL(INFO, printf("\nproblemsize: %d\n", problemsize));

        /* test all permutations */
        for (k = 0; k < NUM_FUNC; k++) {
            myfds->feld1 = malloc( ( problemsize ) * ( problemsize ) * sizeof( DT ) );
            IDL( 3, printf( "Alloc 1 done\n" ) );
            myfds->feld2 = malloc( ( problemsize ) * ( problemsize ) * sizeof( DT ) );
            IDL( 3, printf( "Alloc 2 done\n" ) );
            myfds->feld3 = malloc( ( problemsize ) * ( problemsize ) * sizeof( DT ) );
            IDL( 3, printf( "Alloc 3 done\n" ) );

            init_(myfds, &problemsize);
            useversion_(&k);
            IDL(INFO, printf("Version: %d<%d\t", k, NUM_FUNC));
            dstart[k] = gettime();
            entry_( myfds, &problemsize );
            dend[k] = gettime();
            IDL(INFO, printf("start: %.3f \t stop: %.3f\t", dstart[k], dend[k] ));
            
            free( myfds->feld1 );
            myfds->feld1 = NULL;
            free( myfds->feld2 );
            myfds->feld2 = NULL;
            free( myfds->feld3 );
            myfds->feld3 = NULL;
        }
        /* calculate the FLOPS */
        dres = count_(&k, &problemsize);

        /* calculate the used time */
        for (k = 0; k < NUM_FUNC; k++)
        {
            dtime[k] = dend[k] - dstart[k];
        }
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

        printf("\n");
        for (k = 0; k <= NUM_FUNC; k++)
        {
            printf("%.3f\t", dresults[k]);
        }
        
        //free(myfds);
    }
    safe_exit(0);
    exit(0); /*to eliminate compiler-warning */
}

