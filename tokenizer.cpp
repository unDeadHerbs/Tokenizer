#include "tokenizer.hpp"
#include <fstream>
using std::ifstream;
using std::ostream;
using std::string;
#include <iomanip>
using std::setw;

static ifstream file;
bool initialize(char *filename) {
  file.open(filename, std::ios::in);
  return true;
}
Token get_token() {
  Token ret;
  return ret;
}
bool cleanup() { return true; }

string TOKEN_name(TOKEN t) {
#define echocase(x)                                                            \
  case x:                                                                      \
    return #x
  switch (t) {
    echocase(EOF);
    echocase(UNKNOWN);
    echocase(ERROR);
    echocase(PLUS);
    echocase(MINUS);
    echocase(MULT);
    echocase(DIV);
    echocase(EXP);
    echocase(LESS);
    echocase(LESS_EQ);
    echocase(GREATER);
    echocase(GREATER_EQ);
    echocase(EQUAL);
    echocase(NOT_EQUAL);
    echocase(ASSIGN);
    echocase(NOT);
    echocase(LPAREN);
    echocase(RPAREN);
    echocase(LBRACE);
    echocase(RBRACE);
    echocase(LBRACKET);
    echocase(RBRACKET);
    echocase(AND);
    echocase(OR);
    echocase(DOT);
    echocase(AT);
    echocase(COLON);
    echocase(SEMICOLON);
    echocase(COMMA);
    echocase(IDENT);
    echocase(INT);
    echocase(STRING);
    echocase(REAL);
#undef echocase
  default:
    return "bad token";
  }
}

ostream &operator<<(ostream &o, Token t) {
  return o << setw(20) << TOKEN_name(t.id) << " | token " << t.text << " at "
           << t.line << ":" << t.col;
}

bool initialize(char *filename);
Token get_token();
bool cleanup();
