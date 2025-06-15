#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/ConditionalNode.hpp"
#include "Expressions/WhileNode.hpp"
#include "Expressions/ForRangeNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>


static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(ConditionalNode& node) {
    llvm::Function* function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* afterBlock = llvm::BasicBlock::Create(ctx, "ifcont", function);

    std::vector<llvm::BasicBlock*> branchCondBlocks;
    std::vector<llvm::BasicBlock*> branchBodyBlocks;
    for (size_t i = 0; i < node.branches.size(); ++i) {
        branchCondBlocks.push_back(llvm::BasicBlock::Create(ctx, "ifcond", function));
        branchBodyBlocks.push_back(llvm::BasicBlock::Create(ctx, "ifbody", function));
    }
    llvm::BasicBlock* elseEntryBlock = nullptr;
    llvm::BasicBlock* actualElseBodyBlock = nullptr;

    if (node.elseBody) {
        actualElseBodyBlock = llvm::BasicBlock::Create(ctx, "elsebody", function);
        elseEntryBlock = actualElseBodyBlock;
    } else {





        elseEntryBlock = afterBlock;
    }


    if (!branchCondBlocks.empty()) {
        builder.CreateBr(branchCondBlocks[0]);
    } else {
        builder.CreateBr(elseEntryBlock);
    }

    std::vector<std::pair<llvm::BasicBlock*, llvm::Value*>> valueProducingBlocks;

    for (size_t i = 0; i < node.branches.size(); ++i) {
        builder.SetInsertPoint(branchCondBlocks[i]);
        node.branches[i].condition->accept(*this);
        llvm::Value* condValue = lastValue;

        condValue = builder.CreateICmpNE(condValue, llvm::ConstantInt::get(condValue->getType(), 0, true), "ifcondbool");

        llvm::BasicBlock* nextCondBlockOrElse = (i + 1 < node.branches.size()) ? branchCondBlocks[i+1] : elseEntryBlock;
        builder.CreateCondBr(condValue, branchBodyBlocks[i], nextCondBlockOrElse);

        builder.SetInsertPoint(branchBodyBlocks[i]);
        node.branches[i].body->accept(*this);

        if (lastValue) {
             valueProducingBlocks.push_back({builder.GetInsertBlock(), lastValue});
        } else {


        }
        builder.CreateBr(afterBlock);
    }

    if (node.elseBody) {
        if (actualElseBodyBlock != elseEntryBlock && branchCondBlocks.empty()) {






        }
        builder.SetInsertPoint(actualElseBodyBlock);
        node.elseBody->accept(*this);
        if (lastValue) {
            valueProducingBlocks.push_back({builder.GetInsertBlock(), lastValue});
        }
        builder.CreateBr(afterBlock);
    } else if (branchCondBlocks.empty()) {







    }

    builder.SetInsertPoint(afterBlock);

    if (!valueProducingBlocks.empty()) {

        llvm::Type* phiType = valueProducingBlocks[0].second->getType();
        bool allSameType = true;
        for (const auto& vb : valueProducingBlocks) {
            if (vb.second->getType() != phiType) {
                allSameType = false;

                phiType = llvm::Type::getDoubleTy(ctx);
                break;
            }
        }

        if (allSameType || valueProducingBlocks.size() == 1) {
            llvm::PHINode* phi = builder.CreatePHI(phiType, valueProducingBlocks.size(), "iftmp");
            for (auto& vb : valueProducingBlocks) {
                llvm::Value* value = vb.second;

                if (value->getType() != phiType) {
                    if (value->getType()->isIntegerTy() && phiType->isDoubleTy()) {
                        value = builder.CreateSIToFP(value, phiType, "int_to_double");
                    } else if (value->getType()->isDoubleTy() && phiType->isIntegerTy()) {
                        value = builder.CreateFPToSI(value, phiType, "double_to_int");
                    }
                }
                phi->addIncoming(value, vb.first);
            }
            lastValue = phi;
        } else {

            llvm::PHINode* phi = builder.CreatePHI(phiType, valueProducingBlocks.size(), "iftmp");
            for (auto& vb : valueProducingBlocks) {
                llvm::Value* value = vb.second;

                if (value->getType()->isIntegerTy()) {
                    value = builder.CreateSIToFP(value, phiType, "int_to_double");
                }
                phi->addIncoming(value, vb.first);
            }
            lastValue = phi;
        }
    } else {


        lastValue = nullptr;
    }
}

