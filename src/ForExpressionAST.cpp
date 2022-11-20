#include "ForExpressionAST.h"
#include "CodeGeneration.h"

Value *ForExpressionsAST::codegen() {
  Function *currFunction =
      CodeGeneration::Builder->GetInsertBlock()->getParent();

  /* Create alloca for variable */
  AllocaInst *alloca =
      CodeGeneration::createEntryBlockAlloca(currFunction, m_varName);

  /* emit code for start value */
  Value *startVal = m_startExpr->codegen();
  if (!startVal)
    return nullptr;

  CodeGeneration::Builder->CreateStore(startVal, alloca);

  BasicBlock *preHeaderBB = CodeGeneration::Builder->GetInsertBlock();

  /* Create loop entry BasicBlock*/
  BasicBlock *loopHeaderBasicB =
      BasicBlock::Create(*CodeGeneration::TheContext, "loop", currFunction);

  CodeGeneration::Builder->CreateBr(loopHeaderBasicB);
  CodeGeneration::Builder->SetInsertPoint(loopHeaderBasicB);

  /* if a variable exists with same name as for variable, then save it , and
   * restore it later */
  AllocaInst *oldVarValue = nullptr;
  if (CodeGeneration::NamedValues.find(m_varName) !=
      CodeGeneration::NamedValues.end()) {
    oldVarValue = CodeGeneration::NamedValues[m_varName];
  }
  CodeGeneration::NamedValues[m_varName] = alloca;

  /* emit code to check for loop condition */
  Value *condVal = m_condExpr->codegen();
  if (!condVal)
    return nullptr;

  /* Then result of the condition is 0 or 1 the branch accordingly */
  condVal = CodeGeneration::Builder->CreateICmpEQ(
      condVal,
      ConstantInt::get(*CodeGeneration::TheContext, APInt(64, 1, false)),
      "loopcond");

  /* create BasicBlocks for loopbody and after */
  BasicBlock *loopBodyBasicB =
      BasicBlock::Create(*CodeGeneration::TheContext, "loopBody", currFunction);
  BasicBlock *afterBasicB = BasicBlock::Create(*CodeGeneration::TheContext,
                                               "afterloop", currFunction);

  /* if condition is true go to loopBody, else go to after loop */
  CodeGeneration::Builder->CreateCondBr(condVal, loopBodyBasicB, afterBasicB);

  /* start of loopBody */
  CodeGeneration::Builder->SetInsertPoint(loopBodyBasicB);

  /* emit loopbody code in loopBody basicblock */
  for (auto index = 0; index < m_bodyExpr->expressionList.size(); ++index) {
    if (!m_bodyExpr->expressionList[index]->codegen())
      return nullptr;
  }

  /* update the for variable */
  Value *stepVal = m_stepExpr->codegen();
  if (!stepVal)
    return nullptr;

  Value *curVarValue = CodeGeneration::Builder->CreateLoad(
      alloca->getAllocatedType(), alloca, m_varName.c_str());
  Value *nextVarValue =
      CodeGeneration::Builder->CreateAdd(curVarValue, stepVal, "nextvar");
  CodeGeneration::Builder->CreateStore(nextVarValue, alloca);

  /* jump to beginning of loop */
  CodeGeneration::Builder->CreateBr(loopHeaderBasicB);

  /* start of after loop */
  CodeGeneration::Builder->SetInsertPoint(afterBasicB);

  /* reset old Value */
  if (oldVarValue)
    CodeGeneration::NamedValues[m_varName] = oldVarValue;
  else
    CodeGeneration::NamedValues.erase(m_varName);

  /*return null, ie 0*/
  return Constant::getNullValue(Type::getInt64Ty(*CodeGeneration::TheContext));
}
