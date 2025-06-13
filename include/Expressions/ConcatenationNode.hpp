#ifndef CONCATENATION_NODE_HPP
#define CONCATENATION_NODE_HPP

#include "../AST/ExpressionNode.hpp"
#include "BinaryOperatorNode.hpp"
#include <string>
namespace llvm {
    class Value;
}

class LLVMCodegenVisitor;

class ConcatenationNode : public BinaryOperatorNode {
public:
    ConcatenationNode(ExpressionNode* left, ExpressionNode* right);
    virtual int evaluate() const override;
    virtual std::string evaluateString() const override;
    virtual char getOperator() const override;
    virtual void accept(LLVMCodegenVisitor& visitor) override;
    llvm::Value* CodeGen(LLVMCodegenVisitor& visitor);
};

#endif
