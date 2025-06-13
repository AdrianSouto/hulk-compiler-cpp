#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/EqualNode.hpp"
#include "Expressions/NotEqualNode.hpp"
#include "Expressions/GreaterNode.hpp"
#include "Expressions/GreaterEqNode.hpp"
#include "Expressions/LessNode.hpp"
#include "Expressions/LessEqNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

void LLVMCodegenVisitor::visit(EqualNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateICmpEQ(l, r, "eqtmp");
}

void LLVMCodegenVisitor::visit(NotEqualNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateICmpNE(l, r, "netmp");
}

void LLVMCodegenVisitor::visit(GreaterNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateICmpSGT(l, r, "gttmp");
}

void LLVMCodegenVisitor::visit(GreaterEqNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateICmpSGE(l, r, "getmp");
}

void LLVMCodegenVisitor::visit(LessNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateICmpSLT(l, r, "lttmp");
}

void LLVMCodegenVisitor::visit(LessEqNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateICmpSLE(l, r, "letmp");
}