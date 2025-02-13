//  Copyright (C) 2021 Benjamin Stürz
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef FILE_TOKEN_H
#define FILE_TOKEN_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "strint.h"
#include "config.h"

#if ENABLE_FP
typedef long double fpmax_t;
#endif

enum token_type {
   TK_DUMMY,
   TK_INTEGER,
#if ENABLE_FP
   TK_FLOAT,
#endif
   TK_STRING, TK_CHARACTER, TK_NAME,
   TK_LPAREN, TK_RPAREN,
   TK_LBRACK, TK_RBRACK,
   TK_CLPAREN, TK_CRPAREN,
   TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH,
   TK_PLEQ, TK_MIEQ, TK_STEQ, TK_SLEQ,
   TK_PLPL, TK_MIMI,
   TK_NOT, TK_NEQ,
   TK_EQ, TK_EQEQ,
   TK_GR, TK_GRGR, TK_GREQ, TK_GRGREQ,
   TK_LE, TK_LELE, TK_LEEQ, TK_LELEEQ,
   TK_COMMA, TK_COLON, TK_SEMICOLON,
   TK_AMP, TK_AMPAMP, TK_AMPEQ,
   TK_PIPE, TK_PIPI, TK_PIPEEQ,
   TK_XOR, TK_XOREQ, TK_WAVE,
   TK_PERC, TK_PERCEQ,
   TK_QMARK, TK_DDD, TK_DOT,
   TK_ARROW,

   TK_EOF,
   KW_CONST, KW_VOID, KW_SIGNED, KW_UNSIGNED,
   KW_BYTE, KW_CHAR, KW_SHORT, KW_INT, KW_LONG,
   KW_FLOAT, KW_DOUBLE,
   KW_IF, KW_ELSE, KW_WHILE, KW_DO,
   KW_RETURN, KW_FOR, KW_BREAK, KW_CONTINUE,
   KW_SIZEOF, KW_ARRAYLEN, KW_AUTO,
   KW_EXTERN, KW_STATIC, KW_TYPEDEF,
   KW_ENUM, KW_STRUCT, KW_UNION,
   KW_TYPEOF, KW_SWITCH, KW_CASE,
   KW_DEFAULT, KW_VOLATILE,
   KW_RESTRICT, KW_BOOL, KW_NORETURN,
   KW_STATIC_ASSERT, KW_INLINE,
   NUM_TOKENS,
};

extern const char* token_type_str[NUM_TOKENS];

struct source_pos {
   const char* file;
   size_t line;
   size_t column;
};

struct token {
   enum token_type type;
   struct source_pos begin, end;
   union {
      istr_t str;
      uintmax_t iVal;
#if ENABLE_FP
      fpmax_t fVal;
#endif
      int ch;
   };
};

void token_init(void);
void print_token(FILE*, const struct token*);
void print_token_info(FILE*, const struct token*);
void print_source_pos(FILE*, const struct source_pos*);


#endif /* FILE_TOKEN_H */
