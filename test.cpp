#include <iostream>
using std::cout;
using std::endl;
#include "tokenizer.hpp"
using std::ostream;
using std::string;
#include <iomanip>
using std::setw;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " [Filename]" << endl;
    return 0;
  }
  cout << "Starting tokenizer test." << endl
       << endl
       << "| Token Type | Token Value" << endl
       << "+------------+-- - - -  -  -" << endl;
  if (!initialize(argv[1]))
    exit(-1);
  while (Token t = get_token())
    cout << t << endl;
  cleanup();
  cout << "+------------+-- - - -  -  -" << endl;
}

string TOKEN_name(TOKEN t) {
#define echocase(x)                                                            \
  case x:                                                                      \
    return #x
  switch (t) {
    echocase(EOF_T);
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
  return o << "|" << setw(11) << TOKEN_name(t.id) << " | '" << t.text << "' at "
           << t.line << ":" << t.col;
}
