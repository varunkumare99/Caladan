#ifndef PrototypeAST_H
#define PrototypeAST_H

#include "llvm/IR/Function.h"
#include <iostream>
#include <vector>

using namespace llvm;

class PrototypeAST {
private:
  std::string m_functionName;
  std::vector<std::string> m_arguments;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> args)
      : m_functionName{name}, m_arguments{std::move(args)} {}

  const std::string &getFunctionName() const { return m_functionName; }
  Function *codegen();
};

#endif
