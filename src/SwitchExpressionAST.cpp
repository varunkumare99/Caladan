#include "SwitchExpressionAST.h"
#include "CodeGeneration.h"

Value *SwitchExpressionAST::codegen() {
  Value *condValue = m_condExpr->codegen();
  if (!condValue)
    return nullptr;

  Function *currFunction =
      CodeGeneration::Builder->GetInsertBlock()->getParent();

  std::vector<BasicBlock *> casesBlockB;
  for (auto i = 0; i < m_casesExpr.size(); ++i) {
    casesBlockB.push_back(
        BasicBlock::Create(*CodeGeneration::TheContext,
                           "case_" + std::to_string(m_casesExpr[0].first)));
  }

  BasicBlock *defaultBlockB =
      BasicBlock::Create(*CodeGeneration::TheContext, "defaultExpr");
  BasicBlock *endOfSwitchBlockB =
      BasicBlock::Create(*CodeGeneration::TheContext, "endSwitch");

  auto switchInstruction = CodeGeneration::Builder->CreateSwitch(
      condValue, defaultBlockB, m_casesExpr.size());

  for (auto i = 0; i < m_casesExpr.size(); ++i) {
    switchInstruction->addCase(
        ConstantInt::get(*CodeGeneration::TheContext,
                         APInt(64, m_casesExpr[i].first, true)),
        casesBlockB[i]);
  }

  for (auto i = 0; i < m_casesExpr.size(); ++i) {
    currFunction->getBasicBlockList().push_back(casesBlockB[i]);
    CodeGeneration::Builder->SetInsertPoint(casesBlockB[i]);
    for (auto index = 0; index < m_casesExpr[i].second->expressionList.size();
         ++index) {
      Value *caseExprValue =
          m_casesExpr[i].second->expressionList[index]->codegen();
      if (!caseExprValue)
        return nullptr;
    }
    CodeGeneration::Builder->CreateBr(endOfSwitchBlockB);
  }

  currFunction->getBasicBlockList().push_back(defaultBlockB);
  CodeGeneration::Builder->SetInsertPoint(defaultBlockB);
  Value *defaultExprValue = m_defaultExpr->codegen();
  if (!defaultExprValue)
    return nullptr;
  CodeGeneration::Builder->CreateBr(endOfSwitchBlockB);

  currFunction->getBasicBlockList().push_back(endOfSwitchBlockB);
  CodeGeneration::Builder->SetInsertPoint(endOfSwitchBlockB);

  /*return null, ie 0*/
  return Constant::getNullValue(Type::getInt64Ty(*CodeGeneration::TheContext));
}
