#include "CodeGeneration.h"
#include "KaleidoscopeJIT.h"
#include "Parser.h"

#include "llvm/Support/TargetSelect.h"

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT int putchari(int X) {
  fputc((char)X, stderr);
  return 0;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" DLLEXPORT int printi(int X) {
  fprintf(stderr, "%d\n", X);
  return 0;
}

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
