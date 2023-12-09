//
// Created by li-xinjia on 23/11/20.
//

#ifndef SCANNER_PRETTYPRINT_H
#define SCANNER_PRETTYPRINT_H

#include <stdarg.h>

#include "error.h"
#include "identifier.h"
#include "scanner.h"
#include "token.h"

#define NORMAL 0
#define ERROR  1

extern int token;
extern char buffer[4096];
extern int tab_num;
extern int can_break;

extern int in_procedure;
extern char current_procedure_name[MAXSTRSIZE];
extern int current_type;
extern int current_array_size;

void init_pretty_print();
void pretty_print();
void printf_with_tab(char *str, ...);
int scan_loop(void);

int parse_program();
int parse_block();
int parse_variable_declaration();
int parse_variable_names();
int parse_variable_name();
int parse_type();
int parse_standard_type();
int parse_array_type();
int parse_subprogram_declaration();
int parse_procedure_name();
int parse_formal_parameters();
int parse_compound_statement();
int parse_statement();
int parse_condition_statement();
int parse_iteration_statement();
int parse_exit_statement();
int parse_call_statement();
int parse_expressions();
int parse_return_statement();
int parse_assignment_statement();
int parse_left_part();
int parse_variable();
int parse_expression();
int parse_simple_expression();
int parse_term();
int parse_factor();
int parse_constant();
int parse_multiplicative_operator();
int parse_additive_operator();
int parse_relational_operator();
int parse_input_statement();
int parse_output_statement();
int parse_output_format();

#endif //SCANNER_PRETTYPRINT_H
