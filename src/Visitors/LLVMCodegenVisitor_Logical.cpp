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
    

    if (l->getType()->isIntegerTy()) {
        l = builder.CreateICmpNE(l, llvm::ConstantInt::get(l->getType(), 0), "andl");
    } else if (l->getType()->isDoubleTy()) {
        l = builder.CreateFCmpONE(l, llvm::ConstantFP::get(l->getType(), 0.0), "andl");
    } else {

        llvm::Value* intVal = builder.CreatePtrToInt(l, llvm::Type::getInt32Ty(builder.getContext()), "ptrtoint");
        l = builder.CreateICmpNE(intVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(builder.getContext()), 0), "andl");
    }
    

    if (r->getType()->isIntegerTy()) {
        r = builder.CreateICmpNE(r, llvm::ConstantInt::get(r->getType(), 0), "andr");
    } else if (r->getType()->isDoubleTy()) {
        r = builder.CreateFCmpONE(r, llvm::ConstantFP::get(r->getType(), 0.0), "andr");
    } else {

        llvm::Value* intVal = builder.CreatePtrToInt(r, llvm::Type::getInt32Ty(builder.getContext()), "ptrtoint");
        r = builder.CreateICmpNE(intVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(builder.getContext()), 0), "andr");
    }
    
    lastValue = builder.CreateAnd(l, r, "andtmp");
}

void LLVMCodegenVisitor::visit(OrNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    

    if (l->getType()->isIntegerTy()) {
        l = builder.CreateICmpNE(l, llvm::ConstantInt::get(l->getType(), 0), "orl");
    } else if (l->getType()->isDoubleTy()) {
        l = builder.CreateFCmpONE(l, llvm::ConstantFP::get(l->getType(), 0.0), "orl");
    } else {

        llvm::Value* intVal = builder.CreatePtrToInt(l, llvm::Type::getInt32Ty(builder.getContext()), "ptrtoint");
        l = builder.CreateICmpNE(intVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(builder.getContext()), 0), "orl");
    }
    

    if (r->getType()->isIntegerTy()) {
        r = builder.CreateICmpNE(r, llvm::ConstantInt::get(r->getType(), 0), "orr");
    } else if (r->getType()->isDoubleTy()) {
        r = builder.CreateFCmpONE(r, llvm::ConstantFP::get(r->getType(), 0.0), "orr");
    } else {

        llvm::Value* intVal = builder.CreatePtrToInt(r, llvm::Type::getInt32Ty(builder.getContext()), "ptrtoint");
        r = builder.CreateICmpNE(intVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(builder.getContext()), 0), "orr");
    }
    
    lastValue = builder.CreateOr(l, r, "ortmp");
}

void LLVMCodegenVisitor::visit(NotNode& node) {
    node.operand->accept(*this);
    llvm::Value* v = lastValue;
    

    if (v->getType()->isIntegerTy()) {
        v = builder.CreateICmpEQ(v, llvm::ConstantInt::get(v->getType(), 0), "nottmp");
    } else if (v->getType()->isDoubleTy()) {
        v = builder.CreateFCmpOEQ(v, llvm::ConstantFP::get(v->getType(), 0.0), "nottmp");
    } else {

        llvm::Value* intVal = builder.CreatePtrToInt(v, llvm::Type::getInt32Ty(builder.getContext()), "ptrtoint");
        v = builder.CreateICmpEQ(intVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(builder.getContext()), 0), "nottmp");
    }
    
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