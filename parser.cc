 /*Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include  <set>
#include "parser.h"
#include "cstdlib"
#include <vector>



using namespace std;

int unique_id = 1;

std::vector <token> listOfREGs;


void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}


Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}


Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}


set <REG_node *> match_one_char(set <REG_node*>S, char c) {

    std::set <REG_node *> S_dash ;
    std::set<REG_node*>::iterator it;
    REG_node* n;
    for (it = S.begin(); it != S.end(); ++it)
    {
    	n =*it;

        if (n->first_label == c) {
        	S_dash.insert(n->first_neighbor);
        }
        if (n->second_label == c) {
        	S_dash.insert(n->second_neighbor);
        }
    }

    if (S_dash.size() == 0) {
        return S_dash;
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (it = S_dash.begin(); it != S_dash.end(); ++it){
        	n = *it;

            if (n->first_label == '_' &&
                (S_dash.find(n->first_neighbor) == S_dash.end())) {
            	S_dash.insert(n->first_neighbor);
                changed = true;
            }
            if (n->second_label == '_' &&
                (S_dash.find(n->second_neighbor) == S_dash.end())) {
            	S_dash.insert(n->second_neighbor);
                changed = true;
            }
        }
    }

    return S_dash;
}

// Parsing


string match(REG* r, string input,int p) {

    set <REG_node *> setOfNodes;
    setOfNodes.insert(r->start);
    setOfNodes = match_one_char(setOfNodes,'_');
    setOfNodes.insert(r->start);

    bool not_empty = true;
    int q = p;
    string matchedPrefix = "";

    while (p < input.size() && not_empty) {
    	if (input[p] == ' ') {
    	    			break;
    	    		}
    	setOfNodes = match_one_char(setOfNodes, input[p]);
        if (setOfNodes.size() == 0) {
            not_empty = false;
        }
        else if (setOfNodes.find(r->accept) != setOfNodes.end()) {
            matchedPrefix = input.substr(q, p-q+1);
        }
        else {
            matchedPrefix = "";
        }
        p++;
    }

    return matchedPrefix;
}



Get_back GetToken_p(vector <token> L, string s, int p) {

	string lM= "";
	string cM= "";
	string tN = "";
	token n;

	p =0;
	for (std::vector<token>::iterator it = L.begin() ; it != L.end() ; ++it) {
		n = *it;
		cM = match(n.r, s, p);
		if (cM > lM) {
			lM = cM;
			tN = n.name.lexeme;
		}
	}

	return Get_back(tN, lM);
}


void Parser::parse_input()
{
    //input -> tokens_section INPUT_TEXT

    parse_tokens_section();
    Token t = expect(INPUT_TEXT);
    int x = t.lexeme.length()-2;
    	string st = t.lexeme.substr(1,x);
    int p = 0;
    	while (p < st.length()) {

    		while (st[p] == ' ') {
    			p++;
    		}

    		if(!(p<st.length()))
    		{
    			break;
    		}

    		Get_back tok = GetToken_p(listOfREGs, st.substr(p,st.length()), p);
    		p = p + tok.matched.length();

    		if (tok.matched != "") {
    			cout << tok.tokenName << " , \"" << tok.matched << "\"" << "\n";
    		}
    	}
}







void Parser::parse_tokens_section()
{
    // tokens_section -> token_list HASH


    parse_token_list();
    expect(HASH);
}

void Parser::parse_token_list()
{
    // token_list -> token
    // token_list -> token COMMA token_list

    parse_token();
    Token t = peek();
    if (t.token_type == COMMA)
    {
        // token_list -> token COMMA token_list
        expect(COMMA);
        parse_token_list();
    }
    else if (t.token_type == HASH)
    {
        // token_list -> token
    }
    else
    {
        syntax_error();
    }

}

void Parser::parse_token()
{
    // token -> ID expr
	Token t = lexer.GetToken();

   int a;
   struct REG * FREG;
   struct token temp;
   temp.name = t;
   temp.r = parse_expr();
   listOfREGs.push_back(temp);
   unique_id = 1;
     //a = 2+3;
}


REG * Parser::parse_expr()
{
   struct REG * UREG = new struct REG;
   struct REG_node * fstart = new REG_node;
   fstart->uniqueID = unique_id;
   unique_id++;
   //id++;
   //fstart->uniqueID = id;
   struct REG_node * faccept = new REG_node;
   faccept->uniqueID = unique_id;
   unique_id++;
   //id++;
   //faccept->uniqueID = id;

   // expr -> CHAR
   // expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
   // expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
   // expr -> LPAREN expr RPAREN STAR
   // expr -> UNDERSCORE

   Token t = lexer.GetToken();
   if (t.token_type == CHAR) {
      // expr -> CHAR
      fstart->first_label = t.lexeme[0];
      fstart->first_neighbor = NULL; //changed faccept

      UREG->start = fstart;
      UREG->accept = faccept;
   }
   else if (t.token_type == UNDERSCORE) {
      // expr -> UNDERSCORE
        fstart->first_label = t.lexeme[0];

        fstart->first_neighbor = NULL; //changed faccept
        UREG->start = fstart;
        UREG->accept = faccept;


   }
   else if (t.token_type == LPAREN) {
      // expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
      // expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
      // expr -> LPAREN expr RPAREN STAR
      struct REG * r1 = parse_expr();
      expect(RPAREN);
      Token t2,tem;
      tem  = peek();
      if(tem.token_type != COMMA )
      {
    	  if(tem.token_type != HASH)
    	  {
    		  t2 = lexer.GetToken();
    	  }
      }

      if (t2.token_type == DOT) {
         expect(LPAREN);
         struct REG * r2 = parse_expr();
         expect(RPAREN);

         r1->accept->first_neighbor = r2->start;
         r1->accept->first_label = '_';

         UREG->start = r1->start;
         UREG->accept = r2->accept;
      }
      else if (t2.token_type == OR) {
         expect(LPAREN);
         struct REG * r2 = parse_expr();
         expect(RPAREN);

         UREG->start = fstart;
         UREG->start->first_label = '_';
         UREG->start->first_neighbor = r1->start;
         UREG->start->second_label = '_';
         UREG->start->second_neighbor = r2->start;

         UREG->accept = faccept;

         r1->accept->first_label = '_';
         r1->accept->first_neighbor = UREG->accept;

         r2->accept->first_label = '_';
         r2->accept->first_neighbor = UREG->accept;


      }
      else if (t2.token_type == STAR)
      {
    	 UREG->start = fstart;
    	 UREG->accept = faccept;
         UREG->start->first_label = '_';
         UREG->start->first_neighbor = r1->start;
         UREG->start->second_label = '_';
         UREG->start->second_neighbor = UREG->accept;



         r1->accept->first_label = '_';
         r1->accept->first_neighbor = r1->start;
         r1->accept->second_label = '_';
         r1->accept->second_neighbor = UREG->accept;
      }
      else
      {
    	  UREG = r1;
      }

   }
   else
   {
      syntax_error();
   }

   return UREG;
}


void Parser::ParseProgram()
{
    parse_input();
    expect(END_OF_FILE);
}

int main()
{
    Parser parser;

    parser.ParseProgram();


}







