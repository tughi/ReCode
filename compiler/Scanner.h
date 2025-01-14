/* Copyright (C) 2024 Stefan Selariu */

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Token.h"

typedef struct Scanner {
    Source *source;
    size_t current_char_index;
    uint16_t current_line;
    uint16_t current_column;
    Source_Location current_location;
    Token *current_token;
} Scanner;

Scanner *Scanner__create(Source *source);

Token *Scanner__next_token(Scanner *self);

Token *Scanner__peek_token(Scanner *self, uint8_t offset);

#endif