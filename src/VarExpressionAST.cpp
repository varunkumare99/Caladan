#include "VarExpressionAST.h"
#include "CodeGeneration.h"
#include "llvm/IR/Instructions.h"

Value *VarExpressionsAST::codegen() {
  std::vector<AllocaInst *> oldBindings;

  Function *currFunction =
      CodeGeneration::Builder->GetInsertBlock()->getParent();

  /*Register all var's and emit their initializers */
  for (unsigned i = 0; i < m_varNames.size(); ++i) {
    const std::string &varName = m_varNames[i].first;
    ExpressionAST *varInit = m_varNames[i].second.get();

    Value *varInitValue;
    if (varInit) {
      varInitValue = varInit->codegen();
      if (!varInitValue)
        return nullptr;
    } else {
      varInitValue =
          ConstantInt::get(*CodeGeneration::TheContext, APInt(64, 0, false));
    }

    AllocaInst *alloca =
        CodeGeneration::createEntryBlockAlloca(currFunction, varName);
    /* store the initValue on stack */
    CodeGeneration::Builder->CreateStore(varInitValue, alloca);

    /* remember old bindings */
    oldBindings.push_back(CodeGeneration::NamedValues[varName]);

    /* Remember the current one */
    CodeGeneration::NamedValues[varName] = alloca;
  }

  /* generated for body as all vars are in scope */
  Value *varBodyVal = nullptr;
  for (auto index = 0; index < m_varBody->expressionList.size(); ++index) {
    varBodyVal = m_varBody->expressionList[index]->codegen();
    if (!varBodyVal)
      return nullptr;
  }

  /* remove vars from scope */
  for (unsigned i = 0; i < m_varNames.size(); ++i) {
    CodeGeneration::NamedValues[m_varNames[i].first] = oldBindings[i];
  }
  return varBodyVal;
}
