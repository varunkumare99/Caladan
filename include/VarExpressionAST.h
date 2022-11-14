#ifndef VarExpressionAST_H
#define VarExpressionAST_H

#include "ExpressionAST.h"

class VarExpressionsAST : public ExpressionAST {
private:
  std::vector<std::pair<std::string, std::unique_ptr<ExpressionAST>>>
      m_varNames;
  std::unique_ptr<ExpressionAST> m_varBody;

public:
  VarExpressionsAST(
      std::vector<std::pair<std::string, std::unique_ptr<ExpressionAST>>>
          varNames,
      std::unique_ptr<ExpressionAST> varBody)
      : m_varNames(std::move(varNames)), m_varBody(std::move(varBody)) {}

  Value *codegen() override;
};
#endif
