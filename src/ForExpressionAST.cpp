#include "ForExpressionAST.h"
#include "CodeGeneration.h"

Value *ForExpressionsAST::codegen() {
  /* emit code for start value */
  Value *startVal = m_startExpr->codegen();
  if (!startVal)
    return nullptr;

  Function *currFunction =
      CodeGeneration::Builder->GetInsertBlock()->getParent();
  BasicBlock *preHeaderBB = CodeGeneration::Builder->GetInsertBlock();

  /* Create loop entry BasicBlock*/
  BasicBlock *loopHeaderBasicB =
      BasicBlock::Create(*CodeGeneration::TheContext, "loop", currFunction);

  CodeGeneration::Builder->CreateBr(loopHeaderBasicB);
  CodeGeneration::Builder->SetInsertPoint(loopHeaderBasicB);

  PHINode *variable = CodeGeneration::Builder->CreatePHI(
      Type::getInt64Ty(*CodeGeneration::TheContext), 2, m_varName);

  /* add initial value (start value) to phinode */
  variable->addIncoming(startVal, preHeaderBB);

  /* if a variable exists with same name as for variable, then save it , and
   * restore it later */
  Value *oldVarValue = CodeGeneration::NamedValues[m_varName];
  CodeGeneration::NamedValues[m_varName] = variable;

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
  if (!m_bodyExpr->codegen())
    return nullptr;

  /* update the for variable */
  Value *stepVal = m_stepExpr->codegen();
  if (!stepVal)
    return nullptr;
  Value *nextVarVal =
      CodeGeneration::Builder->CreateAdd(variable, stepVal, "nextVarVal");

  /* jump to beginning of loop */
  CodeGeneration::Builder->CreateBr(loopHeaderBasicB);

  /* start of after loop */
  CodeGeneration::Builder->SetInsertPoint(afterBasicB);

  /* add updated value from loop body to phinode */
  variable->addIncoming(nextVarVal, loopBodyBasicB);

  /* reset old Value */
  if (oldVarValue)
    CodeGeneration::NamedValues[m_varName] = oldVarValue;
  else
    CodeGeneration::NamedValues.erase(m_varName);

  /*return null, ie 0*/
  return Constant::getNullValue(Type::getInt64Ty(*CodeGeneration::TheContext));
}
