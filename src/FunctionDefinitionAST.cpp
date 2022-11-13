#include "FunctionDefinitionAST.h"
#include "CodeGeneration.h"
#include "llvm/IR/Verifier.h"

Function *FunctionDefinitionAST::codegen() {
  auto &prototype = *m_prototype;
  CodeGeneration::FunctionProtos[m_prototype->getFunctionName()] =
      std::move(m_prototype);
  Function *currFunction =
      CodeGeneration::getFunction(prototype.getFunctionName());

  if (!currFunction)
    return nullptr;

  BasicBlock *basicBlock =
      BasicBlock::Create(*CodeGeneration::TheContext, "entry", currFunction);
  CodeGeneration::Builder->SetInsertPoint(basicBlock);

  CodeGeneration::NamedValues.clear();
  for (auto &arg : currFunction->args())
    CodeGeneration::NamedValues[std::string(arg.getName())] = &arg;

  if (Value *retVal = m_body->codegen()) {
    CodeGeneration::Builder->CreateRet(retVal);
    verifyFunction(*currFunction);
    // optimizer
    CodeGeneration::TheFPM->run(*currFunction);
    return currFunction;
  }

  currFunction->eraseFromParent();
  return nullptr;
}
