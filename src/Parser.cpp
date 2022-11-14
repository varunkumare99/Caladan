#include "Parser.h"
#include "BinaryExpressionAST.h"
#include "CallExpressionAST.h"
#include "CodeGeneration.h"
#include "ForExpressionAST.h"
#include "FunctionDefinitionAST.h"
#include "IfExpressionAST.h"
#include "NumberExpressionAST.h"
#include "PrototypeAST.h"
#include "VariableExpressionAST.h"
#include <memory>

std::map<int, int> Parser::m_binaryOpPrecedence{
    {'>', 20}, {'<', 20}, {'+', 20}, {'-', 20}, {'*', 40}, {'/', 40}};

int Parser::getNextToken() { return m_currentToken = m_lexer.getToken(); }

int Parser::getTokPrecedence() {
  if (!isascii(m_currentToken))
    return -1;

  int tokPrecedence = m_binaryOpPrecedence[m_currentToken];

  if (tokPrecedence <= 0)
    return -1;
  return tokPrecedence;
}

std::unique_ptr<ExpressionAST> Parser::logError(const char *str) {
  fprintf(stderr, "Error: %s\n", str);
  return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::logErrorP(const char *str) {
  logError(str);
  return nullptr;
}

std::unique_ptr<ExpressionAST> Parser::parseNumberExpression() {
  auto result = std::make_unique<NumberExpressionAST>(m_lexer.getNumVal());
  getNextToken(); // consume the number
  return std::move(result);
}

std::unique_ptr<ExpressionAST> Parser::parseParenExpression() {
  getNextToken(); // consume '('
  auto expr = parseExpression();
  if (!expr)
    return nullptr;

  if (m_currentToken != ')')
    return logError("expected ')'");
  getNextToken(); // consume ')'
  return expr;
}

std::unique_ptr<ExpressionAST> Parser::parseIdentifierExpression() {
  std::string idName = m_lexer.getIdentifierStr();

  getNextToken(); // consume identifier
  if (m_currentToken != '(')
    return std::make_unique<VariableExpressionAST>(idName);

  // function call
  getNextToken(); // consume '('
  std::vector<std::unique_ptr<ExpressionAST>> args;
  if (m_currentToken != ')') {
    while (true) {
      if (auto arg = parseExpression())
        args.push_back(std::move(arg));
      else
        return nullptr;

      if (m_currentToken == ')')
        break;

      if (m_currentToken != ',')
        return logError("Expected ')' or ',' in argument list");
      getNextToken();
    }
  }
  getNextToken(); // consume ')'
  return std::make_unique<CallExpressionAST>(idName, std::move(args));
}

std::unique_ptr<ExpressionAST>
Parser::parseBinaryOperationRHS(int exprPrec,
                                std::unique_ptr<ExpressionAST> lhsExpr) {
  while (true) {
    int tokPrecedence = getTokPrecedence();

    if (tokPrecedence < exprPrec)
      return lhsExpr;

    int binop = m_currentToken;
    getNextToken(); // consume binop

    auto rhsExpr = parsePrimary();
    if (!rhsExpr)
      return nullptr;

    int nextPrec = getTokPrecedence();
    if (tokPrecedence < nextPrec) {
      rhsExpr = parseBinaryOperationRHS(tokPrecedence + 1, std::move(rhsExpr));
      if (!rhsExpr)
        return nullptr;
    }
    lhsExpr = std::make_unique<BinaryExpressionAST>(binop, std::move(lhsExpr),
                                                    std::move(rhsExpr));
  }
}

std::unique_ptr<ExpressionAST> Parser::parsePrimary() {
  switch (m_currentToken) {
  default:
    return logError("unknown token when expecting an expression");
  case Lexer::tok_identifier:
    return parseIdentifierExpression();
  case Lexer::tok_number:
    return parseNumberExpression();
  case '(':
    return parseParenExpression();
  case Lexer::tok_if:
    return parseIfExpression();
  case Lexer::tok_for:
    return parseForExpression();
  }
}
std::unique_ptr<ExpressionAST> Parser::parseExpression() {
  auto lhsExpr = parsePrimary();
  if (!lhsExpr)
    return nullptr;
  return parseBinaryOperationRHS(0, std::move(lhsExpr));
}

std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
  if (m_currentToken != Lexer::tok_identifier)
    return logErrorP("Expected function name in prototype");

  std::string functionName = m_lexer.getIdentifierStr();
  getNextToken();

  if (m_currentToken != '(')
    return logErrorP("Expected '(' in prototype");

  std::vector<std::string> argNames;
  while (getNextToken() == Lexer::tok_identifier) {
    argNames.push_back(m_lexer.getIdentifierStr());
  }
  if (m_currentToken != ')')
    return logErrorP("Expected ')' in prototype");

  getNextToken(); // consume ')'
  return std::make_unique<PrototypeAST>(functionName, std::move(argNames));
}

