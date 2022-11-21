#ifndef WhileExpressionAST_H
#define WhileExpressionAST_H

#include "ExpressionAST.h"

class WhileExpressionAST : public ExpressionAST {
private:
  std::unique_ptr<ExpressionAST> m_condExpr;
  std::unique_ptr<ExpressionListAST> m_bodyExpr;

public:
  WhileExpressionAST(std::unique_ptr<ExpressionAST> condExpr,
                     std::unique_ptr<ExpressionListAST> bodyExpr)
      : m_condExpr(std::move(condExpr)), m_bodyExpr(std::move(bodyExpr)) {}

  Value *codegen() override;
};
#endif
