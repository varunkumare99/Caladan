#ifndef IFExpressionAST_H
#define IFExpressionAST_H

#include "ExpressionAST.h"
#include <algorithm>
#include <memory>
class IfExpressionAST : public ExpressionAST {
private:
  std::unique_ptr<ExpressionAST> m_condExpr, m_ifExpr, m_elseExpr;

public:
  IfExpressionAST(std::unique_ptr<ExpressionAST> condExpr,
                  std::unique_ptr<ExpressionAST> ifExpr,
                  std::unique_ptr<ExpressionAST> elseExpr)
      : m_condExpr(std::move(condExpr)), m_ifExpr(std::move(ifExpr)),
        m_elseExpr(std::move(elseExpr)) {}

  Value *codegen() override;
};

#endif
