#include <stdio.h>

#include "scanner.h"
#include "prettyPrint.h"

int main(int nc, char *np[]) {
    int token, i;

    if (nc < 2) {
        fprintf(stderr, "File name id not given.\n");
        return 0;
    }
    if (init_scan(np[1]) < 0) {
        fprintf(stderr, "File %s can not open.\n", np[1]);
        return 0;
    }

    init_pretty_print();
//    if(parse_program() == NORMAL){
//        pretty_print();
//    }


    return 0;
}