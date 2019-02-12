#include <ostream>
#include <string>

#undef EOF
enum TOKEN {
	    EOF		= 0 ,
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
  TOKEN id=EOF;
  int line=0;
  int col=0;
  std::string text;
public:
  friend std::ostream& operator<<(std::ostream &o, Token t);
  operator bool()const{return id!=EOF;
  }
};
bool initialize(char* filename);
Token get_token();
bool cleanup();

