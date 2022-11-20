#include "VariableExpressionAST.h"
#include "CodeGeneration.h"

Value *VariableExpressionAST::codegen() {
  AllocaInst *variable = CodeGeneration::NamedValues[m_name];
  if (!variable)
    return CodeGeneration::logErrorV(
        "unknown variable name, VariableExpressionAST: ");
  return CodeGeneration::Builder->CreateLoad(variable->getAllocatedType(),
                                             variable, m_name.c_str());
}
