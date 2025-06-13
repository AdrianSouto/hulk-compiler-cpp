#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/ConcatenationNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

void LLVMCodegenVisitor::visit(ConcatenationNode& node) {
    node.left->accept(*this);
    llvm::Value* leftValue = lastValue;
    
    node.right->accept(*this);
    llvm::Value* rightValue = lastValue;
    

    bool leftIsString = leftValue->getType()->iserTy();
    bool rightIsString = rightValue->getType()->iserTy();
    

    llvm::Function* mallocFunc = module.getFunction("malloc");
    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(
            llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0),
            {llvm::Type::getInt64Ty(ctx)},
            false
        );
        mallocFunc = llvm::Function::Create(
            mallocType,
            llvm::Function::ExternalLinkage,
            "malloc",
            module
        );
    }
    
    llvm::Value* bufSize = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 256);
    llvm::Value* bufPtr = builder.CreateCall(mallocFunc, {bufSize}, "concat_heap_buf");
    
    llvm::FunctionType* sprintfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(ctx),
        {llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0), llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0)},
        true
    );
    llvm::FunctionCallee sprintfFunc = module.getOrInsertFunction("sprintf", sprintfType);
    

    llvm::FunctionType* strcpyType = llvm::FunctionType::get(
        llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0),
        {llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0), llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0)},
        false
    );
    llvm::FunctionCallee strcpyFunc = module.getOrInsertFunction("strcpy", strcpyType);
    
    llvm::FunctionType* strcatType = llvm::FunctionType::get(
        llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0),
        {llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0), llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0)},
        false
    );
    llvm::FunctionCallee strcatFunc = module.getOrInsertFunction("strcat", strcatType);
    

    if (leftIsString) {

        builder.CreateCall(strcpyFunc, {bufPtr, leftValue});
    } else if (leftValue->getType()->isIntegerTy(1)) {

        llvm::Value* trueStr = builder.CreateGlobalStringPtr("true");
        llvm::Value* falseStr = builder.CreateGlobalStringPtr("false");
        llvm::Value* selectedStr = builder.CreateSelect(leftValue, trueStr, falseStr, "bool_str");
        builder.CreateCall(strcpyFunc, {bufPtr, selectedStr});
    } else {

        llvm::Value* formatStr;
        if (leftValue->getType()->isDoubleTy() || leftValue->getType()->isFloatTy()) {
            formatStr = builder.CreateGlobalStringPtr("%g");
        } else {
            formatStr = builder.CreateGlobalStringPtr("%d");
        }
        builder.CreateCall(sprintfFunc, {bufPtr, formatStr, leftValue});
    }
    

    if (rightIsString) {

        builder.CreateCall(strcatFunc, {bufPtr, rightValue});
    } else if (rightValue->getType()->isIntegerTy(1)) {

        llvm::Value* trueStr = builder.CreateGlobalStringPtr("true");
        llvm::Value* falseStr = builder.CreateGlobalStringPtr("false");
        llvm::Value* selectedStr = builder.CreateSelect(rightValue, trueStr, falseStr, "bool_str");
        builder.CreateCall(strcatFunc, {bufPtr, selectedStr});
    } else {

        
        llvm::Value* tempBufSize = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 32);
        llvm::Value* tempBufPtr = builder.CreateCall(mallocFunc, {tempBufSize}, "temp_heap_buf");
        
        llvm::Value* formatStr;
        if (rightValue->getType()->isDoubleTy() || rightValue->getType()->isFloatTy()) {
            formatStr = builder.CreateGlobalStringPtr("%g");
        } else {
            formatStr = builder.CreateGlobalStringPtr("%d");
        }
        builder.CreateCall(sprintfFunc, {tempBufPtr, formatStr, rightValue});
        builder.CreateCall(strcatFunc, {bufPtr, tempBufPtr});
        

        llvm::Function* freeFunc = module.getFunction("free");
        if (!freeFunc) {
            llvm::FunctionType* freeType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(ctx),
                {llvm::erType::get(llvm::Type::getInt8Ty(ctx), 0)},
                false
            );
            freeFunc = llvm::Function::Create(
                freeType,
                llvm::Function::ExternalLinkage,
                "free",
                module
            );
        }
        builder.CreateCall(freeFunc, {tempBufPtr});
    }
    
    lastValue = bufPtr;
}