#include "helper.h"
#include <errno.h>
#include <ctype.h>

char* sec_getenv(const char *env, int exitOnNull)
{
    char *res = 0;
    /* First try to read from Environment*/
    res = getenv(env);
    if (res!=NULL) return res;
    else
    {
        if ( exitOnNull > 0 )
        {

            fprintf (stderr, "sec_getenv(): env. variable %s not defined\n", env);
            fflush (stderr);
            (void) safe_exit (127);
        }      
    }
    return res;
}


long int int_getenv(const char *env, int exitOnNull)
{
  /* used for getting environment as string */
  const char *string = 0;
  /* end of the string */
  char *endptr = 0;
  /* what is computed out of the string */
  long int result = 0;
  /* was there an error? */
  int error = 0;
  /* get the environment variable to string */
  string = sec_getenv (env, exitOnNull );
  /* found? */
  if ( *string != 0 )
  {
    /* get the numereous meaning (ending char is a 0 (args)) and a decimal meaning (arg3) */
    result = strtol (string, &endptr, 10);
    /* couldnt translate? */
    if ( *endptr != 0 ) error++;
  }
  /* couldnt get env? */
  else error++;
  /* error? */
  if ( error > 0 )
  {
    if ( exitOnNull > 0 )
    {
      (void) fprintf (stderr, "BenchIT: bi_int_getenv(): env. variable %s not an int (%s)\n",
                      env, string);
      (void) fflush (stderr);
//      if ( exitOnNull > 1 ) bi_dumpTable();
      (void) safe_exit (127);
    }
    result = 0;
  }
  /* error or not */
  return result;
}


double get_list_element(int index) {
  int ii=0;
  list_t *current;

  if (listsize < index) {
    printf("list index out of bounds\n");
    return 0.0;
  }
  current=list;
  for(ii=1; ii<index; ii++) {
    current=current->pnext;
  }

  //printf("bi_get_list_element: list[%d] = %f\n", ii, current->dnumber);
  return current->dnumber;
}



double get_list_maxelement() {
  int ii = 0;
  list_t *current = NULL;
  double maximum = 0.0;

  if (listsize < 1) {
    printf("no items in list\n");
    return 0.0;
  }
  current = list;
  maximum = current->dnumber;
  for(ii=1; ii<listsize; ii++) {
    current=current->pnext;
    maximum = (maximum < current->dnumber) ? current->dnumber : maximum;
  }

  return maximum;
}



double get_list_minelement() {
  int ii = 0;
  list_t *current = NULL;
  double minimum = 0.0;

  if (listsize < 1) {
    printf("no items in list\n");
    return 0.0;
  }
  current = list;
  minimum = current->dnumber;
  for(ii=1; ii<listsize; ii++) {
    current=current->pnext;
    minimum = (minimum > current->dnumber) ? current->dnumber : minimum;
  }

  return minimum;
}



/*! @brief The function parses a list of numbers in a certain sysntax
 * and returns a chained list with the expanded numbers.
 *  @param[out] count holds the number of elements in the result-list
 *  @param[in] pcstring the string containing the intervalls
 *  @return expanded list of values which is count elements long
 */
