#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/AdditionNode.hpp"
#include "Expressions/SubtractionNode.hpp"
#include "Expressions/MultiplicationNode.hpp"
#include "Expressions/DivisionNode.hpp"
#include "Expressions/PowerNode.hpp"
#include "Expressions/ModuloNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

void LLVMCodegenVisitor::visit(AdditionNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateAdd(l, r, "addtmp");
}

void LLVMCodegenVisitor::visit(SubtractionNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateSub(l, r, "subtmp");
}

void LLVMCodegenVisitor::visit(MultiplicationNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateMul(l, r, "multmp");
}

void LLVMCodegenVisitor::visit(DivisionNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    lastValue = builder.CreateSDiv(l, r, "divtmp");
}

void LLVMCodegenVisitor::visit(PowerNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;

    llvm::Type* doubleTy = llvm::Type::getDoubleTy(ctx);
    llvm::Value* lDouble = builder.CreateSIToFP(l, doubleTy, "lftodbl");
    llvm::Value* rDouble = builder.CreateSIToFP(r, doubleTy, "rttodbl");

    llvm::Function* powFunc = module.getFunction("llvm.pow.f64");
    if (!powFunc) {
        llvm::FunctionType* powType = llvm::FunctionType::get(
            doubleTy, {doubleTy, doubleTy}, false);
        powFunc = llvm::Function::Create(
            powType, llvm::Function::ExternalLinkage, "llvm.pow.f64", module);
    }

    llvm::Value* powResult = builder.CreateCall(powFunc, {lDouble, rDouble}, "powtmp");
    lastValue = builder.CreateFPToSI(powResult, llvm::Type::getInt32Ty(ctx), "powint");
}

void LLVMCodegenVisitor::visit(ModuloNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    
    
    llvm::Function* function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* originalBlock = builder.GetInsertBlock();
    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(ctx, "mod.notzero", function);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(ctx, "mod.iszero", function);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(ctx, "mod.cont", function);

    llvm::Value* zero = llvm::ConstantInt::get(r->getType(), 0, true);
    llvm::Value* cond = builder.CreateICmpNE(r, zero, "isNotZero");
    builder.CreateCondBr(cond, thenBlock, elseBlock);

    builder.SetInsertPoint(thenBlock);
    llvm::Value* resultNonZero = builder.CreateSRem(l, r, "modtmp");
    builder.CreateBr(mergeBlock);
    thenBlock = builder.GetInsertBlock(); 

    builder.SetInsertPoint(elseBlock);
    
    
    llvm::Value* resultZero = llvm::ConstantInt::get(l->getType(), 0, true); 
    
    
    builder.CreateBr(mergeBlock);
    elseBlock = builder.GetInsertBlock(); 

    builder.SetInsertPoint(mergeBlock);
    llvm::PHINode* phi = builder.CreatePHI(l->getType(), 2, "modresult");
    phi->addIncoming(resultNonZero, thenBlock);
    phi->addIncoming(resultZero, elseBlock);
    lastValue = phi;
}