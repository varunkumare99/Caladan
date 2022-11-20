#ifndef ExpressionAST_H
#define ExpressionAST_H

#include "llvm/IR/Value.h"
using namespace llvm;
class ExpressionAST {
public:
  virtual ~ExpressionAST() = default;
  virtual Value *codegen() = 0;
};

struct ExpressionListAST {
  std::vector<std::unique_ptr<ExpressionAST>> expressionList;
};
#endif
