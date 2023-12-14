//
// Created by li-xinjia on 23/10/15.
//

#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H

#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char is_alphabet(char c);
extern char is_number(char c);
extern char is_spaces(char c);
extern char is_symbol(char c);
extern int  is_keyword(char *str);
extern void read_all_comment();

extern int  init_scan(char *filename);
extern int  scan(void);
extern int  get_linenum(void);
extern void end_scan(void);

extern int scan_loop(void);

//extern void print_result();

#endif //SCANNER_SCANNER_H
