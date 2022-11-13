#include "PrototypeAST.h"
#include "CodeGeneration.h"

Function *PrototypeAST::codegen() {
  std::vector<Type *> argumentsVec(
      m_arguments.size(), Type::getInt64Ty(*CodeGeneration::TheContext));

  FunctionType *functionType;
  functionType = FunctionType::get(
      Type::getInt64Ty(*CodeGeneration::TheContext), argumentsVec, false);

  Function *currFunction =
      Function::Create(functionType, Function::ExternalLinkage, m_functionName,
                       CodeGeneration::TheModule.get());

  unsigned idx = 0;
  for (auto &arg : currFunction->args())
    arg.setName(m_arguments[idx++]);
  return currFunction;
}
