#include "BinaryExpressionAST.h"
#include "CodeGeneration.h"
#include "VariableExpressionAST.h"

Value *BinaryExpressionAST::codegen() {

  if (m_operation == '=') {
    // special case
    VariableExpressionAST *lhsExpr =
        static_cast<VariableExpressionAST *>(m_lhsExpr.get());
    if (!lhsExpr)
      return CodeGeneration::logErrorV(
          "destination of '=' must be a variable ");

    /* emit code to generate rhs Value */
    Value *rhsValue = m_rhsExpr->codegen();
    if (!rhsValue)
      return nullptr;

    // Look up Variable Name
    Value *variable = CodeGeneration::NamedValues[lhsExpr->getVariableName()];
    if (!variable)
      return CodeGeneration::logErrorV("unknown variable name");

    /* load the rhs value onto the variable in the stack */
    CodeGeneration::Builder->CreateStore(rhsValue, variable);
    return rhsValue;
  }

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
