#include "NumberExpressionAST.h"
#include "CodeGeneration.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"

Value *NumberExpressionAST::codegen() {
  return ConstantInt::get(*CodeGeneration::TheContext, APInt(64, m_val, true));
}
