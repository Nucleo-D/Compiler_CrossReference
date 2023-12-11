#include "identifier.h"
#include "error.h"

//struct ID *idroot;
/* Pointers to root of global & local symbol tables */
struct ID *globalidroot, *localidroot;

struct ID *sortedidroot;

void init_idtab() { /* Initialise the table */
                    //    idroot = NULL;
    globalidroot = NULL;
    localidroot = NULL;
}

//struct ID *search_idtab(char *np) { /* search the name pointed by np */
//    struct ID *p;
//
//    for (p = idroot; p != NULL; p = p->nextp) {
//        if (strcmp(np, p->name) == 0) return (p);
//    }
//    return (NULL);
//}

//void id_countup(char *np) { /* Register and count up the name pointed by np */
//    struct ID *p;
//    char      *cp;
//
//    if ((p = search_idtab(np)) != NULL) p->count++;
//    else {
//        if ((p = (struct ID *) malloc(sizeof(struct ID))) == NULL) {
//            printf("can not malloc in id_countup\n");
//            return;
//        }
//        if ((cp = (char *) malloc(strlen(np) + 1)) == NULL) {
//            printf("can not malloc-2 in id_countup\n");
//            return;
//        }
//        strcpy(cp, np);
//        p->name = cp;
//        p->count = 1;
//        p->nextp = idroot;
//        idroot = p;
//    }
//}

//void print_idtab() { /* Output the registered data */
//    struct ID *p;
//
//    for (p = idroot; p != NULL; p = p->nextp) {
//        if (p->count != 0)
//            printf("\t\"Identifier\" \"%s\"\t%d\n", p->name, p->count);
//    }
//}

struct ID *search_globalidtab(char *np) {
    struct ID *p;

    for (p = globalidroot; p != NULL; p = p->nextp) {
        if (strcmp(np, p->name) == 0) {
            add_reflinenum(np, get_linenum(), NULL);
            return (p);
        }
    }
    return (NULL);
}

struct ID *search_localidtab(char *np, char *procname) {
    struct ID *p;

    for (p = localidroot; p != NULL; p = p->nextp) {
        if (strcmp(np, p->name) == 0 && strcmp(procname, p->procname) == 0) {
            add_reflinenum(np, get_linenum(), procname);
            return (p);
        }
    }
    return (NULL);
}

void add_idtab(char *np, int ispara, int deflinenum, char *procname) {
    struct ID *p;

    if ((p = (struct ID *) malloc(sizeof(struct ID))) == NULL) {
        error("can not malloc struct in add_idtab\n");
        return;
    }
    if ((p->name = (char *) malloc(strlen(np) + 1)) == NULL) {
        error("can not malloc name in add_idtab\n");
        return;
    }
    strcpy(p->name, np);
    if (p->procname != NULL) {
        if ((p->procname = (char *) malloc(strlen(procname) + 1)) == NULL) {
            error("can not malloc procname in add_idtab\n");
            return;
        }
        strcpy(p->procname, procname);
    }

    p->itp = NULL;
    p->ispara = ispara;
    p->deflinenum = deflinenum;
    p->irefp = NULL;
    p->nextp = globalidroot;
    globalidroot = p;
}

void add_reflinenum(char *np, int linenum, char *procname) {
    struct ID   *p;
    struct LINE *q;

    if (procname == NULL && (p = search_globalidtab(np)) != NULL) {
        if ((q = (struct LINE *) malloc(sizeof(struct LINE))) == NULL) {
            error("can not malloc struct in add_reflinenum\n");
            return;
        }
        q->reflinenum = linenum;
        q->nextlinep = p->irefp;
        p->irefp = q;
    } else if (procname != NULL && (p = search_localidtab(np, procname)) != NULL) {
        if ((q = (struct LINE *) malloc(sizeof(struct LINE))) == NULL) {
            error("can not malloc struct in add_reflinenum\n");
            return;
        }
        q->reflinenum = linenum;
        q->nextlinep = p->irefp;
        p->irefp = q;
    } else {
        error("can not find id in add_reflinenum\n");
        return;
    }
}

