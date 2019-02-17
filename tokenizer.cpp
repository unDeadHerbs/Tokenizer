#include "tokenizer.hpp"
using std::string;
#include <ctype.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
using std::queue;

#define DEBUG 0
#if DEBUG > 0
#include <iostream>
#define DPRINT(x)                                                              \
  (std::cout << "DPRINT: File:" << __FILE__ << " Line:" << __LINE__ << " - "   \
             << x << std::endl)
#else
#define DPRINT(x) (0)
#endif

static FILE *fp;
bool initialize(char *filename) {
  fp = fopen(filename, "rb");
  DPRINT("initialized fp=" << fp);
  return fp != NULL;
}

enum STATE {
  HALT_S = 0,
  START_S,
  ERROR_S,
  WHITESPACE_S,
  OPERATOR_S,
  STRING_S,
  NUMBER_S,
  CHAR_S,
  COMMENT_S,
  COMMENT_BLOCK_S
};

STATE next_state(char tok) {
  DPRINT("getting state for '" << tok << "'");
  switch (tok) {
  case EOF:
    return HALT_S;
  case ' ':
  case '\t':
  case '\n':
    return WHITESPACE_S;
  case '+':
  case '-':
  case '*':
  case '/':
  case '^':
  case '<':
  case '>':
  case '=':
  case '~':
  case '!':
  case '(':
  case ')':
  case '[':
  case ']':
  case '{':
  case '}':
  case '&':
  case '.':
  case '@':
  case ';':
  case ',':
  case ':':
    return OPERATOR_S;
  case '"':
    return STRING_S;
  case '#':
    return COMMENT_S;
  default:
    if ('0' <= tok && tok <= '9')
      return NUMBER_S;
    if ('a' <= tok && tok <= 'z')
      return CHAR_S;
    if ('A' <= tok && tok <= 'Z')
      return CHAR_S;
    if ('_' == tok)
      return CHAR_S;
    return ERROR_S;
  }
}

#define _bitMask(x) ((1 << (x)) - 1)
#define _bitShift(v, x) ((v) >> (x))
#define bitRange(v, u, l) (_bitMask(((u) - (l))) & _bitShift((v), (l)))
string hex_to_unicode(string hex) {
  int len = 0;
  long v = strtol(hex.c_str(), NULL, 16);
  long vt(v);
  while (vt)
    (vt >>= 1, len++);
  if (len <= 7)
    return string() + char(v);
  else if (len <= 5 + 6)
    return string() + char((0b110 << 5) | bitRange(v, 5 + 6, 6)) +
           char((0b10 << 6) | bitRange(v, 6, 0));
  else if (len <= 4 + 6 + 6)
    return string() + char((0b1110 << 4) | bitRange(v, 4 + 6 + 6, 6 + 6)) +
           char((0b10 << 6) | bitRange(v, 6 + 6, 6)) +
           char((0b10 << 6) | bitRange(v, 6, 0));
  else if (len <= 3 + 6 + 6 + 6)
    return string() +
           char((0b11110 << 3) | bitRange(v, 3 + 6 + 6 + 6, 6 + 6 + 6)) +
           char((0b10 << 6) | bitRange(v, 6 + 6 + 6, 6 + 6)) +
           char((0b10 << 6) | bitRange(v, 6 + 6, 6)) +
           char((0b10 << 6) | bitRange(v, 6, 0));
  else
    return "INVALID UTF8 (" + hex + ")";
}
#undef _bitMask
#undef _bitShift
#undef bitRange

