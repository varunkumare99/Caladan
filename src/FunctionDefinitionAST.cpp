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
  for (auto &arg : currFunction->args()) {
    // Create an alloca for this variable
    AllocaInst *alloca =
        CodeGeneration::createEntryBlockAlloca(currFunction, arg.getName());

    // Store the initValue into the variable
    CodeGeneration::Builder->CreateStore(&arg, alloca);

    // add to symbol table
    CodeGeneration::NamedValues[std::string(arg.getName())] = alloca;
  }

  for (int i = 0; i < m_body->expressionList.size(); ++i) {
    if ((i + 1) < m_body->expressionList.size()) {
      m_body->expressionList[i]->codegen();
    } else if (Value *retVal = m_body->expressionList[i]->codegen()) {
      CodeGeneration::Builder->CreateRet(retVal);
      verifyFunction(*currFunction);
      // optimizer
      CodeGeneration::TheFPM->run(*currFunction);
      return currFunction;
    }
  }
  currFunction->eraseFromParent();
  return nullptr;
}
