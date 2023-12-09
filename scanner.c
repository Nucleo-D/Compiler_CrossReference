//
// Created by li-xinjia on 23/10/15.
//

#include "scanner.h"
#include "identifier.h"
#include "token.h"
#include <stdio.h>

FILE *fp = NULL;

char is_alphabet(char c) {
    return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", c) != NULL;
}

char is_number(char c) {
    return c >= '0' && c <= '9';
}

char is_spaces(char c) {
    if (c == '\n') {
        line_number++;
    }
    return strchr(" \t\n\r", c) != NULL;
}

int is_keyword(char *str) {
    for (int i = 0; i < KEYWORDSIZE; ++i) {
        if (strcmp(str, key[i].keyword) == 0) {
            return key[i].keytoken;
        }
    }
    return -1;
}

char is_symbol(char c) {
    return strchr("+-*=;(){},.<>[]:!", c) != NULL;
}

void read_all_comment() {
    char c, comment_type;
    c = getc(fp);
    if (c == '{') {
        comment_type = 0;
    } else if (c == '/') {
        c = getc(fp);
        if (c == '*') {
            comment_type = 1;
        } else {
            error_scan("Unknown character.");
        }
    } else {
        ungetc(c, fp);
        return;
    }

    while (1) {
        c = getc(fp);
        if (c == '\n') {
            line_number++;
        }
        if (c == '}' && comment_type == 0) {
            return;
        }
        if (c == '*' && comment_type == 1) {
            c = getc(fp);
            if (c == '/') {
                return;
            } else {
                ungetc(c, fp);
            }
        }
        if (c == EOF) {
            return;
        }
    }
}


int init_scan(char *filename) {
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    init_idtab();

    return 0;
}


int scan(void) {
    char c, str[MAXSTRSIZE];
    int  p = 0;

    c = getc(fp);

    // Check if EOF
    if (c == EOF) {
        return -1;
    }

    if (is_spaces(c)) {
        return 0;
    }
    if (c == '{' || c == '/') {
        ungetc(c, fp);
        read_all_comment();
        return 0;
    }

    // Read number
    if (is_number(c)) {
        p = 0;
        num_attr = c - 48;
        num_string_attr[p++] = c;
        while (1) {
            c = getc(fp);
            if (is_number(c)) {
                num_attr = num_attr * 10 + c - 48;
                num_string_attr[p++] = c;
                if (num_attr > 32768) {
                    error_scan("Number is too large.");
                }
                continue;
            }
            if (is_spaces(c) || c == EOF) {
                break;
            }
            if (c == '{' || c == '/') {
                ungetc(c, fp);
                read_all_comment();
                break;
            }
            ungetc(c, fp);
            break;
        }
        num_string_attr[p] = '\0';
        return TNUMBER;
    }

    // Read symbol
    if (is_symbol(c)) {

        // Save the first c
        str[0] = c;

        // Check if double symbol
        c = getc(fp);
        str[1] = c;
        str[2] = '\0';
        for (int i = 0; i < NUMOFTOKEN; ++i) {
            if (strcmp(str, tokenstr[i]) == 0) {
                return i;
            }
        }

        // Give back c if not double
        ungetc(c, fp);

        // Check if single symbol
        str[1] = '\0';
        for (int i = 0; i < NUMOFTOKEN; ++i) {
            if (strcmp(str, tokenstr[i]) == 0) {
                return i;
            }
        }

        error_scan("Symbol not exist");
        return -1;
    }

    // Read string
    if (c == '\'') {
        p = 0;
        while (1) {
            c = getc(fp);
            if (c == '\'') {
                c = getc(fp);
                if (c == '\'') {
                    string_attr[p++] = '\'';
                } else {
                    ungetc(c, fp);
                    string_attr[p++] = '\0';
                    return TSTRING;
                }
            }
            if (c == EOF) {
                error_scan("String not close.");
                return -1;
            }
            if (p >= MAXSTRSIZE) {
                error_scan("String too long.");
                return -1;
            }
            string_attr[p++] = c;
        }
    }

    // Read name
    ungetc(c, fp);
    p = 0;
    while (1) {
        c = getc(fp);
        if (is_spaces(c) || c == EOF) {
            str[p++] = '\0';
            break;
        }
        if (c == '{' || c == '/') {
            str[p++] = '\0';
            ungetc(c, fp);
            read_all_comment();
            break;
        }
        if (is_symbol(c)) {
            str[p++] = '\0';
            ungetc(c, fp);
            break;
        }
        if (is_alphabet(c) || is_number(c)) {
            str[p++] = c;
            continue;
        }
        error_scan("Unknown character.");
    }

    // Check if keyword
    int keyword_num = is_keyword(str);
    if (keyword_num != -1) {
        return keyword_num;
    }

//    id_countup(str);
    sprintf(name_attr, "%s", str);
    return TNAME;
}

int get_linenum() {
    return line_number;
}

void end_scan(void) {
    fclose(fp);
}

//void print_result(void) {
//    if (numtoken[1] > 0) {
//        printf("\"%s\"\t%d\n", tokenstr[1], numtoken[1]);
//        print_idtab();
//    }
//    for (int i = 2; i < NUMOFTOKEN; ++i) {
//        if (numtoken[i] > 0)
//            printf("\"%s\"\t%d\n", tokenstr[i], numtoken[i]);
//    }
//    release_idtab();
//}