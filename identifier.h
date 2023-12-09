//
// Created by li-xinjia on 23/10/21.
//

#ifndef SCANNER_IDENTIFIER_H
#define SCANNER_IDENTIFIER_H

#include "token.h"

struct TYPE {
    int ttype; /* TPINT TPCHAR TPBOOL TPARRAYINT TPARRAYCHAR TPARRAYBOOL TPPROC */
    int arraysize; /* 配列型の場合の配列サイズ */
    struct TYPE *etp; /* 配列型の場合の要素の型 */
    struct TYPE *paratp; /* 手続き型の場合の，仮引数の型リスト */
};

struct LINE {
    int reflinenum;
    struct LINE *nextlinep;
};

struct ID {
    char *name;
    char *procname; /* 手続き宣言内で定義されている時の手続き名．それ以外はNULL */
    struct TYPE *itp; /* 型 */
    int ispara; /* 1: 仮引数, 0: 通常の変数 */
    int deflinenum; /* 定義行 */
    struct LINE *irefp; /* 参照行のリスト */
    struct ID *nextp;
};

struct ID *search_idtab(char *np);
void       init_idtab();
//void       id_countup(char *np);
//void       print_idtab();
void       release_idtab();


#endif //SCANNER_IDENTIFIER_H
