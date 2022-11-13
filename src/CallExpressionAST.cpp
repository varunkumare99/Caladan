#include "CallExpressionAST.h"
#include "CodeGeneration.h"

Value *CallExpressionAST::codegen() {
  Function *CalleeF = CodeGeneration::getFunction(m_functionCallee);
  if (!CalleeF)
    return CodeGeneration::logErrorV("Unknown function referenced");

  if (CalleeF->arg_size() != m_arguments.size())
    return CodeGeneration::logErrorV("Incorrect #arguments passed");

  std::vector<Value *> argsV;
  for (unsigned i = 0, e = m_arguments.size(); i != e; ++i) {
    argsV.push_back(m_arguments[i]->codegen());
    if (!argsV.back())
      return nullptr;
  }

  return CodeGeneration::Builder->CreateCall(CalleeF, argsV, "calltmp");
}
