#ifndef CodeGeneration_H
#define CodeGeneration_H

#include "KaleidoscopeJIT.h"
#include "PrototypeAST.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include <map>
#include <memory>

using namespace llvm;
using namespace llvm::orc;

namespace CodeGeneration {
extern std::unique_ptr<LLVMContext> TheContext;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<IRBuilder<>> Builder;
extern std::map<std::string, Value *> NamedValues;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
extern std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
extern std::unique_ptr<KaleidoscopeJIT> TheJIT;
extern ExitOnError ExitOnErr;

extern Value *logErrorV(const char *str);
extern Function *getFunction(std::string Name);
void initializeModuleAndPassManager(void);
}; // namespace CodeGeneration

#endif
