#include "Lexer.h"

int Lexer::getToken() {

  while (isspace(m_lastChar))
    m_lastChar = getchar();

  if (isalpha(m_lastChar)) {
    m_identifierStr = m_lastChar;
    while (isalnum((m_lastChar = getchar())))
      m_identifierStr += m_lastChar;

    if (m_identifierStr == "func")
      return tok_def;
    if (m_identifierStr == "extern")
      return tok_extern;
    if (m_identifierStr == "if")
      return tok_if;
    if (m_identifierStr == "else")
      return tok_else;
    if (m_identifierStr == "for")
      return tok_for;
    if (m_identifierStr == "var")
      return tok_var;
    if (m_identifierStr == "switch")
      return tok_switch;
    if (m_identifierStr == "case")
      return tok_case;
    if (m_identifierStr == "default")
      return tok_default;
    return tok_identifier;
  }

  if (isdigit(m_lastChar) || m_lastChar == '.') {
    std::string numericStr;

    do {
      numericStr += m_lastChar;
      m_lastChar = getchar();
    } while (isdigit(m_lastChar) || m_lastChar == '.');

    m_numVal = strtod(numericStr.c_str(), nullptr);
    return tok_number;
  }

  if (m_lastChar == '#') {
    do {
      m_lastChar = getchar();
    } while (m_lastChar != EOF && m_lastChar != '\n' && m_lastChar != '\r');

    if (m_lastChar != EOF)
      return getToken();
  }

  if (m_lastChar == EOF)
    return tok_eof;

  int currentChar = m_lastChar;
  m_lastChar = getchar();
  return currentChar;
}

double Lexer::getNumVal() { return m_numVal; }

std::string Lexer::getIdentifierStr() { return m_identifierStr; }
