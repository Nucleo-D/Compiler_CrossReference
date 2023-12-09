//
// Created by li-xinjia on 23/11/20.
//

#include "prettyPrint.h"

int  token;
char buffer[4096];
int  tab_num = 0;
int  can_break = 0;
int  in_procedure = 0;
char current_procedure_name[MAXSTRSIZE];

void init_pretty_print() {
    setvbuf(stdout, buffer, _IOFBF, 4096);
    token = scan_loop();
}

void pretty_print() {
    fflush(stdout);
}

void printf_with_tab(char *str, ...) {
    for (int i = 0; i < tab_num; ++i) {
        printf("    ");
    }

    va_list args;
    va_start(args, str);

    vprintf(str, args);
    va_end(args);
}

int scan_loop(void) {
    int result = scan();
    while (result == 0) {
        result = scan();
    }
    return result;
}

int parse_program() {
    if (token != TPROGRAM) return (error("Keyword ’program’ is not found"));
    printf("program");
    token = scan_loop();
    if (token != TNAME) return (error("Program name is not found"));
    printf(" %s", name_attr);
    token = scan_loop();
    if (token != TSEMI) return (error("Semicolon is not found"));
    printf(";\n");
    token = scan_loop();
    if (parse_block() == ERROR) return (ERROR);
    if (token != TDOT) return (error("Period is not found at the end of program"));
    printf(".");
    token = scan_loop();
    return (NORMAL);
}

