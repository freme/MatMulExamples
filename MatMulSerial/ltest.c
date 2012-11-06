#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "matmul.h"

int main(){
    char *p = 0;
    int ii = 0, problemsize = 0;

    /* get and parse the list of matrix sizes */
    p = sec_getenv("PROBLEMLIST", 1);
    parselist(p);

    for (ii=0;ii<listsize;ii++){
        problemsize = (int)round(get_list_element(ii));
        printf("%d\n", problemsize);
    }

    return 0;
}

