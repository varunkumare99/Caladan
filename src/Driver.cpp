#include "CodeGeneration.h"
#include "KaleidoscopeJIT.h"
#include "Parser.h"

#include "llvm/Support/TargetSelect.h"

int main() {

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();
  Lexer lexer;
  Parser parser(lexer);

  fprintf(stderr, PROMPT);
  parser.getNextToken();

  CodeGeneration::TheJIT = CodeGeneration::ExitOnErr(KaleidoscopeJIT::Create());
  CodeGeneration::initializeModuleAndPassManager();
  parser.mainLoop();
  CodeGeneration::TheModule->print(errs(), nullptr);
  return 0;
}
