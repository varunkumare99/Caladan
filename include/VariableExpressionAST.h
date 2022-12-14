#ifndef VariableExpressionAST_H
#define VariableExpressionAST_H

#include "ExpressionAST.h"
#include <iostream>

class VariableExpressionAST : public ExpressionAST {
private:
  std::string m_name;

public:
  VariableExpressionAST(const std::string &name) : m_name{name} {}
  const std::string &getVariableName() const { return m_name; }
  virtual Value *codegen() override;
};

#endif
