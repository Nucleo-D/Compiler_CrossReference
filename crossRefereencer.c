//
// Created by li-xinjia on 23/12/10.
//

#include "crossRefereencer.h"

struct NAME_BUFFER *name_buffer_root;

int cross_token;
int cross_can_break = 0;

int  in_procedure = 0;
int  in_array = 0;
int  in_procedure_parameter = 0;
int  in_variable_declaration = 0;
int  bool_expression_flag = 0;
int  integer_expression_flag = 0;
int  char_expression_flag = 0;
int  in_call_statement = 0;
char current_call_procedure_name[MAXSTRSIZE];
char current_procedure_name[MAXSTRSIZE];
int  current_array_size = 0;

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

void init_cross_reference() {
    init_idtab();
    init_name_buffer();
    cross_token = scan_loop();
}

void output_cross_reference() {
    struct ID   *p;
    struct LINE *q;
    struct TYPE *r;
    char         para[MAXSTRSIZE];
    char         line[MAXSTRSIZE];
    char         number[10];

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
            sprintf(line, "%d", q->reflinenum);
            q = q->nextlinep;
        }

        while (q != NULL) {
            sprintf(number, ",%d", q->reflinenum);
            strcat(line, number);
            q = q->nextlinep;
        }
        printf("%s", line);
        p = p->nextp;
    }
}

int parse_program_cross() {
    if (cross_token != TPROGRAM) return (error("Keyword ’program’ is not found"));
    cross_token = scan_loop();
    if (cross_token != TNAME) return (error("Program name is not found"));
    cross_token = scan_loop();
    if (cross_token != TSEMI) return (error("Semicolon is not found"));
    cross_token = scan_loop();
    if (parse_block_cross() == ERROR) return (ERROR);
    if (cross_token != TDOT) return (error("Period is not found at the end of program"));
    cross_token = scan_loop();
    return (NORMAL);
}