void parselist(const char *pcstring) {
  /*pointer to the 1st element of the bi_list_t and
   return value if the function*/
  list_t *pfirst;
  list_t *panchor;
  /*variable for buffering and working*/
  list_t *pelement;
  /*loop variables, variables for memorising
   series of numbers and raise*/
  unsigned long long li, lj, ln, lstartnumber, lendnumber, lraise;
  int icount, negraise, ret;

  /*debugging level 1: mark begin and end of function*/
  if (DEBUG > 0) {
    printf("reached function parser\n");
    fflush(stdout);
  }

  /*initializing*/
  li = (unsigned long long) 0;
  lj = (unsigned long long) 0;
  ln = (unsigned long long) 0;
  lstartnumber = (unsigned long long) 0;
  lendnumber = (unsigned long long) 0;
  lraise = (unsigned long long) 0;
  icount = (int) 0;
  negraise = (int) 0;
  ret = (int) 0;
  pfirst = NULL;
  panchor = NULL;
  errno=0;
  /*as long as the strings end is not reached do ...*/
  while (pcstring[li] != 0) {
    /*if the beginning of a number is found ...*/
    if (isdigit(pcstring[li]) ) {
      /*save the number that was found*/
      ret = sscanf( (pcstring + li ), "%llu", &lstartnumber);
      if (errno != 0 || ret == EOF ) {
        perror("sscanf failed - wrong value type in PROBLEMLIST\n");
      }
      /*move ahead in the string until the end of the number ...*/
      ln = ( long ) (log10 ( ( double ) lstartnumber ) );
      li += ln + 1;
      /*whitespaces are ignored*/
      while (isspace(pcstring[li]) ){
        li++;
      }
      /*if next character is a minus
       -> series of numbers is defined*/
      if (pcstring[li] == '-') {
        li++;
        /*whitespaces are ignored*/
        if (isspace(pcstring[li]) )
          li++;
        /*if next number if found ...*/
        if (isdigit(pcstring[li]) ) {
          /*the number is used as
           the end of the series*/
          ret = sscanf( (pcstring + li ), "%llu", &lendnumber );
          if (errno != 0) {
            perror("sscanf failed - wrong value type in PROBLEMLIST\n");
          }
          /*move ahead in the string until the end of the number*/
          ln = ( long ) (log10 ( (double) (lendnumber ) ) );
          li += ln + 1;

          /*if there is nothing different defined
           all numbers between start and and are
           added to the list*/
          lraise = 1;
          /*whitespaces are ignored*/
          while (isspace(pcstring[li]) ) {
            li++;
          }
          /* check for attemps to use floats - bad thing */
          if (pcstring[li] == '.'){
            fprintf(stderr, "\nfloating point numbers in PROBLEMLIST not supported\n");
            safe_exit(1);
          }

          /*if next char is a slash
           -> raise must be changed to ...*/
          if (pcstring[li] == '/') {
            li++;
            /*whitespaces are ignored*/
            while (isspace(pcstring[li]) ) {
              li++;
            }
            /* check for neg number starting with "-" */
            negraise = (int) 0; /* reset to non-negative */
            if (pcstring[li] == '-') {
                li++;
                negraise=1;
            }
            /*... the following number ...*/
            if (isdigit(pcstring[li]) ) {
              ret = sscanf( (pcstring + li ), "%llu", &lraise );
              if (errno != 0) {
                perror("sscanf failed - wrong value type in PROBLEMLIST\n");
              }
              if ( lraise == 0) {
                fprintf(stderr, "\nstepsize zero won't work - reset to one\n");
                lraise = 1;
              }
            }
            /*and it needs to be moved ahead
             until the end of the number*/
            ln = ( long ) (log10 (( double ) (lraise )));
            li += ln + 1;
            /* check for attemps to use floats - bad thing */
            if (pcstring[li] == '.') {
              fprintf(stderr, "\nfloating point numbers in PROBLEMLIST not supported\n");
              safe_exit(1);
            }
          }

          /*create a new element ....*/
          pelement = ( list_t * ) malloc(sizeof(list_t));
          /* remember the first element */
          if(pfirst==NULL) pfirst = pelement;
          /* create anchor id nessessary */
          if(panchor==NULL)panchor = ( list_t * ) malloc(sizeof(list_t));
          panchor->pnext=pelement;
          panchor=pelement;
          pelement->dnumber = (double) lstartnumber;
          icount++;

          /* check sanity of found values */
          fflush(stdout);
          fflush(stderr);
          if ( (negraise == 0) && (lendnumber < (lstartnumber + lraise)) ) {
            fprintf(stderr, "\nwrong start-stop/stepsize values");
            fprintf(stderr, "\nyou will only do one measurement\n");
            lendnumber=lstartnumber;
            lraise = 1;
          }
          if ( (negraise == 1) && (lstartnumber < (lendnumber + lraise)) ) {
            fprintf(stderr, "\nwrong start-stop/stepsize values");
            fprintf(stderr, "\nyou will only do one measurement\n");
            lendnumber=lstartnumber;
            lraise = 1;
            negraise = 0;
          }
          fflush(stderr);
          fflush(stdout);

          /*now all desired elements between start
           and end are added to the list*/
          if (negraise == 0 ){
             for (lj = lstartnumber; lj <= lendnumber - lraise; lj
                  += lraise ) {
                /*allocate element*/
                pelement = ( list_t * ) malloc(sizeof(list_t));
                panchor->pnext=pelement;
                panchor=pelement;
                /*create an element with the number
                 (startnumber is already in the list!)*/
                pelement->dnumber = (double) (lj + lraise);
                /*and keep in mind that an element was inserted*/
                icount++;
             }
          }
          else {
             for (lj = lstartnumber; lj >= lendnumber + lraise; lj -= lraise) {
                /*allocate element*/
                pelement = ( list_t * ) malloc(sizeof(list_t));
                panchor->pnext=pelement;
                panchor=pelement;
                /*create an element with the number
                 (startnumber is already in the list!)*/
                pelement->dnumber = (double) (lj - lraise);
                /*and keep in mind that an element was inserted*/
                icount++;
             }
           }
         }
       }
       else {
         /* if the next char is a comma or end is reached */
         if (pcstring[li] == ',' || pcstring[li] == 0) {
           /*create a new element ....*/
           pelement = ( list_t * ) malloc(sizeof(list_t));
           /* remember the first element */
           if(pfirst==NULL) pfirst = pelement;
           /* create anchor id nessessary */
           if(panchor==NULL)panchor = ( list_t * ) malloc(sizeof(list_t));
           panchor->pnext=pelement;
           panchor=pelement;
           pelement->dnumber = (double) lstartnumber;
           icount++;
         }
         else {
           /* check for attemps to use floats - bad thing */
           if (pcstring[li] == '.') {
           fprintf(stderr, "\nfloating point numbers in PARAMETERS not supported\n");
           safe_exit(1);
      }

         }
       }
    }
    /*if no number is found -> go on in the string */
    else
      li++;
  }

  list=pfirst;
  listsize=icount;

  /*debugging level 1: mark begin and end of function */
  if (DEBUG > 0) {
    fflush(stdout); printf("parser created %d entries in list\n", icount );
    /*return the pointer that points to the start of the bi_list_t */
    printf("listsize=%d, value=%f\n", listsize, pelement->dnumber);
    printf("completed function parser\n");
    fflush(stdout);
  }
}


void safe_exit( int code )
{
  if (code == 0)
    MPI_Finalize();
  else
    MPI_Abort(MPI_COMM_WORLD, code);
  exit( code );
}

