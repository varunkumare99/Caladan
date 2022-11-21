#include "Parser.h"
#include "BinaryExpressionAST.h"
#include "CallExpressionAST.h"
#include "CodeGeneration.h"
#include "ForExpressionAST.h"
#include "FunctionDefinitionAST.h"
#include "IfExpressionAST.h"
#include "NumberExpressionAST.h"
#include "PrototypeAST.h"
#include "SwitchExpressionAST.h"
#include "VarExpressionAST.h"
#include "VariableExpressionAST.h"
#include "WhileExpressionAST.h"
#include <memory>

std::map<int, int> Parser::m_binaryOpPrecedence{{'=', 2},  {'>', 20}, {'<', 20},
                                                {'+', 20}, {'-', 20}, {'*', 40},
                                                {'/', 40}, {'%', 20}};

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
  case Lexer::tok_var:
    return parseVarExpression();
  case Lexer::tok_switch:
    return parseSwitchExpression();
  case Lexer::tok_while:
    return parseWhileExpression();
  }
}

std::unique_ptr<ExpressionAST> Parser::parseExpression() {
  auto lhsExpr = parsePrimary();
  if (!lhsExpr)
    return nullptr;
  return parseBinaryOperationRHS(0, std::move(lhsExpr));
}

std::unique_ptr<ExpressionListAST> Parser::parseExpressionList() {
  ExpressionListAST expressionListAST;
  std::unique_ptr<ExpressionAST> currExpr = parseExpression();
  expressionListAST.expressionList.push_back(std::move(currExpr));
  while (m_currentToken == ':') {
    getNextToken(); // consume ':'
    currExpr = parseExpression();
    expressionListAST.expressionList.push_back(std::move(currExpr));
  }
  return std::make_unique<ExpressionListAST>(std::move(expressionListAST));
}

std::unique_ptr<ExpressionAST> Parser::parseSwitchExpression() {
  getNextToken(); // consume 'switch'

  if (m_currentToken != '(')
    return logError("expected '(' after switch");
  getNextToken(); // consume '('

  auto switchCondExpression = parseExpression();
  if (m_currentToken != ')')
    return logError("expected ')' after switch cond");
  getNextToken(); // consume '('

  if (m_currentToken != '{')
    return logError("expected '{' after switch cond");
  getNextToken(); // consume '{'

  std::vector<std::pair<int, std::unique_ptr<ExpressionListAST>>> casesExpr;
  while (m_currentToken == Lexer::tok_case) {
    getNextToken(); // consume 'case'
    if (m_currentToken != Lexer::tok_number)
      return logError("expected integer in case val");

    int caseVal = m_lexer.getNumVal();
    getNextToken(); // consume integer

    if (m_currentToken != ':')
      return logError("expected ':' after case condition");
    getNextToken(); // consume ':'

    auto caseExpr = parseExpressionList();
    if (!caseExpr)
      return nullptr;
    casesExpr.push_back(std::make_pair(caseVal, std::move(caseExpr)));
  }

  std::unique_ptr<ExpressionAST> defaultExpr;
  if (m_currentToken != Lexer::tok_default)
    return logError("expected default in switch ");
  else {
    getNextToken(); // consume 'default'
    if (m_currentToken != ':')
      return logError("expected ':' after default");
    getNextToken(); // consume ':'
    defaultExpr = parseExpression();
  }

  getNextToken(); // consume '}' end of var
  return std::make_unique<SwitchExpressionAST>(std::move(switchCondExpression),
                                               std::move(defaultExpr),
                                               std::move(casesExpr));
}

std::unique_ptr<ExpressionAST> Parser::parseVarExpression() {
  getNextToken(); // consume 'var'

  std::vector<std::pair<std::string, std::unique_ptr<ExpressionAST>>> varNames;

  // At least one variable name is required
  if (m_currentToken != Lexer::tok_identifier)
    return logError("expected identifier after var");

  while (true) {
    std::string varName = m_lexer.getIdentifierStr();
    getNextToken(); // consume 'identifier'

    // Read the optional initializer
    std::unique_ptr<ExpressionAST> initExpr = nullptr;
    if (m_currentToken == '=') {
      getNextToken(); // consume '='

      initExpr = parseExpression();
      if (!initExpr)
        return nullptr;
    }

    varNames.push_back(std::make_pair(varName, std::move(initExpr)));

    // End of var list, exit loop
    if (m_currentToken != ',')
      break;
    getNextToken(); // eat the ','

    if (m_currentToken != Lexer::tok_identifier)
      return logError("expected identifier list after var");
  }

  if (m_currentToken != '{')
    return logError("expected '{' after var declaration");
  getNextToken(); // consume '{'

  auto bodyExpr = parseExpressionList();
  if (bodyExpr->expressionList.size() == 0)
    return nullptr;

  getNextToken(); // consume '}' end of var
  return std::make_unique<VarExpressionsAST>(std::move(varNames),
                                             std::move(bodyExpr));
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

  auto bodyExpr = parseExpressionList();
  if (bodyExpr->expressionList.size() == 0)
    return nullptr;

  getNextToken(); // consume '}'
  return std::make_unique<ForExpressionsAST>(
      varName, std::move(startExpr), std::move(condExpr), std::move(stepExpr),
      std::move(bodyExpr));
}

std::unique_ptr<ExpressionAST> Parser::parseWhileExpression() {
  getNextToken(); // consume 'while'

  auto whileCondExpr = parseExpression();
  if (!whileCondExpr)
    return nullptr;

  if (m_currentToken != '{')
    return logError("expected '{' after while condition");
  getNextToken(); // consume '{'

  auto whileBodyExpr = parseExpressionList();
  if (whileBodyExpr->expressionList.size() == 0)
    return nullptr;
  if (m_currentToken != '}')
    return logError("exprected '}' end while body");
  getNextToken(); // consume '}'

  return std::make_unique<WhileExpressionAST>(std::move(whileCondExpr),
                                              std::move(whileBodyExpr));
}

std::unique_ptr<ExpressionAST> Parser::parseIfExpression() {
  getNextToken(); // eat the if

  auto condExpr = parseExpression();
  if (!condExpr)
    return nullptr;

  if (m_currentToken != '{')
    return logError("Expected '{' after if condition");
  getNextToken(); // consume '{'

  auto ifExpr = parseExpressionList();
  if (ifExpr->expressionList.size() == 0)
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

  auto elseExpr = parseExpressionList();
  if (elseExpr->expressionList.size() == 0)
    return nullptr;

  getNextToken(); // consume '}'
  return std::make_unique<IfExpressionAST>(
      std::move(condExpr), std::move(ifExpr), std::move(elseExpr));
}

std::unique_ptr<FunctionDefinitionAST> Parser::parseDefinition() {
  getNextToken();
  auto proto = parsePrototype();
  if (!proto)
    return nullptr;

  getNextToken(); // consume '{'
  auto expr = parseExpressionList();
  /* getNextToken(); //consume '}' */

  if (expr->expressionList.size())
    return std::make_unique<FunctionDefinitionAST>(std::move(proto),
                                                   std::move(expr));
  return nullptr;
}

std::unique_ptr<FunctionDefinitionAST> Parser::parseTopLevelExpr() {
  ExpressionListAST expressionListAST;
  if (auto expr = parseExpression()) {
    expressionListAST.expressionList.push_back(std::move(expr));
    auto proto = std::make_unique<PrototypeAST>("__anon_expr",
                                                std::vector<std::string>());
    return std::make_unique<FunctionDefinitionAST>(
        std::move(proto),
        std::make_unique<ExpressionListAST>(std::move(expressionListAST)));
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
