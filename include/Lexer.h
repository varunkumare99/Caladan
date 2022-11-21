#ifndef LEXER_H
#define LEXER_H

#include <iostream>
class Lexer {

private:
  std::string m_identifierStr;
  double m_numVal;
  char m_lastChar{' '};

public:
  enum Token {
    tok_eof = -1,

    tok_def = -2,
    tok_extern = -3,

    tok_identifier = -4,
    tok_number = -5,

    tok_if = -6,
    tok_else = -7,

    tok_for = -8,

    tok_var = -9,

    tok_switch = -10,
    tok_case = -11,
    tok_default = -12,

    tok_while = -13
  };

  int getToken();
  double getNumVal();
  std::string getIdentifierStr();
};

#endif
