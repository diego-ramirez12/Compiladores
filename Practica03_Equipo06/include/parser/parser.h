#ifndef MINIC_PARSER_H
#define MINIC_PARSER_H

#include "lexer/lexer.h"

typedef struct {
    Lexer *lexer;

    Token current;
    int has_current;

    int had_error;
    int panic_mode;
    int internal_failure;
} Parser;

int parser_init(Parser *parser, Lexer *lexer);

int parser_advance(Parser *parser);

int parser_check(const Parser *parser, TokenType type);

int parser_match(Parser *parser, TokenType type);

const Token *parser_current(const Parser *parser);

void parser_destroy(Parser *parser);

int parser_parse(Parser *parser);

#endif
