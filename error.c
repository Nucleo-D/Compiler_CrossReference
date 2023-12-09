//
// Created by li-xinjia on 23/10/15.
//

#include "error.h"
#include "prettyPrint.h"

int error(char *mes) {
    fprintf(stderr, "\nIn line %d : \n ERROR: %s\n", get_linenum(), mes);
    release_idtab();
    end_scan();
    return ERROR;
}

void error_scan(char *mes) {
    fprintf(stderr, "\nIn line %d : \n ERROR: %s\n", get_linenum(), mes);
    release_idtab();
    end_scan();
    exit(EXIT_FAILURE);
}