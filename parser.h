/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"



using namespace std;
struct REG {
	struct REG_node * start;
	struct REG_node * accept;
};

class Parser {
  private:
    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();


    void parse_input();
    void parse_char_list();
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    struct REG * parse_expr();


  public:
    void ParseProgram();
};


//Written by Nicholas Jorgensen, code provided in project 1 description.
struct REG_node {
  int uniqueID;
  struct REG_node * first_neighbor;
  char first_label;
  struct REG_node * second_neighbor;
  char second_label;
};




struct token {
  Token name;
  struct REG* r;
};


struct Reg_structure_list{
	Token T_name;
	REG* Register;

};


struct retrievedToken {
  string tokenName;
  string matched;

  retrievedToken(string name, string lexeme) : tokenName(name), matched(lexeme) {}
};

#endif