std::unique_ptr<ExpressionAST> Parser::parseForExpression() {
  getNextToken(); // consume 'for'

  if (m_currentToken != Lexer::tok_identifier)
    return logError("expected identifier after for");

  std::string varName = m_lexer.getIdentifierStr();
  getNextToken(); // consume 'identifier'

  if (m_currentToken != '=')
    return logError("expected '=' after for");
  getNextToken(); // consume '='

  auto startExpr = parseExpression();
  if (!startExpr)
    return nullptr;

  if (m_currentToken != ',')
    return logError("expeted ',' after the start value");
  getNextToken(); // consume ','

  auto condExpr = parseExpression();
  if (!condExpr)
    return nullptr;

  if (m_currentToken != ',')
    return logError("expeted ',' after the condition");
  getNextToken(); // consume ';'

  auto stepExpr = parseExpression();
  if (!stepExpr)
    return nullptr;

  if (m_currentToken != '{')
    return logError("expected '}' at beginning of loop body");
  getNextToken(); // consume '{'

  auto bodyExpr = parseExpression();
  if (!bodyExpr)
    return nullptr;

  return std::make_unique<ForExpressionsAST>(
      varName, std::move(startExpr), std::move(condExpr), std::move(stepExpr),
      std::move(bodyExpr));
}

std::unique_ptr<ExpressionAST> Parser::parseIfExpression() {
  getNextToken(); // eat the if

  auto condExpr = parseExpression();
  if (!condExpr)
    return nullptr;

  if (m_currentToken != '{')
    return logError("Expected '{' after if condition");
  getNextToken(); // consume '{'

  auto ifExpr = parseExpression();
  if (!ifExpr)
    return nullptr;

  if (m_currentToken != '}')
    return logError("Expected '}' after if condition");
  getNextToken(); // consume '}'

  if (m_currentToken != Lexer::tok_else)
    return logError("Expected else");
  getNextToken(); // consume 'else'

  if (m_currentToken != '{')
    return logError("Expected '{' after if condition");
  getNextToken(); // consume '{'

  auto elseExpr = parseExpression();
  if (!elseExpr)
    return nullptr;

  /* if (m_currentToken != '}') */
  /* 	return logError("Expected '}' after if condition"); */

  return std::make_unique<IfExpressionAST>(
      std::move(condExpr), std::move(ifExpr), std::move(elseExpr));
}

std::unique_ptr<FunctionDefinitionAST> Parser::parseDefinition() {
  getNextToken();
  auto proto = parsePrototype();
  if (!proto)
    return nullptr;

  getNextToken(); // consume '{'
  auto expr = parseExpression();
  /* getNextToken(); //consume '}' */

  if (expr)
    return std::make_unique<FunctionDefinitionAST>(std::move(proto),
                                                   std::move(expr));
  return nullptr;
}

std::unique_ptr<FunctionDefinitionAST> Parser::parseTopLevelExpr() {
  if (auto expr = parseExpression()) {
    auto proto = std::make_unique<PrototypeAST>("__anon_expr",
                                                std::vector<std::string>());
    return std::make_unique<FunctionDefinitionAST>(std::move(proto),
                                                   std::move(expr));
  }
  return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::parseExtern() {
  getNextToken();
  return parsePrototype();
}

void Parser::handleDefinition() {
  if (auto functionAST = parseDefinition()) {
    if (auto *functionIR = functionAST->codegen()) {
      fprintf(stderr, "Parsed a function definition\n");
      functionIR->print(errs());
      fprintf(stderr, "\n");
      CodeGeneration::ExitOnErr(CodeGeneration::TheJIT->addModule(
          ThreadSafeModule(std::move(CodeGeneration::TheModule),
                           std::move(CodeGeneration::TheContext))));
      CodeGeneration::initializeModuleAndPassManager();
    }
  } else {
    getNextToken();
  }
}

void Parser::handleExtern() {
  if (auto prototypeAST = parseExtern()) {
    if (auto *functionIR = prototypeAST->codegen()) {
      fprintf(stderr, "Parsed an extern\n");
      functionIR->print(errs());
      fprintf(stderr, "\n");
      CodeGeneration::FunctionProtos[prototypeAST->getFunctionName()] =
          std::move(prototypeAST);
    }
  } else {
    getNextToken();
  }
}

void Parser::handleTopLevelExpression() {
  if (auto functionAST = parseTopLevelExpr()) {
    if (auto *functionIR = functionAST->codegen()) {
      fprintf(stderr, "Parsed a top-level expr\n");
      functionIR->print(errs());
      fprintf(stderr, "\n");

      auto resourceTracker =
          CodeGeneration::TheJIT->getMainJITDylib().createResourceTracker();
      auto threadSafeModule =
          ThreadSafeModule(std::move(CodeGeneration::TheModule),
                           std::move(CodeGeneration::TheContext));
      CodeGeneration::ExitOnErr(CodeGeneration::TheJIT->addModule(
          std::move(threadSafeModule), resourceTracker));
      CodeGeneration::initializeModuleAndPassManager();

      auto exprSymbol = CodeGeneration::ExitOnErr(
          CodeGeneration::TheJIT->lookup("__anon_expr"));

      int (*functionPtr)() = (int (*)())(intptr_t)exprSymbol.getAddress();
      fprintf(stderr, "\033[91;1mResult :\033[0m %d\n", functionPtr());

      CodeGeneration::ExitOnErr(resourceTracker->remove());
    }
  } else {
    getNextToken();
  }
}

void Parser::mainLoop() {
  while (true) {
    fprintf(stderr, PROMPT);
    switch (m_currentToken) {
    case Lexer::tok_eof:
      return;
    case '}':
    case ';':
      getNextToken(); // consume '}'
      break;
    case Lexer::tok_def:
      handleDefinition();
      break;
    case Lexer::tok_extern:
      handleExtern();
      break;
    default:
      handleTopLevelExpression();
      break;
    }
  }
}
