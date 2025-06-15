#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/AndNode.hpp"
#include "Expressions/OrNode.hpp"
#include "Expressions/NotNode.hpp"
#include "Expressions/UnaryMinusNode.hpp"

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

void LLVMCodegenVisitor::visit(UnaryMinusNode& node) {
    node.operand->accept(*this);
    llvm::Value* v = lastValue;
    
    if (v->getType()->isIntegerTy()) {

        lastValue = builder.CreateNeg(v, "negtmp");
    } else if (v->getType()->isDoubleTy()) {

        lastValue = builder.CreateFNeg(v, "fnegtmp");
    } else {

        llvm::Value* intVal = builder.CreatePtrToInt(v, llvm::Type::getInt32Ty(builder.getContext()), "ptrtoint");
        lastValue = builder.CreateNeg(intVal, "negtmp");
    }
}