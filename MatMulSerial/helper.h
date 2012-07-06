#ifndef _HELPER_H__
#define _HELPER_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <signal.h> /**< This is for catching SIG_INT and SIG_TERM */


// #define DT int
// #define DT long
// #define DT float
#define DT double

// #define FORMAT1 "%i, "
// #define FORMAT2 "%i\n"
// #define FORMAT1 "%li, "
// #define FORMAT2 "%li\n"
// #define FORMAT1 "%3.3e, "
// #define FORMAT2 "%3.3e\n"
#define FORMAT1 "%3.3le, "
#define FORMAT2 "%3.3le\n"

#ifndef DEBUG
#define DEBUG 0
#endif

#define INFO 3
#define WARNING 2
#define ERROR 1

/*!@brief Macro for executing the command Y if DEBUGLEVEL is equal to or larger
 * than X.\n Flushes stdout and stderr after the command.
 */
#define IDL(X,Y) if((DEBUG)>=(X)){(void)(Y);fflush(stdout);fflush(stderr);}

/*!@brief structure for a chained list that can safe values with
 * the precision of the double-type
 */
typedef struct list_t{
   /*!@brief value */
   double dnumber;
   /*!@brief pointer to next element of the list */
   struct list_t *pnext;
   }
list_t;

list_t *list; /**< parameterlist */
int listsize; /**< parameterlistsize */
double d_bi_start_sec; /**< start time */

extern char* sec_getenv(const char *env, int exitOnNull );
extern long int int_getenv(const char *env, int exitOnNull );
extern void parselist(const char *pcstring );
double get_list_element(int index);
double get_list_maxelement();
double get_list_minelement();
void free_list();
extern void safe_exit(int code);

void sigint_handler (int signum);
void sigterm_handler (int signum);
double mygettimeofday(void);
double gettime(void);

#endif //_HELPER_H__

