#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/AndNode.hpp"
#include "Expressions/OrNode.hpp"
#include "Expressions/NotNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

void LLVMCodegenVisitor::visit(AndNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    
    l = builder.CreateICmpNE(l, llvm::ConstantInt::get(l->getType(), 0), "andl");
    r = builder.CreateICmpNE(r, llvm::ConstantInt::get(r->getType(), 0), "andr");
    lastValue = builder.CreateAnd(l, r, "andtmp");
}

void LLVMCodegenVisitor::visit(OrNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    
    l = builder.CreateICmpNE(l, llvm::ConstantInt::get(l->getType(), 0), "orl");
    r = builder.CreateICmpNE(r, llvm::ConstantInt::get(r->getType(), 0), "orr");
    lastValue = builder.CreateOr(l, r, "ortmp");
}

void LLVMCodegenVisitor::visit(NotNode& node) {
    node.operand->accept(*this);
    llvm::Value* v = lastValue;
    v = builder.CreateICmpEQ(v, llvm::ConstantInt::get(v->getType(), 0), "nottmp");
    lastValue = v;
}