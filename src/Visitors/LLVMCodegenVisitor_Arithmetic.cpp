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
    

    if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {

        if (!l->getType()->isDoubleTy()) {
            if (l->getType()->isIntegerTy()) {
                l = builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx), "ltodbl");
            }
        }
        if (!r->getType()->isDoubleTy()) {
            if (r->getType()->isIntegerTy()) {
                r = builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx), "rtodbl");
            }
        }
        lastValue = builder.CreateFAdd(l, r, "addtmp");
    } else {

        lastValue = builder.CreateAdd(l, r, "addtmp");
    }
}

void LLVMCodegenVisitor::visit(SubtractionNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    

    if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {

        if (!l->getType()->isDoubleTy()) {
            if (l->getType()->isIntegerTy()) {
                l = builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx), "ltodbl");
            }
        }
        if (!r->getType()->isDoubleTy()) {
            if (r->getType()->isIntegerTy()) {
                r = builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx), "rtodbl");
            }
        }
        lastValue = builder.CreateFSub(l, r, "subtmp");
    } else {

        lastValue = builder.CreateSub(l, r, "subtmp");
    }
}

void LLVMCodegenVisitor::visit(MultiplicationNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    

    if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {

        if (!l->getType()->isDoubleTy()) {
            if (l->getType()->isIntegerTy()) {
                l = builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx), "ltodbl");
            }
        }
        if (!r->getType()->isDoubleTy()) {
            if (r->getType()->isIntegerTy()) {
                r = builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx), "rtodbl");
            }
        }
        lastValue = builder.CreateFMul(l, r, "multmp");
    } else {

        lastValue = builder.CreateMul(l, r, "multmp");
    }
}

void LLVMCodegenVisitor::visit(DivisionNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;
    

    if (l->getType()->isDoubleTy() || r->getType()->isDoubleTy()) {

        if (!l->getType()->isDoubleTy()) {
            if (l->getType()->isIntegerTy()) {
                l = builder.CreateSIToFP(l, llvm::Type::getDoubleTy(ctx), "ltodbl");
            }
        }
        if (!r->getType()->isDoubleTy()) {
            if (r->getType()->isIntegerTy()) {
                r = builder.CreateSIToFP(r, llvm::Type::getDoubleTy(ctx), "rtodbl");
            }
        }
        lastValue = builder.CreateFDiv(l, r, "divtmp");
    } else {

        lastValue = builder.CreateSDiv(l, r, "divtmp");
    }
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

    builder.SetInsert(thenBlock);
    llvm::Value* resultNonZero = builder.CreateSRem(l, r, "modtmp");
    builder.CreateBr(mergeBlock);
    thenBlock = builder.GetInsertBlock(); 

    builder.SetInsert(elseBlock);
    
    
    llvm::Value* resultZero = llvm::ConstantInt::get(l->getType(), 0, true); 
    
    
    builder.CreateBr(mergeBlock);
    elseBlock = builder.GetInsertBlock(); 

    builder.SetInsert(mergeBlock);
    llvm::PHINode* phi = builder.CreatePHI(l->getType(), 2, "modresult");
    phi->addIncoming(resultNonZero, thenBlock);
    phi->addIncoming(resultZero, elseBlock);
    lastValue = phi;
}