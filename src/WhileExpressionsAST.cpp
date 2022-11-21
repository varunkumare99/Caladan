#include "CodeGeneration.h"
#include "WhileExpressionAST.h"

Value *WhileExpressionAST::codegen() {

  BasicBlock *whileCondBB =
      BasicBlock::Create(*CodeGeneration::TheContext, "whileCond");
  BasicBlock *whileBodyBB =
      BasicBlock::Create(*CodeGeneration::TheContext, "whileBody");
  BasicBlock *whileEndBB =
      BasicBlock::Create(*CodeGeneration::TheContext, "whileEnd");

  Function *currFunction =
      CodeGeneration::Builder->GetInsertBlock()->getParent();

  CodeGeneration::Builder->CreateBr(whileCondBB);
  currFunction->getBasicBlockList().push_back(whileCondBB);
  CodeGeneration::Builder->SetInsertPoint(whileCondBB);

  Value *condValue = m_condExpr->codegen();
  if (!condValue)
    return nullptr;

  condValue = CodeGeneration::Builder->CreateICmpEQ(
      condValue,
      ConstantInt::get(*CodeGeneration::TheContext, APInt(64, 1, true)),
      "whileCondValue");

  CodeGeneration::Builder->CreateCondBr(condValue, whileBodyBB, whileEndBB);

  currFunction->getBasicBlockList().push_back(whileBodyBB);
  CodeGeneration::Builder->SetInsertPoint(whileBodyBB);

  for (auto index = 0; index < m_bodyExpr->expressionList.size(); ++index) {
    auto whileBodyValue = m_bodyExpr->expressionList[index]->codegen();
    if (!whileBodyValue)
      return nullptr;
  }

  CodeGeneration::Builder->CreateBr(whileCondBB);

  currFunction->getBasicBlockList().push_back(whileEndBB);
  CodeGeneration::Builder->SetInsertPoint(whileEndBB);

  /*return null, ie 0*/
  return Constant::getNullValue(Type::getInt64Ty(*CodeGeneration::TheContext));
}