int parse_block() {
    while (token == TVAR || token == TPROCEDURE) {
        switch (token) {
            case TVAR:
                if (parse_variable_declaration() == ERROR) return (ERROR);
                break;
            case TPROCEDURE:
                if (parse_subprogram_declaration() == ERROR) return (ERROR);
                break;
            default:
                return (error("Block is not found"));
        }
    }
    if (parse_compound_statement() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_variable_declaration() {
    if (token != TVAR) return (error("Var is not found"));
    printf_with_tab("    var\n");
    printf_with_tab("        ");
    token = scan_loop();
    if (parse_variable_names() == ERROR) return (ERROR);
    if (token != TCOLON) return (error("Colon is not found"));
    printf(" : ");
    token = scan_loop();
    if (parse_type() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    printf(";\n");
    token = scan_loop();
    while (token == TNAME) {
        printf_with_tab("        ");
        if (parse_variable_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        printf(" : ");
        token = scan_loop();
        if (parse_type() == ERROR) return (ERROR);
        if (token != TSEMI) return (error("Semicolon is not found"));
        printf(";\n");
        token = scan_loop();
    }
    return NORMAL;
}

int parse_variable_names() {
    if (parse_variable_name() == ERROR) return (ERROR);
    while (token == TCOMMA) {
        if (token != TCOMMA) return (error("Comma is not found"));
        printf(" ,");
        token = scan_loop();
        if (!is_symbol(tokenstr[token][0])) printf(" ");
        if (parse_variable_name() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_variable_name() {
    if (token != TNAME) return (error("Variable name is not found"));
    printf("%s", name_attr);
    token = scan_loop();
    return NORMAL;
}

int parse_type() {
    switch (token) {
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if (parse_standard_type() == ERROR) return (ERROR);
            break;
        case TARRAY:
            if (parse_array_type() == ERROR) return (ERROR);
            break;
        default:
            return (error("Type is not found"));
    }
    return NORMAL;
}

int parse_standard_type() {
    if (token != TINTEGER && token != TBOOLEAN && token != TCHAR) return (error("Standard type is not found"));
    printf("%s", tokenstr[token]);
    token = scan_loop();
    return NORMAL;
}

int parse_array_type() {
    if (token != TARRAY) return (error("Array is not found"));
    printf("array");
    token = scan_loop();
    if (token != TLSQPAREN) return (error("Left square parenthesis is not found"));
    printf(" [ ");
    token = scan_loop();
    if (token != TNUMBER) return (error("Number is not found"));
    printf("%s", num_string_attr);
    token = scan_loop();
    if (token != TRSQPAREN) return (error("Right square parenthesis is not found"));
    printf(" ] ");
    token = scan_loop();
    if (token != TOF) return (error("Of is not found"));
    printf("of ");
    token = scan_loop();
    if (parse_standard_type() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_subprogram_declaration() {
    if (token != TPROCEDURE) return (error("Procedure is not found"));
    in_procedure = 1;
    tab_num++;
    printf_with_tab("procedure ");
    token = scan_loop();
    if (parse_procedure_name() == ERROR) return (ERROR);
    strcpy(current_procedure_name, name_attr);
    if (token == TLPAREN) {
        if (parse_formal_parameters() == ERROR) return (ERROR);
    }
    if (token != TSEMI) return (error("Semicolon is not found"));
    printf(";\n");
    token = scan_loop();
    if (token == TVAR) {
        if (parse_variable_declaration() == ERROR) return (ERROR);
    }
    if (parse_compound_statement() == ERROR) return (ERROR);
    if (token != TSEMI) return (error("Semicolon is not found"));
    printf(";\n");
    token = scan_loop();
    tab_num--;
    in_procedure = 0;
    return NORMAL;
}

int parse_procedure_name() {
    if (token != TNAME) return (error("Procedure name is not found"));
    printf("%s", name_attr);
    token = scan_loop();
    return NORMAL;
}

int parse_formal_parameters() {
    if (token != TLPAREN) return (error("Left parenthesis is not found"));
    printf(" ( ");
    token = scan_loop();
    if (parse_variable_names() == ERROR) return (ERROR);
    if (token != TCOLON) return (error("Colon is not found"));
    printf(" : ");
    token = scan_loop();
    if (parse_type() == ERROR) return (ERROR);
    while (token == TSEMI) {
        if (token != TSEMI) return (error("Semicolon is not found"));
        printf(";");
        token = scan_loop();
        if (parse_variable_names() == ERROR) return (ERROR);
        if (token != TCOLON) return (error("Colon is not found"));
        printf(" : ");
        token = scan_loop();
        if (parse_type() == ERROR) return (ERROR);
    }
    if (token != TRPAREN) return (error("Right parenthesis is not found"));
    printf(" )");
    token = scan_loop();
    return NORMAL;
}

int parse_compound_statement() {
    if (token != TBEGIN) return (error("Begin is not found"));
    printf_with_tab("begin\n");
    tab_num++;
    token = scan_loop();
    while (token != TEND && token != TDOT) {
        if (parse_statement() == ERROR) return (ERROR);
        if (token == TEND) {
            printf("\n");
            break;
        }
        if (token != TSEMI) return (error("Semicolon is not found"));
        printf(";\n");
        token = scan_loop();
    }
    if (token != TEND) return (error("End is not found"));
    tab_num--;
    printf_with_tab("end");
    token = scan_loop();
    return NORMAL;
}

int parse_statement() {
    switch (token) {
        case TNAME:
            if (parse_assignment_statement() == ERROR) return (ERROR);
            break;
        case TIF:
            if (parse_condition_statement() == ERROR) return (ERROR);
            break;
        case TWHILE:
            if (parse_iteration_statement() == ERROR) return (ERROR);
            break;
        case TBREAK:
            if (parse_exit_statement() == ERROR) return (ERROR);
            break;
        case TCALL:
            if (parse_call_statement() == ERROR) return (ERROR);
            break;
        case TRETURN:
            if (parse_return_statement() == ERROR) return (ERROR);
            break;
        case TBEGIN:
            if (parse_compound_statement() == ERROR) return (ERROR);
            break;
        case TREAD:
        case TREADLN:
            if (parse_input_statement() == ERROR) return (ERROR);
            break;
        case TWRITE:
        case TWRITELN:
            if (parse_output_statement() == ERROR) return (ERROR);
            break;
        default:
    }
    return NORMAL;
}

int parse_condition_statement() {
    if (token != TIF) return (error("If is not found"));
    printf_with_tab("if");
    tab_num++;
    token = scan_loop();
    if (token != TLPAREN) printf(" ");
    if (parse_expression() == ERROR) return (ERROR);
    if (token != TTHEN) return (error("Then is not found"));
    printf(" then\n");
    token = scan_loop();
    if (parse_statement() == ERROR) return (ERROR);
    tab_num--;
    if (token == TELSE) {
        tab_num++;
        if (token != TELSE) return (error("Else is not found"));
        printf("\n");
        tab_num--;
        printf_with_tab("else\n");
        tab_num++;
        token = scan_loop();
        if (parse_statement() == ERROR) return (ERROR);
        tab_num--;
    }
    return NORMAL;
}

int parse_iteration_statement() {
    if (token != TWHILE) return (error("While is not found"));
    printf_with_tab("while");
    token = scan_loop();
    if (!is_symbol(tokenstr[token][0])) printf(" ");
    if (parse_expression() == ERROR) return (ERROR);
    if (token != TDO) return (error("Do is not found"));
    printf(" do\n");
    tab_num++;
    can_break++;
    token = scan_loop();
    if (parse_statement() == ERROR) return (ERROR);
    tab_num--;
    can_break--;
    return NORMAL;
}

int parse_exit_statement() {
    if (token != TBREAK) return (error("Break is not found"));
    printf_with_tab("break");
    token = scan_loop();
    char error_message[100] = "";
    sprintf(error_message, "'break' must be in a while loop. (current token: '%s')", tokenstr[token]);
    if (can_break == 0) return (error(error_message));
    return NORMAL;
}

int parse_call_statement() {
    if (token != TCALL) return (error("Call is not found"));
    printf_with_tab("call ");
    token = scan_loop();
    if (parse_procedure_name() == ERROR) return (ERROR);
    if (token == TLPAREN) {
        if (token != TLPAREN) return (error("Left parenthesis is not found"));
        printf(" ( ");
        token = scan_loop();
        if (parse_expressions() == ERROR) return (ERROR);
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        printf(" )");
        token = scan_loop();
    }
    return NORMAL;
}

int parse_expressions() {
    if (parse_expression() == ERROR) return (ERROR);
    while (token == TCOMMA) {
        if (token != TCOMMA) return (error("Comma is not found"));
        printf(" ,");
        token = scan_loop();
        if (!is_symbol(tokenstr[token][0])) printf(" ");
        if (parse_expression() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_return_statement() {
    if (token != TRETURN) return (error("Return is not found"));
    printf_with_tab("return");
    token = scan_loop();
    return NORMAL;
}

int parse_assignment_statement() {
    printf_with_tab("");
    if (parse_left_part() == ERROR) return (ERROR);
    if (token != TASSIGN) return (error("Assign is not found"));
    printf(" :=");
    token = scan_loop();
    if (!is_symbol(tokenstr[token][0])) printf(" ");
    if (parse_expression() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_left_part() {
    if (parse_variable() == ERROR) return (ERROR);
    return NORMAL;
}

int parse_variable() {
    if (parse_variable_name() == ERROR) return (ERROR);
    while (token == TLSQPAREN) {
        if (token != TLSQPAREN) return (error("Left square parenthesis is not found"));
        printf(" [ ");
        token = scan_loop();
        if (parse_expression() == ERROR) return (ERROR);
        if (token != TRSQPAREN) return (error("Right square parenthesis is not found"));
        printf(" ]");
        if (!is_symbol(tokenstr[token][0]) || token == TTHEN) printf(" ");
        token = scan_loop();
    }
    return NORMAL;
}

int parse_expression() {
    if (parse_simple_expression() == ERROR) return (ERROR);
    if (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        if (parse_relational_operator() == ERROR) return (ERROR);
        if (parse_simple_expression() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_simple_expression() {
    if (token == TPLUS || token == TMINUS) {
        if (parse_additive_operator() == ERROR) return (ERROR);
    }
    if (parse_term() == ERROR) return (ERROR);
    while (token == TPLUS || token == TMINUS || token == TOR) {
        if (parse_additive_operator() == ERROR) return (ERROR);
        if (parse_term() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_term() {
    if (parse_factor() == ERROR) return (ERROR);
    while (token == TSTAR || token == TDIV || token == TAND) {
        if (parse_multiplicative_operator() == ERROR) return (ERROR);
        if (parse_factor() == ERROR) return (ERROR);
    }
    return NORMAL;
}

int parse_factor() {
    switch (token) {
        case TNAME:
            if (parse_variable() == ERROR) return (ERROR);
            break;
        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING:
            if (parse_constant() == ERROR) return (ERROR);
            break;
        case TLPAREN:
            if (token != TLPAREN) return (error("Left parenthesis is not found"));
            printf(" ( ");
            token = scan_loop();
            if (parse_expression() == ERROR) return (ERROR);
            if (token != TRPAREN) return (error("Right parenthesis is not found"));
            printf(" )");
            token = scan_loop();
            break;
        case TNOT:
            if (token != TNOT) return (error("Not is not found"));
            printf("not ");
            token = scan_loop();
            if (parse_factor() == ERROR) return (ERROR);
            break;
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if (parse_standard_type() == ERROR) return (ERROR);
            if (token != TLPAREN) return (error("Left parenthesis is not found"));
            printf(" ( ");
            token = scan_loop();
            if (parse_expression() == ERROR) return (ERROR);
            if (token != TRPAREN) return (error("Right parenthesis is not found"));
            printf(" )");
            token = scan_loop();
            break;
        default:
            return (error("Factor is not found"));
    }
    return NORMAL;
}

int parse_constant() {
    switch (token) {
        case TNUMBER:
            if (token != TNUMBER) return (error("Number is not found"));
            printf("%s", num_string_attr);
            token = scan_loop();
            break;
        case TFALSE:
            if (token != TFALSE) return (error("False is not found"));
            printf("false");
            token = scan_loop();
            break;
        case TTRUE:
            if (token != TTRUE) return (error("True is not found"));
            printf("true");
            token = scan_loop();
            break;
        case TSTRING:
            if (token != TSTRING) return (error("String is not found"));
            printf("\'%s\'", string_attr);
            token = scan_loop();
            break;
    }
    return NORMAL;
}

int parse_multiplicative_operator() {
    switch (token) {
        case TSTAR:
            if (token != TSTAR) return (error("Star is not found"));
            printf(" *");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TDIV:
            if (token != TDIV) return (error("Div is not found"));
            printf(" div");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TAND:
            if (token != TAND) return (error("And is not found"));
            printf(" and");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        default:
            return (error("Multiplicative operator is not found"));
    }
    return NORMAL;
}

int parse_additive_operator() {
    switch (token) {
        case TPLUS:
            if (token != TPLUS) return (error("Plus is not found"));
            printf(" +");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TMINUS:
            if (token != TMINUS) return (error("Minus is not found"));
            printf(" -");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TOR:
            if (token != TOR) return (error("Or is not found"));
            printf(" or");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        default:
            return (error("Additive operator is not found"));
    }
    return NORMAL;
}

int parse_relational_operator() {
    switch (token) {
        case TEQUAL:
            if (token != TEQUAL) return (error("Equal is not found"));
            printf(" =");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TNOTEQ:
            if (token != TNOTEQ) return (error("Not equal is not found"));
            printf(" <>");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TLE:
            if (token != TLE) return (error("Less than is not found"));
            printf(" <");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TLEEQ:
            if (token != TLEEQ) return (error("Less than or equal is not found"));
            printf(" <=");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TGR:
            if (token != TGR) return (error("Greater than is not found"));
            printf(" >");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        case TGREQ:
            if (token != TGREQ) return (error("Greater than or equal is not found"));
            printf(" >=");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            break;
        default:
            return (error("Relational operator is not found"));
    }
    return NORMAL;
}

int parse_input_statement() {
    if (token != TREAD && token != TREADLN) return (error("Read or Readln is not found"));
    printf_with_tab("%s", tokenstr[token]);
    token = scan_loop();
    while (token == TLPAREN) {
        if (token != TLPAREN) return (error("Left parenthesis is not found"));
        printf(" ( ");
        token = scan_loop();
        if (parse_variable() == ERROR) return (ERROR);
        while (token == TCOMMA) {
            if (token != TCOMMA) return (error("Comma is not found"));
            printf(" ,");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            if (parse_variable() == ERROR) return (ERROR);
        }
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        printf(" )");
        token = scan_loop();
    }
    return NORMAL;
}

int parse_output_statement() {
    if (token != TWRITE && token != TWRITELN) return (error("Write or Writeln is not found"));
    printf_with_tab("%s", tokenstr[token]);
    token = scan_loop();
    while (token == TLPAREN) {
        if (token != TLPAREN) return (error("Left parenthesis is not found"));
        printf(" ( ");
        token = scan_loop();
        if (parse_output_format() == ERROR) return (ERROR);
        while (token == TCOMMA) {
            if (token != TCOMMA) return (error("Comma is not found"));
            printf(" ,");
            token = scan_loop();
            if (!is_symbol(tokenstr[token][0])) printf(" ");
            if (parse_output_format() == ERROR) return (ERROR);
        }
        if (token != TRPAREN) return (error("Right parenthesis is not found"));
        printf(" )");
        token = scan_loop();
    }
    return NORMAL;
}

int parse_output_format() {
    if (parse_expression() == ERROR) return (ERROR);
    if (token == TCOLON) {
        if (token != TCOLON) return (error("Colon is not found"));
        printf(" : ");
        token = scan_loop();
        if (token != TNUMBER) return (error("Number is not found"));
        printf("%s", num_string_attr);
        token = scan_loop();
    }
    if (token == TSTRING) {
        if (token != TSTRING) return (error("String is not found"));
        printf("\'%s\'", string_attr);
        token = scan_loop();
    }
    return NORMAL;
}
