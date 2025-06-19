#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Statements/PrintStatementNode.hpp"
#include "Statements/LetVarNode.hpp"
#include "Statements/BlockNode.hpp"
#include "Statements/ExpressionStatementNode.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "AST/Program.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <iostream>
#include <typeinfo>


static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(PrintStatementNode& node) {
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
        // For printing doubles, we'll convert to int if it's a whole number
        // This is a simple approach - you might want to make this more sophisticated
        formatStr = builder.CreateGlobalStringPtr("%.6g\n");
    } else {

        formatStr = builder.CreateGlobalStringPtr("Unsupported type\n");
    }

    builder.CreateCall(printfFunc, {formatStr, val});
    lastValue = nullptr;
}

void LLVMCodegenVisitor::visit(LetVarNode& node) {

    localVarsStack.push_back(localVarsStack.empty() ? std::map<std::string, llvm::AllocaInst*>() : localVarsStack.back());

    node.expr->accept(*this);
    llvm::Value* val = lastValue;


    if (auto typeInst = dynamic_cast<TypeInstantiationNode*>(node.expr)) {
        variableTypes[node.identifier] = typeInst->typeName;
        std::cerr << "DEBUG: Tracking variable '" << node.identifier << "' as type '" << typeInst->typeName << "'" << std::endl;
    } else if (dynamic_cast<NumberNode*>(node.expr)) {
        variableTypes[node.identifier] = "Number";
    } else if (dynamic_cast<StringLiteralNode*>(node.expr)) {
        variableTypes[node.identifier] = "String";
    } else if (dynamic_cast<BooleanNode*>(node.expr)) {
        variableTypes[node.identifier] = "Boolean";
    }

    llvm::Function* func = builder.GetInsertBlock()->getParent();
    llvm::AllocaInst* alloca = createEntryBlockAlloca(func, val->getType(), node.identifier);
    builder.CreateStore(val, alloca);
    localVarsStack.back()[node.identifier] = alloca;


    node.body->accept(*this);

    llvm::Value* bodyResult = lastValue;


    localVarsStack.pop_back();
    

    lastValue = bodyResult;
}

void LLVMCodegenVisitor::visit(BlockNode& node) {

    llvm::Value* blockResult = nullptr;
    
    for (auto stmt : node.statements) {
        stmt->accept(*this);

        if (lastValue) {
            blockResult = lastValue;
        }
    }
    

    lastValue = blockResult;
}

void LLVMCodegenVisitor::visit(ExpressionStatementNode& node) {

    if (node.expression) {
        node.expression->accept(*this);
    }

}

void LLVMCodegenVisitor::visit(Program& node) {
    // First pass: Process type definitions (they generate their own functions)
    for (StatementNode* stmt : node.Statements) {
        if (dynamic_cast<TypeDefNode*>(stmt)) {
            std::cerr << "DEBUG: Processing TypeDefNode in first pass" << std::endl;
            stmt->accept(*this);
        }
    }
    
    // Second pass: Forward declare all functions
    for (StatementNode* stmt : node.Statements) {
        if (DefFuncNode* funcNode = dynamic_cast<DefFuncNode*>(stmt)) {
            // Create function declaration
            std::vector<llvm::Type*> paramTypes;
            for (const auto& param : funcNode->parameters) {
                // Default to double for now
                paramTypes.push_back(llvm::Type::getDoubleTy(ctx));
            }
            
            llvm::Type* returnType = llvm::Type::getDoubleTy(ctx);
            llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
            
            // Just create the function declaration, don't generate body yet
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, funcNode->identifier, module);
        }
    }

    llvm::FunctionType* mainFuncType = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", module);


    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(ctx, "entry", mainFunc);
    builder.SetInsertPoint(entryBlock);




    localVarsStack.clear();
    localVarsStack.push_back(std::map<std::string, llvm::AllocaInst*>());


    for (StatementNode* stmt : node.Statements) {
        // Skip type definitions as they were already processed
        if (dynamic_cast<TypeDefNode*>(stmt)) {
            continue;
        }
        std::cerr << "DEBUG: Processing statement type: " << typeid(*stmt).name() << std::endl;
        stmt->accept(*this);
    }


    llvm::BasicBlock* currentBlock = builder.GetInsertBlock();
    if (currentBlock && currentBlock->getParent() == mainFunc) {
        if (!currentBlock->getTerminator()) {
            builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0, true));
        }
    } else {
        if (entryBlock->getTerminator() == nullptr) {
            builder.SetInsertPoint(entryBlock);
            builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0, true));
        }
    }

    if (!localVarsStack.empty()) {
        localVarsStack.pop_back();
    }
}