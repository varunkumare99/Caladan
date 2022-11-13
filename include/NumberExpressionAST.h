#ifndef NumberExpressionAST_H
#define NumberExpressionAST_H

#include "ExpressionAST.h"

class NumberExpressionAST : public ExpressionAST {
private:
  int m_val;

public:
  NumberExpressionAST(int val) : m_val(val) {}
  Value *codegen() override;
};

#endif
