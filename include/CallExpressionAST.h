#ifndef CallExpressionAST_H
#define CallExpressionAST_H

#include "ExpressionAST.h"
#include <iostream>
#include <memory>
#include <vector>

class CallExpressionAST : public ExpressionAST {
private:
  std::string m_functionCallee;
  std::vector<std::unique_ptr<ExpressionAST>> m_arguments;

public:
  CallExpressionAST(const std::string &callee,
                    std::vector<std::unique_ptr<ExpressionAST>> args)
      : m_functionCallee{callee}, m_arguments{std::move(args)} {}
  Value *codegen() override;
};

#endif