int parse_block_cross() {
    while (cross_token == TVAR || cross_token == TPROCEDURE) {
        switch (cross_token) {
            case TVAR:
                if (parse_variable_declaration_cross() == ERROR) return (ERROR);
                break;
            case TPROCEDURE:
                if (parse_subprogram_declaration_cross() == ERROR) return (ERROR);
                break;
            default:
                return (error("Block is not found"));
        }
    }
    if (parse_compound_statement_cross() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_variable_declaration_cross() {
    struct ID *p;
    in_variable_declaration = 1;
    if (cross_token != TVAR) return (error("Var is not found"));
    cross_token = scan_loop();
    if (parse_variable_names_cross() == ERROR) return (ERROR);
    if (cross_token != TCOLON) return (error("Colon is not found"));
    cross_token = scan_loop();
    if (parse_type_cross() == ERROR) return (ERROR);
    if (cross_token != TSEMI) return (error("Semicolon is not found"));
    cross_token = scan_loop();
    while (cross_token == TNAME) {
        if (parse_variable_names_cross() == ERROR) return (ERROR);
        if (cross_token != TCOLON) return (error("Colon is not found"));
        cross_token = scan_loop();
        if (parse_type_cross() == ERROR) return (ERROR);
        if (cross_token != TSEMI) return (error("Semicolon is not found"));
        cross_token = scan_loop();
    }
    in_variable_declaration = 0;
    return NORMAL;
}

int parse_variable_names_cross() {
    if (in_procedure == 1) {
        if (search_localidtab(name_attr, current_procedure_name) != NULL) return (error("Variable name is already defined"));
    } else {
        if (search_globalidtab(name_attr) != NULL) return (error("Variable name is already defined"));
    }
    if (parse_variable_name_cross() == ERROR) return (ERROR);
    while (cross_token == TCOMMA) {
        if (cross_token != TCOMMA) return (error("Comma is not found"));
        cross_token = scan_loop();
        if (in_procedure == 1) {
            if (search_localidtab(name_attr, current_procedure_name) != NULL) return (error("Variable name is already defined"));
        } else {
            if (search_globalidtab(name_attr) != NULL) return (error("Variable name is already defined"));
        }
        if (parse_variable_name_cross() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_variable_name_cross() {
    if (cross_token != TNAME) return (error("Variable name is not found"));
    add_name(name_attr);
    struct ID *p;
    if (in_variable_declaration == 1 || in_procedure_parameter == 1) {
        if (in_procedure == 1) {
            add_idtab(name_attr, 0, get_linenum(), current_procedure_name);
        } else {
            add_idtab(name_attr, 0, get_linenum(), NULL);
        }
    } else {
        if (in_procedure == 1) {
            if ((p = search_localidtab(name_attr, current_procedure_name)) == NULL) {
                if ((p = search_globalidtab(name_attr)) == NULL) return (error("Variable name is not defined"));
            }
        } else {
            if ((p = search_globalidtab(name_attr)) == NULL) return (error("Variable name is not defined"));
        }
        switch (p->itp->ttype) {
            case TPARRAYINT:
            case TPINT:
                integer_expression_flag = 1;
                bool_expression_flag = 0;
                char_expression_flag = 0;
                break;
            case TPCHAR:
            case TPARRAYCHAR:
                integer_expression_flag = 0;
                bool_expression_flag = 0;
                char_expression_flag = 1;
                break;
            case TPBOOL:
            case TPARRAYBOOL:
                integer_expression_flag = 0;
                bool_expression_flag = 1;
                char_expression_flag = 0;
                break;
            case TPPROC:
                return (error("Wrong type of variable"));
            default:
                break;
        }
    }
    cross_token = scan_loop();
    return NORMAL;
}

int parse_type_cross() {
    switch (cross_token) {
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if (parse_standard_type_cross() == ERROR) return (ERROR);
            current_array_size = 1;
            break;
        case TARRAY:
            in_array = 1;
            if (parse_array_type_cross() == ERROR) return (ERROR);
            in_array = 0;
            break;
        default:
            return (error("Type is not found"));
    }
    return NORMAL;
}

int parse_standard_type_cross() {
    char nameBuffer[MAXSTRSIZE];
    if (cross_token != TINTEGER && cross_token != TBOOLEAN && cross_token != TCHAR) return (error("Standard type is not found"));
    if (in_array == 1) {
        while (get_name(nameBuffer) != 0) {
            if (cross_token == TINTEGER) {
                add_array_type(nameBuffer, TPARRAYINT, current_array_size, current_procedure_name);
            } else if (cross_token == TBOOLEAN) {
                add_array_type(nameBuffer, TPARRAYBOOL, current_array_size, current_procedure_name);
            } else if (cross_token == TCHAR) {
                add_array_type(nameBuffer, TPARRAYCHAR, current_array_size, current_procedure_name);
            }
        }
    } else if (in_procedure_parameter == 1) {
        while (get_name(nameBuffer) != 0) {
            if (cross_token == TINTEGER) {
                add_proceduure_parameter(nameBuffer, TPINT, current_procedure_name);
            } else if (cross_token == TBOOLEAN) {
                add_proceduure_parameter(nameBuffer, TPBOOL, current_procedure_name);
            } else if (cross_token == TCHAR) {
                add_proceduure_parameter(nameBuffer, TPCHAR, current_procedure_name);
            }
        }
    } else if (in_procedure == 1) {
        while (get_name(nameBuffer) != 0) {
            if (cross_token == TINTEGER) {
                add_standard_type(nameBuffer, TPINT, current_procedure_name, 0);
            } else if (cross_token == TBOOLEAN) {
                add_standard_type(nameBuffer, TPBOOL, current_procedure_name, 0);
            } else if (cross_token == TCHAR) {
                add_standard_type(nameBuffer, TPCHAR, current_procedure_name, 0);
            }
        }
    } else {
        while (get_name(nameBuffer) != 0) {
            if (cross_token == TINTEGER) {
                add_standard_type(nameBuffer, TPINT, NULL, 0);
            } else if (cross_token == TBOOLEAN) {
                add_standard_type(nameBuffer, TPBOOL, NULL, 0);
            } else if (cross_token == TCHAR) {
                add_standard_type(nameBuffer, TPCHAR, NULL, 0);
            }
        }
    }
    cross_token = scan_loop();
    return NORMAL;
}

int parse_array_type_cross() {
    if (cross_token != TARRAY) return (error("Array is not found"));
    cross_token = scan_loop();
    if (cross_token != TLSQPAREN) return (error("Left square parenthesis is not found"));
    cross_token = scan_loop();
    current_array_size = num_attr;
    if (num_attr <= 0) return (error("Array size must be greater than 0"));
    if (cross_token != TNUMBER) return (error("Number is not found"));
    cross_token = scan_loop();
    if (cross_token != TRSQPAREN) return (error("Right square parenthesis is not found"));
    cross_token = scan_loop();
    if (cross_token != TOF) return (error("Of is not found"));
    cross_token = scan_loop();
    if (parse_standard_type_cross() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_subprogram_declaration_cross() {
    if (cross_token != TPROCEDURE) return (error("Procedure is not found"));
    in_procedure = 1;
    cross_token = scan_loop();
    strcpy(current_procedure_name, name_attr);
    if (parse_procedure_name_cross() == ERROR) return (ERROR);
    if (cross_token == TLPAREN) {
        if (parse_formal_parameters_cross() == ERROR) return (ERROR);
    }
    if (cross_token != TSEMI) return (error("Semicolon is not found"));
    cross_token = scan_loop();
    if (cross_token == TVAR) {
        if (parse_variable_declaration_cross() == ERROR) return (ERROR);
    }
    if (parse_compound_statement_cross() == ERROR) return (ERROR);
    if (cross_token != TSEMI) return (error("Semicolon is not found"));
    cross_token = scan_loop();
    in_procedure = 0;
    return NORMAL;
}

int parse_procedure_name_cross() {
    if (in_call_statement == 1) {
        search_globalidtab(name_attr);
    } else {
        add_idtab(name_attr, 0, get_linenum(), NULL);
        add_procedure_type(name_attr);
    }
    if (cross_token != TNAME) return (error("Procedure name is not found"));
    cross_token = scan_loop();
    return NORMAL;
}

int parse_formal_parameters_cross() {
    in_procedure_parameter = 1;
    if (cross_token != TLPAREN) return (error("Left parenthesis is not found"));
    cross_token = scan_loop();
    if (parse_variable_names_cross() == ERROR) return (ERROR);
    if (cross_token != TCOLON) return (error("Colon is not found"));
    cross_token = scan_loop();
    if (parse_type_cross() == ERROR) return (ERROR);
    while (cross_token == TSEMI) {
        cross_token = scan_loop();
        if (parse_variable_names_cross() == ERROR) return (ERROR);
        if (cross_token != TCOLON) return (error("Colon is not found"));
        cross_token = scan_loop();
        if (parse_type_cross() == ERROR) return (ERROR);
    }
    if (cross_token != TRPAREN) return (error("Right parenthesis is not found"));
    cross_token = scan_loop();
    in_procedure_parameter = 0;
    return NORMAL;
}

int parse_compound_statement_cross() {
    if (cross_token != TBEGIN) return (error("Begin is not found"));
    cross_token = scan_loop();
    while (cross_token != TEND && cross_token != TDOT) {
        if (parse_statement_cross() == ERROR) return (ERROR);
        if (cross_token == TEND) {
            break;
        }
        if (cross_token != TSEMI) return (error("Semicolon is not found"));
        cross_token = scan_loop();
    }
    if (cross_token != TEND) return (error("End is not found"));
    cross_token = scan_loop();
    return NORMAL;
}

int parse_statement_cross() {
    switch (cross_token) {
        case TNAME:
            if (parse_assignment_statement_cross() == ERROR) return (ERROR);
            break;
        case TIF:
            if (parse_condition_statement_cross() == ERROR) return (ERROR);
            break;
        case TWHILE:
            if (parse_iteration_statement_cross() == ERROR) return (ERROR);
            break;
        case TBREAK:
            if (parse_exit_statement_cross() == ERROR) return (ERROR);
            break;
        case TCALL:
            if (parse_call_statement_cross() == ERROR) return (ERROR);
            break;
        case TRETURN:
            if (parse_return_statement_cross() == ERROR) return (ERROR);
            break;
        case TBEGIN:
            if (parse_compound_statement_cross() == ERROR) return (ERROR);
            break;
        case TREAD:
        case TREADLN:
            if (parse_input_statement_cross() == ERROR) return (ERROR);
            break;
        case TWRITE:
        case TWRITELN:
            if (parse_output_statement_cross() == ERROR) return (ERROR);
            break;
        default:
    }
    return NORMAL;
}

int parse_condition_statement_cross() {
    if (cross_token != TIF) return (error("If is not found"));
    cross_token = scan_loop();
    if (parse_expression_cross() == ERROR) return (ERROR);
    if (bool_expression_flag == 0) return (error("Expression must be boolean type"));
    bool_expression_flag = 0;
    if (cross_token != TTHEN) return (error("Then is not found"));
    cross_token = scan_loop();
    if (parse_statement_cross() == ERROR) return (ERROR);
    if (cross_token == TELSE) {
        if (cross_token != TELSE) return (error("Else is not found"));
        cross_token = scan_loop();
        if (parse_statement_cross() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_iteration_statement_cross() {
    if (cross_token != TWHILE) return (error("While is not found"));
    cross_token = scan_loop();
    if (parse_expression_cross() == ERROR) return (ERROR);
    if (bool_expression_flag == 0) return (error("Expression must be boolean type"));
    bool_expression_flag = 0;
    if (cross_token != TDO) return (error("Do is not found"));
    cross_can_break++;
    cross_token = scan_loop();
    if (parse_statement_cross() == ERROR) return (ERROR);
    cross_can_break--;
    return NORMAL;
}

int parse_exit_statement_cross() {
    if (cross_token != TBREAK) return (error("Break is not found"));
    cross_token = scan_loop();
    char error_message[100] = "";
    sprintf(error_message, "'break' must be in a while loop. (current token: '%s')", tokenstr[cross_token]);
    if (cross_can_break == 0) return (error(error_message));
    return NORMAL;
}

int parse_call_statement_cross() {
    in_call_statement = 1;
    if (cross_token != TCALL) return (error("Call is not found"));
    cross_token = scan_loop();
    strcpy(current_call_procedure_name, name_attr);
    if (parse_procedure_name_cross() == ERROR) return (ERROR);
    if (cross_token == TLPAREN) {
        cross_token = scan_loop();
        if (parse_expressions_cross() == ERROR) return (ERROR);
        if (cross_token != TRPAREN) return (error("Right parenthesis is not found"));
        cross_token = scan_loop();
    }
    in_call_statement = 0;
    return NORMAL;
}

int parse_expressions_cross() {
    struct ID *p = search_globalidtab(current_call_procedure_name);
    if (p == NULL) return (error("Procedure name is not defined"));
    struct TYPE *q = p->itp;
    if (q->ttype != TPPROC) return (error("Procedure name is not defined"));
    if (q->paratp == NULL) return NORMAL;
    q = q->paratp;
    while (q != NULL) {
        if (parse_expression_cross() == ERROR) return (ERROR);
        if (bool_expression_flag == 1) {
            if (q->ttype != TPBOOL) return (error("Expression must be boolean type"));
        }
        if (integer_expression_flag == 1) {
            if (q->ttype != TPINT) return (error("Expression must be integer type"));
        }
        if (char_expression_flag == 1) {
            if (q->ttype != TPCHAR) return (error("Expression must be char type"));
        }
        if (q->paratp != NULL) {
            if (cross_token != TCOMMA) return (error("Comma is not found"));
            cross_token = scan_loop();
        }
        q = q->paratp;
    }
    return NORMAL;
}

int parse_return_statement_cross() {
    if (cross_token != TRETURN) return (error("Return is not found"));
    cross_token = scan_loop();
    return NORMAL;
}

int parse_assignment_statement_cross() {
    if (parse_left_part_cross() == ERROR) return (ERROR);
    if (cross_token != TASSIGN) return (error("Assign is not found"));
    cross_token = scan_loop();
    if (parse_expression_cross() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_left_part_cross() {
    //    if (in_procedure == 1) {
    //        if (search_localidtab(name_attr, current_procedure_name) == NULL) {
    //            if (search_globalidtab(name_attr) == NULL) return (error("Variable name is not defined"));
    //        }
    //    } else {
    //        if (search_globalidtab(name_attr) == NULL) return (error("Variable name is not defined"));
    //    }
    if (parse_variable_cross() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_variable_cross() {
    if (parse_variable_name_cross() == ERROR) return (ERROR);
    while (cross_token == TLSQPAREN) {
        if (cross_token != TLSQPAREN) return (error("Left square parenthesis is not found"));
        cross_token = scan_loop();
        if (parse_expression_cross() == ERROR) return (ERROR);
        if (integer_expression_flag == 0) return (error("Expression must be integer type"));
        integer_expression_flag = 0;
        if (cross_token != TRSQPAREN) return (error("Right square parenthesis is not found"));
        cross_token = scan_loop();
    }
    return NORMAL;
}

int parse_expression_cross() {
    if (parse_simple_expression_cross() == ERROR) return (ERROR);
    if (cross_token == TEQUAL || cross_token == TNOTEQ || cross_token == TLE || cross_token == TLEEQ || cross_token == TGR || cross_token == TGREQ) {
        if (parse_relational_operator_cross() == ERROR) return (ERROR);
        if (parse_simple_expression_cross() == ERROR) return (ERROR);
        integer_expression_flag = 0;
        bool_expression_flag = 1;
        char_expression_flag = 0;
    } else {
        integer_expression_flag = 1;
        bool_expression_flag = 0;
        char_expression_flag = 0;
    }
    return NORMAL;
}

int parse_simple_expression_cross() {
    if (cross_token == TPLUS || cross_token == TMINUS) {
        if (parse_additive_operator_cross() == ERROR) return (ERROR);
    }
    if (parse_term_cross() == ERROR) return (ERROR);
    if (integer_expression_flag == 0) return error("Expression must be integer type");
    while (cross_token == TPLUS || cross_token == TMINUS || cross_token == TOR) {
        if (parse_additive_operator_cross() == ERROR) return (ERROR);
        if (cross_token == TOR) {
            if (bool_expression_flag == 0) return (error("Expression must be boolean type"));
            integer_expression_flag = 0;
            bool_expression_flag = 1;
            char_expression_flag = 0;
        } else {
            if (integer_expression_flag == 0) return (error("Expression must be integer type"));
            integer_expression_flag = 1;
            bool_expression_flag = 0;
            char_expression_flag = 0;
        }
        if (parse_term_cross() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_term_cross() {
    if (parse_factor_cross() == ERROR) return (ERROR);
    while (cross_token == TSTAR || cross_token == TDIV || cross_token == TAND) {
        if (cross_token == TAND) {
            if (bool_expression_flag == 0) return (error("Expression must be boolean type"));
            integer_expression_flag = 0;
            bool_expression_flag = 1;
            char_expression_flag = 0;
        } else {
            if (integer_expression_flag == 0) return (error("Expression must be integer type"));
            integer_expression_flag = 1;
            bool_expression_flag = 0;
            char_expression_flag = 0;
        }
        if (parse_multiplicative_operator_cross() == ERROR) return (ERROR);
        if (parse_factor_cross() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_factor_cross() {
    switch (cross_token) {
        case TNAME:
            if (parse_variable_cross() == ERROR) return (ERROR);
            break;
        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING:
            if (parse_constant_cross() == ERROR) return (ERROR);
            break;
        case TLPAREN:
            if (cross_token != TLPAREN) return (error("Left parenthesis is not found"));
            cross_token = scan_loop();
            if (parse_expression_cross() == ERROR) return (ERROR);
            if (cross_token != TRPAREN) return (error("Right parenthesis is not found"));
            cross_token = scan_loop();
            break;
        case TNOT:
            if (cross_token != TNOT) return (error("Not is not found"));
            if (bool_expression_flag == 0) return (error("Expression must be boolean type"));
            cross_token = scan_loop();
            if (parse_factor_cross() == ERROR) return (ERROR);
            bool_expression_flag = 1;
            break;
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if (cross_token == TBOOLEAN) {
                bool_expression_flag = 1;
                integer_expression_flag = 0;
                char_expression_flag = 0;
            }
            if (cross_token == TINTEGER) {
                bool_expression_flag = 0;
                integer_expression_flag = 1;
                char_expression_flag = 0;
            }
            if (cross_token == TCHAR) {
                bool_expression_flag = 0;
                integer_expression_flag = 0;
                char_expression_flag = 1;
            }
            if (parse_standard_type_cross() == ERROR) return (ERROR);
            if (cross_token != TLPAREN) return (error("Left parenthesis is not found"));
            cross_token = scan_loop();
            if (parse_expression_cross() == ERROR) return (ERROR);
            if (cross_token != TRPAREN) return (error("Right parenthesis is not found"));
            cross_token = scan_loop();
            break;
        default:
            return (error("Factor is not found"));
    }
    return NORMAL;
}

int parse_constant_cross() {
    switch (cross_token) {
        case TNUMBER:
            if (cross_token != TNUMBER) return (error("Number is not found"));
            integer_expression_flag = 1;
            bool_expression_flag = 0;
            char_expression_flag = 0;
            cross_token = scan_loop();
            break;
        case TFALSE:
            if (cross_token != TFALSE) return (error("False is not found"));
            integer_expression_flag = 0;
            bool_expression_flag = 1;
            char_expression_flag = 0;
            cross_token = scan_loop();
            break;
        case TTRUE:
            if (cross_token != TTRUE) return (error("True is not found"));
            integer_expression_flag = 0;
            bool_expression_flag = 1;
            char_expression_flag = 0;
            cross_token = scan_loop();
            break;
        case TSTRING:
            if (cross_token != TSTRING) return (error("String is not found"));
            bool_expression_flag = 0;
            integer_expression_flag = 0;
            char_expression_flag = 0;
            cross_token = scan_loop();
            break;
    }
    return NORMAL;
}

int parse_multiplicative_operator_cross() {
    switch (cross_token) {
        case TSTAR:
            if (cross_token != TSTAR) return (error("Star is not found"));
            if (integer_expression_flag == 0) return (error("Expression must be integer type"));
            cross_token = scan_loop();
            integer_expression_flag = 1;
            bool_expression_flag = 0;
            char_expression_flag = 0;
            break;
        case TDIV:
            if (cross_token != TDIV) return (error("Div is not found"));
            if (integer_expression_flag == 0) return (error("Expression must be integer type"));
            cross_token = scan_loop();
            integer_expression_flag = 1;
            bool_expression_flag = 0;
            char_expression_flag = 0;
            break;
        case TAND:
            if (cross_token != TAND) return (error("And is not found"));
            if (bool_expression_flag == 0) return (error("Expression must be boolean type"));
            cross_token = scan_loop();
            integer_expression_flag = 0;
            bool_expression_flag = 1;
            char_expression_flag = 0;
            break;
        default:
            return (error("Multiplicative operator is not found"));
    }
    return NORMAL;
}

int parse_additive_operator_cross() {
    switch (cross_token) {
        case TPLUS:
            if (cross_token != TPLUS) return (error("Plus is not found"));
            if (integer_expression_flag == 0) return (error("Expression must be integer type"));
            cross_token = scan_loop();
            integer_expression_flag = 1;
            bool_expression_flag = 0;
            char_expression_flag = 0;
            break;
        case TMINUS:
            if (cross_token != TMINUS) return (error("Minus is not found"));
            if (integer_expression_flag == 0) return (error("Expression must be integer type"));
            cross_token = scan_loop();
            integer_expression_flag = 1;
            bool_expression_flag = 0;
            char_expression_flag = 0;
            break;
        case TOR:
            if (cross_token != TOR) return (error("Or is not found"));
            if (bool_expression_flag == 0) return (error("Expression must be boolean type"));
            cross_token = scan_loop();
            integer_expression_flag = 0;
            bool_expression_flag = 1;
            char_expression_flag = 0;
            break;
        default:
            return (error("Additive operator is not found"));
    }
    return NORMAL;
}

int parse_relational_operator_cross() {
    switch (cross_token) {
        case TEQUAL:
            if (cross_token != TEQUAL) return (error("Equal is not found"));
            cross_token = scan_loop();
            break;
        case TNOTEQ:
            if (cross_token != TNOTEQ) return (error("Not equal is not found"));
            cross_token = scan_loop();
            break;
        case TLE:
            if (cross_token != TLE) return (error("Less than is not found"));
            cross_token = scan_loop();
            break;
        case TLEEQ:
            if (cross_token != TLEEQ) return (error("Less than or equal is not found"));
            cross_token = scan_loop();
            break;
        case TGR:
            if (cross_token != TGR) return (error("Greater than is not found"));
            cross_token = scan_loop();
            break;
        case TGREQ:
            if (cross_token != TGREQ) return (error("Greater than or equal is not found"));
            cross_token = scan_loop();
            break;
        default:
            return (error("Relational operator is not found"));
    }
    integer_expression_flag = 0;
    bool_expression_flag = 1;
    char_expression_flag = 0;
    return NORMAL;
}

int parse_input_statement_cross() {
    if (cross_token != TREAD && cross_token != TREADLN) return (error("Read or Readln is not found"));
    cross_token = scan_loop();
    while (cross_token == TLPAREN) {
        if (cross_token != TLPAREN) return (error("Left parenthesis is not found"));
        cross_token = scan_loop();
        if (parse_variable_cross() == ERROR) return (ERROR);
        if (integer_expression_flag != 1 && char_expression_flag != 1) { return (error("Expression must be integer or char type")); }
        while (cross_token == TCOMMA) {
            if (cross_token != TCOMMA) return (error("Comma is not found"));
            cross_token = scan_loop();
            if (integer_expression_flag != 1 && char_expression_flag != 1) { return (error("Expression must be integer or char type")); }
            if (parse_variable_cross() == ERROR) return (ERROR);
        }
        if (cross_token != TRPAREN) return (error("Right parenthesis is not found"));
        cross_token = scan_loop();
    }
    return NORMAL;
}

int parse_output_statement_cross() {
    if (cross_token != TWRITE && cross_token != TWRITELN) return (error("Write or Writeln is not found"));
    cross_token = scan_loop();
    while (cross_token == TLPAREN) {
        if (cross_token != TLPAREN) return (error("Left parenthesis is not found"));
        cross_token = scan_loop();
        if (parse_output_format_cross() == ERROR) return (ERROR);
        while (cross_token == TCOMMA) {
            if (cross_token != TCOMMA) return (error("Comma is not found"));
            cross_token = scan_loop();
            if (parse_output_format_cross() == ERROR) return (ERROR);
        }
        if (cross_token != TRPAREN) return (error("Right parenthesis is not found"));
        cross_token = scan_loop();
    }
    return NORMAL;
}

int parse_output_format_cross() {
    if (cross_token == TPLUS || cross_token == TMINUS || cross_token == TNUMBER || cross_token == TNAME || cross_token == TTRUE || cross_token == TFALSE || cross_token == TLPAREN || cross_token == TNOT || cross_token == TINTEGER || cross_token == TBOOLEAN || cross_token == TCHAR) {
        if (parse_expression_cross()) { return error("Expression not found"); }
        if (cross_token == TCOLON) {
            if (cross_token != TCOLON) return (error("Colon is not found"));
            cross_token = scan_loop();
            if (cross_token != TNUMBER) return (error("Number is not found"));
            cross_token = scan_loop();
        }
    } else if (cross_token == TSTRING) {
        if (cross_token != TSTRING) return (error("String is not found"));
        cross_token = scan_loop();
    } else {
        return (error("Expression or String is not found"));
    }


    return NORMAL;
}
