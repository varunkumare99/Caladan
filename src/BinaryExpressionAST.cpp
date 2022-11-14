#include "BinaryExpressionAST.h"
#include "CodeGeneration.h"

Value *BinaryExpressionAST::codegen() {
  Value *lhs = m_lhsExpr->codegen();
  Value *rhs = m_rhsExpr->codegen();

  if (!lhs || !rhs)
    return nullptr;

  switch (m_operation) {
  case '+':
    return CodeGeneration::Builder->CreateAdd(lhs, rhs, "addtmp");
  case '-':
    return CodeGeneration::Builder->CreateSub(lhs, rhs, "subtmp");
  case '*':
    return CodeGeneration::Builder->CreateMul(lhs, rhs, "multmp");
  case '/':
    return CodeGeneration::Builder->CreateSDiv(lhs, rhs, "divtmp");
  case '%':
    return CodeGeneration::Builder->CreateSRem(lhs, rhs, "remdtmp");
  case '<':
    lhs = CodeGeneration::Builder->CreateICmpULT(lhs, rhs, "cmpltmp");
    return CodeGeneration::Builder->CreateZExt(
        lhs, Type::getInt64Ty(*CodeGeneration::TheContext), "booltmp");
  case '>':
    lhs = CodeGeneration::Builder->CreateICmpUGT(lhs, rhs, "cmpgtmp");
    return CodeGeneration::Builder->CreateZExt(
        lhs, Type::getInt64Ty(*CodeGeneration::TheContext), "booltmp");
  default:
    return CodeGeneration::logErrorV("invalid binary operator");
  }
}