void LLVMCodegenVisitor::visit(WhileNode& node) {
    llvm::Function* function = builder.GetInsertBlock()->getParent();


    llvm::BasicBlock* preLoopBlock = builder.GetInsertBlock();
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(ctx, "whilecond", function);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(ctx, "whilebody", function);
    llvm::BasicBlock* afterBlock = llvm::BasicBlock::Create(ctx, "whilecont", function);





    llvm::Type* valueType = llvm::Type::getInt32Ty(ctx);
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    llvm::AllocaInst* resultAlloca = tmpB.CreateAlloca(valueType, nullptr, "whileresult");


    builder.CreateStore(llvm::ConstantInt::get(valueType, 0), resultAlloca);


    builder.CreateBr(condBlock);


    builder.SetInsertPoint(condBlock);


    node.condition->accept(*this);
    llvm::Value* condValue = lastValue;


    llvm::Value* condBool = builder.CreateICmpNE(condValue, llvm::ConstantInt::get(condValue->getType(), 0, true), "whilecondbool");


    builder.CreateCondBr(condBool, bodyBlock, afterBlock);


    builder.SetInsertPoint(bodyBlock);


    node.body->accept(*this);


    if (lastValue && lastValue->getType() == valueType) {
        builder.CreateStore(lastValue, resultAlloca);
    }


    builder.CreateBr(condBlock);


    builder.SetInsertPoint(afterBlock);


    lastValue = builder.CreateLoad(resultAlloca->getAllocatedType(), resultAlloca, "whilefinalvalue");
}

void LLVMCodegenVisitor::visit(ForRangeNode& node) {




    node.startExpr->accept(*this);
    llvm::Value* startValue = lastValue;

    node.endExpr->accept(*this);
    llvm::Value* endValue = lastValue;


    llvm::Function* function = builder.GetInsertBlock()->getParent();
    llvm::Type* intType = llvm::Type::getInt32Ty(ctx);
    llvm::AllocaInst* loopVar = createEntryBlockAlloca(function, intType, node.loopVar);
    

    llvm::AllocaInst* resultVar = createEntryBlockAlloca(function, intType, "for.result");

    builder.CreateStore(llvm::ConstantInt::get(intType, 0), resultVar);


    if (startValue->getType() != intType) {
        if (startValue->getType()->isDoubleTy()) {
            startValue = builder.CreateFPToSI(startValue, intType, "start_to_int");
        } else if (startValue->getType()->isIntegerTy()) {
            if (startValue->getType()->getIntegerBitWidth() < intType->getIntegerBitWidth()) {
                startValue = builder.CreateZExt(startValue, intType, "start_extend");
            } else if (startValue->getType()->getIntegerBitWidth() > intType->getIntegerBitWidth()) {
                startValue = builder.CreateTrunc(startValue, intType, "start_trunc");
            }
        }
    }

    builder.CreateStore(startValue, loopVar);


    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(ctx, "for.cond", function);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(ctx, "for.body", function);
    llvm::BasicBlock* incBB = llvm::BasicBlock::Create(ctx, "for.inc", function);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(ctx, "for.end", function);


    builder.CreateBr(condBB);


    builder.SetInsertPoint(condBB);
    llvm::Value* currentVar = builder.CreateLoad(intType, loopVar, "loop.var");

    if (endValue->getType() != intType) {
        if (endValue->getType()->isDoubleTy()) {
            endValue = builder.CreateFPToSI(endValue, intType, "end_to_int");
        } else if (endValue->getType()->isIntegerTy()) {
            if (endValue->getType()->getIntegerBitWidth() < intType->getIntegerBitWidth()) {
                endValue = builder.CreateZExt(endValue, intType, "end_extend");
            } else if (endValue->getType()->getIntegerBitWidth() > intType->getIntegerBitWidth()) {
                endValue = builder.CreateTrunc(endValue, intType, "end_trunc");
            }
        }
    }
    llvm::Value* cond = builder.CreateICmpSLT(currentVar, endValue, "for.cond");
    builder.CreateCondBr(cond, bodyBB, afterBB);


    builder.SetInsertPoint(bodyBB);


    localVarsStack.push_back(localVarsStack.back());
    localVarsStack.back()[node.loopVar] = loopVar;


    if (node.body) {
        node.body->accept(*this);

        if (lastValue && lastValue->getType() == intType) {
            builder.CreateStore(lastValue, resultVar);
        }
    }


    localVarsStack.pop_back();


    builder.CreateBr(incBB);


    builder.SetInsertPoint(incBB);
    llvm::Value* nextVar = builder.CreateAdd(
        builder.CreateLoad(intType, loopVar, "loop.var"),
        llvm::ConstantInt::get(intType, 1),
        "loop.next"
    );
    builder.CreateStore(nextVar, loopVar);
    builder.CreateBr(condBB);


    builder.SetInsertPoint(afterBB);


    lastValue = builder.CreateLoad(intType, resultVar, "for.final.result");
}