void add_standard_type(char *np, int type, char *procname, int ispara) {
    struct ID *p;

    if (procname == NULL && (p = search_globalidtab(np)) != NULL) {
        if (p->itp != NULL) {
            error("type is already defined in add_standard_type\n");
            return;
        }
        if ((p->itp = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
            error("can not malloc struct in add_standard_type\n");
            return;
        }
        p->itp->ttype = type;
        p->itp->arraysize = 0;
        p->itp->etp = NULL;
        p->itp->paratp = NULL;
        p->ispara = ispara;
    } else if (procname != NULL && (p = search_localidtab(np, procname)) != NULL) {
        if (p->itp != NULL) {
            error("type is already defined in add_standard_type\n");
            return;
        }
        if ((p->itp = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
            error("can not malloc struct in add_standard_type\n");
            return;
        }
        p->itp->ttype = type;
        p->itp->arraysize = 0;
        p->itp->etp = NULL;
        p->itp->paratp = NULL;
        p->ispara = ispara;
    } else {
        error("can not find id in add_standard_type\n");
        return;
    }
}

void add_array_type(char *np, int type, int arraysize, char *procname) {
    struct ID *p;

    if ((p = search_globalidtab(np)) != NULL) {
        if (p->itp != NULL) {
            error("type is already defined in add_array_type\n");
            return;
        }
        if ((p->itp = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
            error("can not malloc struct in add_array_type\n");
            return;
        }
        p->itp->ttype = type;
        p->itp->arraysize = arraysize;
        p->itp->etp = NULL;
        p->itp->paratp = NULL;
        p->ispara = 0;
    } else if ((p = search_localidtab(np, procname)) != NULL) {
        if (p->itp != NULL) {
            error("type is already defined in add_array_type\n");
            return;
        }
        if ((p->itp = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
            error("can not malloc struct in add_array_type\n");
            return;
        }
        p->itp->ttype = type;
        p->itp->arraysize = arraysize;
        p->itp->etp = NULL;
        p->itp->paratp = NULL;
        p->ispara = 0;
    } else {
        error("can not find id in add_array_type\n");
        return;
    }
}

void add_procedure_type(char *np) {
    struct ID *p;

    if ((p = search_globalidtab(np)) != NULL) {
        if (p->itp != NULL) {
            error("type is already defined in add_procedure_type\n");
            return;
        }
        if ((p->itp = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
            error("can not malloc struct in add_procedure_type\n");
            return;
        }
        p->itp->ttype = TPPROC;
        p->itp->arraysize = 0;
        p->itp->etp = NULL;
        p->itp->paratp = NULL;
        p->ispara = 0;
    } else {
        error("can not find id in add_procedure_type\n");
        return;
    }
}

void add_proceduure_parameter(char *np, int type, char *procname) {
    struct ID   *pproc;
    struct ID   *p;
    struct TYPE *q;

    if ((pproc = search_globalidtab(procname)) != NULL) {
        if (pproc->itp == NULL) {
            error("type is not defined in add_proceduure_standard_type_parameter\n");
            return;
        }
        if (pproc->itp->ttype != TPPROC) {
            error("type is not procedure in add_proceduure_standard_type_parameter\n");
            return;
        }
        if ((p = (struct ID *) malloc(sizeof(struct ID))) == NULL) {
            error("can not malloc struct in add_proceduure_standard_type_parameter\n");
            return;
        }
        if ((q = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
            error("can not malloc struct in add_proceduure_standard_type_parameter\n");
            return;
        }

        q->ttype = type;
        q->arraysize = 0;
        q->etp = NULL;
        q->paratp = NULL;

        if (pproc->itp->paratp == NULL) {
            pproc->itp->paratp = q;
        } else {
            struct TYPE *r;
            for (r = pproc->itp->paratp; r->paratp != NULL; r = r->paratp)
                ;
            r->paratp = q;
        }

        add_standard_type(np, type, procname, 1);
    } else {
        error("can not find id in add_proceduure_standard_type_parameter\n");
        return;
    }
}

void release_idtab() { /* Release tha data structure */
    struct ID *p, *q;

    //    for (p = idroot; p != NULL; p = q) {
    //        free(p->name);
    //        q = p->nextp;
    //        free(p);
    //    }
    for (p = globalidroot; p != NULL; p = q) {
        free(p->name);
        q = p->nextp;
        free(p);
    }
    for (p = localidroot; p != NULL; p = q) {
        free(p->name);
        q = p->nextp;
        free(p);
    }
    init_idtab();
}

struct ID *sort_by_name() {
    struct ID *p, *q, *r, *s;
    sortedidroot = NULL;
    p = globalidroot;
    q = localidroot;

    if (p != NULL && q != NULL) {
        if (strcmp(p->name, q->name) < 0) {
            sortedidroot = p;
            p = p->nextp;
        } else {
            sortedidroot = q;
            q = q->nextp;
        }
    } else if (p != NULL) {
        sortedidroot = p;
        p = p->nextp;
    } else if (q != NULL) {
        sortedidroot = q;
        q = q->nextp;
    } else {
        return sortedidroot;
    }

    while (p != NULL && q != NULL) {
        if (strcmp(p->name, q->name) < 0) {
            r = p;
            p = p->nextp;
        } else if (strcmp(p->name, q->name) > 0){
            r = q;
            q = q->nextp;
        }else{
            if(strcmp(p->procname, q->procname) < 0) {
                r = p;
                p = p->nextp;
            }else{
                r = q;
                q = q->nextp;
            }
        }
        s = sortedidroot;
        if (strcmp(r->name, s->name) < 0) {
            r->nextp = s;
            sortedidroot = r;
        } else if(strcmp(r->name, s->name) == 0){
            if(strcmp(r->procname, s->procname) < 0) {
                r->nextp = s;
                sortedidroot = r;
            }
        }
        while (s->nextp != NULL) {
            if (strcmp(r->name, s->nextp->name) < 0) {
                r->nextp = s->nextp;
                s->nextp = r;
                break;
            } else if(strcmp(r->name, s->nextp->name) == 0){
                if(strcmp(r->procname, s->nextp->procname) < 0) {
                    r->nextp = s->nextp;
                    s->nextp = r;
                    break;
                }
            }
        }
    }

    while (p != NULL){
        r = p;
        if (strcmp(r->name, s->name) < 0) {
            r->nextp = s;
            sortedidroot = r;
        } else if(strcmp(r->name, s->name) == 0){
            if(strcmp(r->procname, s->procname) < 0) {
                r->nextp = s;
                sortedidroot = r;
            }
        }
        while (s->nextp != NULL) {
            if (strcmp(r->name, s->nextp->name) < 0) {
                r->nextp = s->nextp;
                s->nextp = r;
                break;
            } else if(strcmp(r->name, s->nextp->name) == 0){
                if(strcmp(r->procname, s->nextp->procname) < 0) {
                    r->nextp = s->nextp;
                    s->nextp = r;
                    break;
                }
            }
        }
        p = p->nextp;
    }

    while (q != NULL){
        r = q;
        if (strcmp(r->name, s->name) < 0) {
            r->nextp = s;
            sortedidroot = r;
        } else if(strcmp(r->name, s->name) == 0){
            if(strcmp(r->procname, s->procname) < 0) {
                r->nextp = s;
                sortedidroot = r;
            }
        }
        while (s->nextp != NULL) {
            if (strcmp(r->name, s->nextp->name) < 0) {
                r->nextp = s->nextp;
                s->nextp = r;
                break;
            } else if(strcmp(r->name, s->nextp->name) == 0){
                if(strcmp(r->procname, s->nextp->procname) < 0) {
                    r->nextp = s->nextp;
                    s->nextp = r;
                    break;
                }
            }
        }
        q = q->nextp;
    }

    return sortedidroot;
}
