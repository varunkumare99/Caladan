#include <map>
#include <memory>

#include "ExpressionAST.h"
#include "FunctionDefinitionAST.h"
#include "Lexer.h"
#define PROMPT "\033[35;1m>\033[0m\033[91;1m>\033[0m\033[93;1m> \033[0m"
class Parser {
private:
  int m_currentToken;
  Lexer m_lexer;
  static std::map<int, int> m_binaryOpPrecedence;

public:
  Parser(Lexer lexer) : m_lexer{lexer} {}
  int getNextToken();
  int getTokPrecedence();

  static std::unique_ptr<ExpressionAST> logError(const char *str);
  static std::unique_ptr<PrototypeAST> logErrorP(const char *str);

  std::unique_ptr<ExpressionAST> parseExpression();
  std::unique_ptr<ExpressionAST> parseNumberExpression();
  std::unique_ptr<ExpressionAST> parseParenExpression();
  std::unique_ptr<ExpressionAST> parseIdentifierExpression();
  std::unique_ptr<ExpressionAST> parsePrimary();
  std::unique_ptr<ExpressionAST> parseIfExpression();
  std::unique_ptr<ExpressionAST>
  parseBinaryOperationRHS(int exprPrec, std::unique_ptr<ExpressionAST> lhs);
  std::unique_ptr<PrototypeAST> parsePrototype();
  std::unique_ptr<FunctionDefinitionAST> parseDefinition();
  std::unique_ptr<FunctionDefinitionAST> parseTopLevelExpr();
  std::unique_ptr<PrototypeAST> parseExtern();

  void handleDefinition();
  void handleExtern();
  void handleTopLevelExpression();
  void mainLoop();
};
