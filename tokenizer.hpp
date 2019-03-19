#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__
#include <ostream>
#include <string>

enum TOKEN {
	    EOF_T	= 0 ,
	    UNKNOWN	=-1 ,
	    ERROR	=-2 ,
	    PLUS	='+',
	    MINUS	='-',
	    MULT	='*',
	    DIV		='/',
	    EXP		='^',
	    LESS	='<',
	    LESS_EQ	= 1 , // <=
	    GREATER	='>',
	    GREATER_EQ	= 2, // >=
	    EQUAL	='=',
	    NOT_EQUAL	= 3 , // ~=
	    ASSIGN	= 4 , // <-
	    NOT		='!',
	    LPAREN	='(',
	    RPAREN	=')',
	    LBRACE	='{',
	    RBRACE	='}',
	    LBRACKET	='[',
	    RBRACKET	=']',
	    AND		='&',
	    OR		='|',
	    DOT		='.',
	    AT		='@',
	    COLON	=':',
	    SEMICOLON	=';',
	    COMMA	=',',
	    IDENT	= 5 ,
	    INT		= 6 ,
	    STRING	= 7 ,
	    REAL	= 8 };

class Token{
  TOKEN id;
  int line;
  int col;
  std::string text;
public:
  friend std::ostream& operator<<(std::ostream &o, Token t);
  Token(TOKEN t,int l,int c,std::string tx):id(t),line(l),col(c),text(tx){}
  operator bool()const{return id!=EOF_T;
  }
};
bool initialize(char* filename);
Token get_token();
bool cleanup();
#endif

