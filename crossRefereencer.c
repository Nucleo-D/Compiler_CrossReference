//
// Created by li-xinjia on 23/12/10.
//

#include "crossRefereencer.h"

struct NAME_BUFFER *name_buffer_root;

void init_name_buffer() {
    name_buffer_root = NULL;
}

void add_name(char *name) {
    struct NAME_BUFFER *new_name_buffer = (struct NAME_BUFFER *) malloc(sizeof(struct NAME_BUFFER));
    strcpy(new_name_buffer->name, name);
    new_name_buffer->next = name_buffer_root;
    name_buffer_root = new_name_buffer;
}

int get_name(char *name) {
    struct NAME_BUFFER *p;
    p = name_buffer_root;
    if (p == NULL) return 0;
    strcpy(name, p->name);
    name_buffer_root = p->next;
    free(p);
    return 1;
}

void clear_name_buffer() {
    struct NAME_BUFFER *p;
    while (name_buffer_root != NULL) {
        p = name_buffer_root;
        name_buffer_root = p->next;
        free(p);
    }
}