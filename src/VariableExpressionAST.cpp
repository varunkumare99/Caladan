#include "VariableExpressionAST.h"
#include "CodeGeneration.h"

Value *VariableExpressionAST::codegen() {
  Value *variable = CodeGeneration::NamedValues[m_name];
  if (!variable)
    return CodeGeneration::logErrorV("unknown variable namn");
  return variable;
}
