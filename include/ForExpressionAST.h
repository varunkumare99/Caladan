#ifndef ForExpressionAST_H
#define ForExpressionAST_H

#include "ExpressionAST.h"

class ForExpressionsAST : public ExpressionAST {
private:
  std::string m_varName;
  std::unique_ptr<ExpressionAST> m_startExpr, m_condExpr, m_stepExpr;
  std::unique_ptr<ExpressionListAST> m_bodyExpr;

public:
  ForExpressionsAST(const std::string &varName,
                    std::unique_ptr<ExpressionAST> startExpr,
                    std::unique_ptr<ExpressionAST> condExpr,
                    std::unique_ptr<ExpressionAST> stepExpr,
                    std::unique_ptr<ExpressionListAST> bodyExpr)
      : m_varName(varName), m_startExpr(std::move(startExpr)),
        m_condExpr(std::move(condExpr)), m_stepExpr(std::move(stepExpr)),
        m_bodyExpr(std::move(bodyExpr)) {}

  Value *codegen() override;
};

#endif
