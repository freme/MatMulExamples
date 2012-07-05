#ifndef MATMUL_H
#define MATMUL_H

#include "helper.h"

typedef struct floating_data_struct
{
      DT *feld1, *feld2, *feld3;
}
fds;


void ( *entry1 ) ( double *a, double *b, double *c, int *size );
extern void multaijk_( double *a, double *b, double *c, int *size );
extern void multaikj_( double *a, double *b, double *c, int *size );
extern void multajik_( double *a, double *b, double *c, int *size );
extern void multajki_( double *a, double *b, double *c, int *size );
extern void multakij_( double *a, double *b, double *c, int *size );
extern void multakji_( double *a, double *b, double *c, int *size );
extern double getlanguage_( void );

extern double d_bi_start_sec; /**< start time */

extern void sigint_handler (int signum);
extern void sigterm_handler (int signum);
extern double mygettimeofday(void);
extern double gettime(void);

void init_( fds *myfds, int *size );
void useversion_( int *version );
void entry_( void *ptr, int *size );
double count_( int *version, int *size );

#endif                                 // MATMUL_H
