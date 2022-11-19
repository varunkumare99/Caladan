#ifndef SwitchExpressionAST_H
#define SwitchExpressionAST_H

#include "ExpressionAST.h"

class SwitchExpressionAST : public ExpressionAST {
private:
  std::unique_ptr<ExpressionAST> m_condExpr, m_defaultExpr;
  std::vector<std::pair<int, std::unique_ptr<ExpressionAST>>> m_casesExpr;

public:
  SwitchExpressionAST(
      std::unique_ptr<ExpressionAST> condExpr,
      std::unique_ptr<ExpressionAST> defaultExpr,
      std::vector<std::pair<int, std::unique_ptr<ExpressionAST>>> casesExpr)
      : m_condExpr(std::move(condExpr)), m_defaultExpr(std::move(defaultExpr)),
        m_casesExpr(std::move(casesExpr)) {}

  Value *codegen() override;
};
#endif
