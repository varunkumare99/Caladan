#ifndef BinaryExpressionAST_H
#define BinaryExpressionAST_H

#include "ExpressionAST.h"
#include <iostream>
#include <memory>

class BinaryExpressionAST : public ExpressionAST {
private:
  char m_operation;
  std::unique_ptr<ExpressionAST> m_lhsExpr, m_rhsExpr;

public:
  BinaryExpressionAST(char opr, std::unique_ptr<ExpressionAST> lhs,
                      std::unique_ptr<ExpressionAST> rhs)
      : m_operation{opr}, m_lhsExpr{std::move(lhs)}, m_rhsExpr{std::move(rhs)} {
  }
  Value *codegen() override;
};

#endif
