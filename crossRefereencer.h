//
// Created by li-xinjia on 23/12/10.
//

#ifndef PRETYPRINTER_CROSSREFEREENCER_H
#define PRETYPRINTER_CROSSREFEREENCER_H

#include "identifier.h"

extern struct NAME_BUFFER {
    char                name[MAXSTRSIZE];
    struct NAME_BUFFER *next;
} *name_buffer_root;

void init_name_buffer();
void add_name(char *name);
int  get_name(char *name);
void clear_name_buffer();

void output_cross_reference();

#endif //PRETYPRINTER_CROSSREFEREENCER_H
