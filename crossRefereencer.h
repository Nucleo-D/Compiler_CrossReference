//
// Created by li-xinjia on 23/12/10.
//

#ifndef PRETYPRINTER_CROSSREFEREENCER_H
#define PRETYPRINTER_CROSSREFEREENCER_H

#include "error.h"
#include "identifier.h"

extern struct NAME_BUFFER {
    char                name[MAXSTRSIZE];
    struct NAME_BUFFER *next;
} *name_buffer_root;

extern int  cross_token;
extern int  cross_can_break;

extern int  in_procedure;
extern int  in_array;
extern int  in_procedure_parameter;
extern int  in_variable_declaration;
extern int  bool_expression_flag;
extern int  integer_expression_flag;
extern int  char_expression_flag;
extern int  in_call_statement;
extern char current_call_procedure_name[MAXSTRSIZE];
extern char current_procedure_name[MAXSTRSIZE];
extern int  current_array_size;


void init_name_buffer();
void add_name(char *name);
int  get_name(char *name);
void clear_name_buffer();

int parse_program_cross();
int parse_block_cross();
int parse_variable_declaration_cross();
int parse_variable_names_cross();
int parse_variable_name_cross();
int parse_type_cross();
int parse_standard_type_cross();
int parse_array_type_cross();
int parse_subprogram_declaration_cross();
int parse_procedure_name_cross();
int parse_formal_parameters_cross();
int parse_compound_statement_cross();
int parse_statement_cross();
int parse_condition_statement_cross();
int parse_iteration_statement_cross();
int parse_exit_statement_cross();
int parse_call_statement_cross();
int parse_expressions_cross();
int parse_return_statement_cross();
int parse_assignment_statement_cross();
int parse_left_part_cross();
int parse_variable_cross();
int parse_expression_cross();
int parse_simple_expression_cross();
int parse_term_cross();
int parse_factor_cross();
int parse_constant_cross();
int parse_multiplicative_operator_cross();
int parse_additive_operator_cross();
int parse_relational_operator_cross();
int parse_input_statement_cross();
int parse_output_statement_cross();
int parse_output_format_cross();

void init_cross_reference();
void output_cross_reference();

#endif //PRETYPRINTER_CROSSREFEREENCER_H