Token get_token() {
  /*static*/ STATE state = START_S;
  static int row = 1;
  static int col = 1;
  int r, c;
  string tokstr;
  static char ch;
#define CURCHAR (ch)
#define NEXT_CHAR                                                              \
  (ch = fgetc(fp), tokstr += ch,                                               \
   DPRINT("got char '" << ch << "' from line:" << row << " col:" << col),      \
   DPRINT("Current token is :" << tokstr),                                     \
   ch == '\n' ? (row++, col = 1) : col++, ch)
#define UNGETCHAR() (ungetc(ch, fp), tokstr.pop_back(), ch = *tokstr.rbegin())
  static queue<Token> tok_store;
  while (state)
    switch (state) {
    case START_S:
      DPRINT("Going to state START_S");
      if (tok_store.size()) {
        Token t = tok_store.front();
        tok_store.pop();
        return t;
      }
      tokstr = "";
      r = row;
      c = col;
      state = next_state(NEXT_CHAR);
      break;
    case ERROR_S:
      DPRINT("Going to state ERROR_S");
      return Token(UNKNOWN, r, c, tokstr);
      break;
    case WHITESPACE_S:
      DPRINT("Going to state WHITESPACE_S");
      state = START_S;
      break;
    case OPERATOR_S:
      DPRINT("Going to state OPERATOR_S");
      switch (CURCHAR) {
      case '<':
        switch (NEXT_CHAR) {
        case '=':
          return Token(GREATER_EQ, r, c, tokstr);
        case '-':
          return Token(ASSIGN, r, c, tokstr);
        case '<':
          if ('-' == NEXT_CHAR)
            state = COMMENT_BLOCK_S;
          else {
            UNGETCHAR();
            tok_store.push(Token(TOKEN('<'), r, c + 1, "<"));
            return Token(TOKEN('<'), r, c, "<");
          }
          break;
        default:
          UNGETCHAR();
          return Token(TOKEN('<'), r, c, tokstr);
        }
        break;
      case '>':
        switch (NEXT_CHAR) {
        case '=':
          return Token(LESS_EQ, r, c, tokstr);
        default:
          UNGETCHAR();
          return Token(TOKEN('>'), r, c, tokstr);
        }
      case '~':
        switch (NEXT_CHAR) {
        case '=':
          return Token(NOT_EQUAL, r, c, tokstr);
        default:
          UNGETCHAR();
          return Token(UNKNOWN, r, c, tokstr);
        }
      case '.':
        if (NUMBER_S == next_state(NEXT_CHAR)) {
          UNGETCHAR();
          state = NUMBER_S;
        } else {
          UNGETCHAR();
          return Token(TOKEN(CURCHAR), r, c, tokstr);
        }
        break;
      default:
        return Token(TOKEN(CURCHAR), r, c, tokstr);
      }
      break;
    case STRING_S:
      DPRINT("Going to state STRING_S with char:'" << CURCHAR << "'");
      // TODO: need to hanel escaped sequences
      while ((NEXT_CHAR,
              STRING_S != next_state(CURCHAR) && HALT_S != next_state(CURCHAR)))
        if ('\\' == CURCHAR)
          switch (NEXT_CHAR) {
          case 'n':
            tokstr.pop_back();
            tokstr.pop_back();
            tokstr += '\n';
            break;
          case 't':
            tokstr.pop_back();
            tokstr.pop_back();
            tokstr += '\t';
            break;
          case 'r':
            tokstr.pop_back();
            tokstr.pop_back();
            tokstr += '\r';
            break;
          case '"':
            tokstr.pop_back();
            tokstr.pop_back();
            tokstr += '"';
            break;
          case '\\':
            tokstr.pop_back();
            tokstr.pop_back();
            tokstr += '\\';
            break;
          case 'a':
            tokstr.pop_back();
            tokstr.pop_back();
            tokstr += '\a';
            break;
          case 'b':
            tokstr.pop_back();
            tokstr.pop_back();
            tokstr += '\b';
            break;
          case 'u': {
            tokstr.pop_back();
            tokstr.pop_back();
            string hex;
            NEXT_CHAR;
            if (isxdigit(CURCHAR)) {
              hex += *tokstr.rbegin();
              tokstr.pop_back();
              NEXT_CHAR;
              if (isxdigit(CURCHAR)) {
                hex += *tokstr.rbegin();
                tokstr.pop_back();
                NEXT_CHAR;
                if (isxdigit(CURCHAR)) {
                  hex += *tokstr.rbegin();
                  tokstr.pop_back();
                  NEXT_CHAR;
                  if (isxdigit(CURCHAR)) {
                    hex += *tokstr.rbegin();
                    tokstr.pop_back();
                  } else
                    UNGETCHAR();
                } else
                  UNGETCHAR();
              } else
                UNGETCHAR();
              tokstr += hex_to_unicode(hex);
            } else {
              tok_store.push(Token(ERROR, row, col, "bad Unicode"));
              UNGETCHAR();
              tokstr += "\\u";
            }
          } break;
          default:
            break;
          }
      return Token(STRING, r, c, tokstr);
    case NUMBER_S:
      DPRINT("Going to state NUMBER_S");
      if (CURCHAR != '.') // incase we jumped here from OPERATOR_S
        while (NUMBER_S == next_state(NEXT_CHAR))
          ;
      if (CURCHAR == '.' || CURCHAR == 'e' || CURCHAR == 'E') // it's a real
      {
        if (CURCHAR == '.') // get . and num
          while (NUMBER_S == next_state(NEXT_CHAR))
            ;
        if (CURCHAR == 'e' || CURCHAR == 'E') {
          NEXT_CHAR;
          if (CURCHAR == '-' || CURCHAR == '+') {
            NEXT_CHAR;
            if (NUMBER_S == next_state(CURCHAR)) {
              while (NUMBER_S == next_state(NEXT_CHAR))
                ;
              UNGETCHAR();
            } else {
              // check if . in tokstr and then produce int or real
              // ident e
              // operator +-
              UNGETCHAR();
              tok_store.push(Token(IDENT, row, col - 1,
                                   string() + *(tokstr.rbegin() + 1)));
              tok_store.push(Token(TOKEN(*tokstr.rbegin()), row, col,
                                   string() + *tokstr.rbegin()));
              tokstr.pop_back();
              tokstr.pop_back();
              if (tokstr.find('.') != string::npos)
                return Token(REAL, r, c, tokstr);
              else
                return Token(INT, r, c, tokstr);
            }
          } else if (NUMBER_S == next_state(CURCHAR)) {
            while (NUMBER_S == next_state(NEXT_CHAR))
              ;
            UNGETCHAR();
          } else {
            UNGETCHAR();
            return Token(REAL, r, c, tokstr);
          }
        }
        return Token(REAL, r, c, tokstr);
      }
      if (HALT_S != next_state(CURCHAR))
        UNGETCHAR();
      return Token(INT, r, c, tokstr);
    case CHAR_S:
      DPRINT("Going to state CHAR_S");
      while ((NEXT_CHAR,
              CHAR_S == next_state(CURCHAR) || NUMBER_S == next_state(CURCHAR)))
        ;
      if (HALT_S != next_state(CURCHAR))
        UNGETCHAR();
      return Token(IDENT, r, c, tokstr);
    case COMMENT_S:
      DPRINT("Going to state COMMENT_S");
      while ((NEXT_CHAR, HALT_S != next_state(CURCHAR) && '\n' != CURCHAR))
        ;
      state = START_S;
      break;
    case COMMENT_BLOCK_S:
      DPRINT("Going to state COMMENT_BLOCK_S");
      while ((NEXT_CHAR, HALT_S != next_state(CURCHAR)))
        if ('-' == CURCHAR)
          if ('>' == NEXT_CHAR)
            if ('>' == NEXT_CHAR)
              break;
      state = START_S;
      break;
    case HALT_S:
      DPRINT("Going to state HALT_S");
      return Token(UNKNOWN, r, c, tokstr);
    }
#undef CURCHAR
#undef NEXT_CHAR
  DPRINT("retuning default EOF");
  return Token(EOF_T, r, c, "");
}
bool cleanup() { return 0 == fclose(fp); }
