#include "CodeGeneration.h"
#include "Parser.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include <memory>

namespace CodeGeneration {
std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::Module> TheModule;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::map<std::string, llvm::AllocaInst *> NamedValues;
std::unique_ptr<legacy::FunctionPassManager> TheFPM;
std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
std::unique_ptr<KaleidoscopeJIT> TheJIT;
ExitOnError ExitOnErr;

Value *logErrorV(const char *str) {
  Parser::logError(str);
  return nullptr;
}

Function *getFunction(std::string Name) {
  if (auto *currFunction = TheModule->getFunction(Name))
    return currFunction;

  auto functionGen = FunctionProtos.find(Name);
  if (functionGen != FunctionProtos.end())
    return functionGen->second->codegen();

  return nullptr;
}

void initializeModuleAndPassManager(void) {
  CodeGeneration::TheContext = std::make_unique<LLVMContext>();
  CodeGeneration::TheModule =
      std::make_unique<Module>("Caladan ", *CodeGeneration::TheContext);
  CodeGeneration::TheModule->setDataLayout(TheJIT->getDataLayout());

  CodeGeneration::Builder =
      std::make_unique<IRBuilder<>>(*CodeGeneration::TheContext);
  CodeGeneration::TheFPM = std::make_unique<legacy::FunctionPassManager>(
      CodeGeneration::TheModule.get());

  CodeGeneration::TheFPM->add(createInstructionCombiningPass());
  CodeGeneration::TheFPM->add(createReassociatePass());
  CodeGeneration::TheFPM->add(createGVNPass());
  CodeGeneration::TheFPM->add(createCFGSimplificationPass());
  CodeGeneration::TheFPM->doInitialization();
}

AllocaInst *createEntryBlockAlloca(Function *currFunction, StringRef varName) {
  IRBuilder<> irBuilder(&currFunction->getEntryBlock(),
                        currFunction->getEntryBlock().begin());
  return irBuilder.CreateAlloca(Type::getInt64Ty(*TheContext), nullptr,
                                varName);
}
} // namespace CodeGeneration
// namespace CodeGeneration
