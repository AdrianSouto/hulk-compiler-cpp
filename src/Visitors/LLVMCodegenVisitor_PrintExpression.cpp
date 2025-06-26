#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/PrintExpressionNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

void LLVMCodegenVisitor::visit(PrintExpressionNode& node) {

    node.expression->accept(*this);
    llvm::Value* val = lastValue;


    llvm::Function* printfFunc = module.getFunction("printf");
    if (!printfFunc) {
        std::vector<llvm::Type*> printfArgs;
        printfArgs.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(ctx), printfArgs, true);
        printfFunc = llvm::Function::Create(
            printfType, llvm::Function::ExternalLinkage, "printf", module);
    }


    llvm::Value* formatStr;

    if (val->getType()->isPointerTy()) {

        formatStr = builder.CreateGlobalStringPtr("%s\n");
    } else if (val->getType()->isIntegerTy(1)) {

        llvm::Value* trueStr = builder.CreateGlobalStringPtr("true");
        llvm::Value* falseStr = builder.CreateGlobalStringPtr("false");
        
        val = builder.CreateSelect(val, trueStr, falseStr);
        formatStr = builder.CreateGlobalStringPtr("%s\n");
    } else if (val->getType()->isIntegerTy()) {

        formatStr = builder.CreateGlobalStringPtr("%d\n");
    } else if (val->getType()->isDoubleTy()) {

        formatStr = builder.CreateGlobalStringPtr("%.6g\n");
    } else {

        formatStr = builder.CreateGlobalStringPtr("Unsupported type\n");
    }


    builder.CreateCall(printfFunc, {formatStr, val});
    




}