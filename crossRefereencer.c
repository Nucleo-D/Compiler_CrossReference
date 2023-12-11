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

void output_cross_reference() {
    struct ID   *p;
    struct LINE *q;
    struct TYPE *r;
    char         name[MAXSTRSIZE];
    char         para[MAXSTRSIZE];
    char         line[MAXSTRSIZE];
    char         number[10];

    FILE *original_stdout = stdout;
    freopen("/dev/null", "w", stdout);
    fflush(stdout);
    fclose(stdout);
    stdout = original_stdout;

    printf("Name\t|\tType\t|\tDefine\t|\tReferences");

    p = sort_by_name();
    while (p != NULL) {
        printf("\n%s\t|\t", p->name);
        if (p->itp->ttype == TPPROC) {
            r = p->itp->paratp->paratp;
            strcpy(para, "");
            switch (r->ttype) {
                case TPINT:
                    sprintf(para, "%s", "int");
                    break;
                case TPCHAR:
                    sprintf(para, "%s", "char");
                    break;
                case TPBOOL:
                    sprintf(para, "%s", "bool");
                    break;
                default:
                    break;
            }
            r = r->paratp;
            while (r != NULL) {
                switch (r->ttype) {
                    case TPINT:
                        strcat(para, ", int");
                        break;
                    case TPCHAR:
                        strcat(para, ", char");
                        break;
                    case TPBOOL:
                        strcat(para, ", bool");
                        break;
                    default:
                        break;
                }
                r = r->paratp;
            }
        }
        switch (p->itp->ttype) {
            case TPINT:
                printf("int\t|\t");
                break;
            case TPCHAR:
                printf("char\t|\t");
                break;
            case TPBOOL:
                printf("bool\t|\t");
                break;
            case TPARRAYINT:
                printf("array[%d] of integer\t|\t", p->itp->arraysize);
                break;
            case TPARRAYCHAR:
                printf("array[%d] of char\t|\t", p->itp->arraysize);
                break;
            case TPARRAYBOOL:
                printf("array[%d] of boolean\t|\t", p->itp->arraysize);
                break;
            case TPPROC:
                printf("procedure(%s)\t|\t", para);
                break;
            default:
                printf("unknown\t|\t");
                break;
        }
        printf("%d\t|\t", p->deflinenum);
        q = p->irefp;

        strcpy(line, "");
        strcpy(number, "");
        if (q != NULL) {
            if (get_name(name)) {
                sprintf(line, "%d", q->reflinenum);
            }
            q = q->nextlinep;
        }

        while (q != NULL) {
            if (get_name(name)) {
                sprintf(number, ",%d", q->reflinenum);
                strcat(line, number);
            }
            q = q->nextlinep;
        }
        p = p->nextp;
    }
}