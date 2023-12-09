//
// Created by li-xinjia on 23/10/21.
//

#ifndef SCANNER_IDENTIFIER_H
#define SCANNER_IDENTIFIER_H

#include "token.h"

struct ID {
    char      *name;
    int        count;
    struct ID *nextp;
};

struct ID *search_idtab(char *np);
void       init_idtab();
void       id_countup(char *np);
void       print_idtab();
void       release_idtab();


#endif //SCANNER_IDENTIFIER_H
