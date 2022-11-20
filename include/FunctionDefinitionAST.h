#ifndef FunctionDefinitionAST_H
#define FunctionDefinitionAST_H

#include "ExpressionAST.h"
#include "PrototypeAST.h"
#include <memory>

class FunctionDefinitionAST {
private:
  std::unique_ptr<PrototypeAST> m_prototype;
  std::unique_ptr<ExpressionListAST> m_body;

public:
  FunctionDefinitionAST(std::unique_ptr<PrototypeAST> proto,
                        std::unique_ptr<ExpressionListAST> body)
      : m_prototype{std::move(proto)}, m_body{std::move(body)} {}
  Function *codegen();
};

#endif
