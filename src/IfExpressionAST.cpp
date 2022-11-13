#include "IfExpressionAST.h"
#include "CodeGeneration.h"

Value *IfExpressionAST::codegen() {
  Value *condValue = m_condExpr->codegen();
  if (!condValue)
    return nullptr;

  condValue = CodeGeneration::Builder->CreateICmpEQ(
      condValue,
      ConstantInt::get(*CodeGeneration::TheContext, APInt(64, 1, true)),
      "ifcond");

  Function *currFunction =
      CodeGeneration::Builder->GetInsertBlock()->getParent();

  BasicBlock *ifBlockB =
      BasicBlock::Create(*CodeGeneration::TheContext, "ifExpr", currFunction);
  BasicBlock *elseBlockB =
      BasicBlock::Create(*CodeGeneration::TheContext, "elseExpr");
  BasicBlock *mergeBlockB =
      BasicBlock::Create(*CodeGeneration::TheContext, "ifcontin");

  CodeGeneration::Builder->CreateCondBr(condValue, ifBlockB, elseBlockB);

  CodeGeneration::Builder->SetInsertPoint(ifBlockB);

  Value *ifValue = m_ifExpr->codegen();
  if (!ifValue)
    return nullptr;

  CodeGeneration::Builder->CreateBr(mergeBlockB);
  ifBlockB = CodeGeneration::Builder->GetInsertBlock();

  currFunction->getBasicBlockList().push_back(elseBlockB);
  CodeGeneration::Builder->SetInsertPoint(elseBlockB);

  Value *elseValue = m_elseExpr->codegen();
  if (!elseValue)
    return nullptr;

  CodeGeneration::Builder->CreateBr(mergeBlockB);
  elseBlockB = CodeGeneration::Builder->GetInsertBlock();

  currFunction->getBasicBlockList().push_back(mergeBlockB);
  CodeGeneration::Builder->SetInsertPoint(mergeBlockB);
  PHINode *phiNode = CodeGeneration::Builder->CreatePHI(
      Type::getInt64Ty(*CodeGeneration::TheContext), 2, "iftmp");

  phiNode->addIncoming(ifValue, ifBlockB);
  phiNode->addIncoming(elseValue, elseBlockB);
  return phiNode;
}
