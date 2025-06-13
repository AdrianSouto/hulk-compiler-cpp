#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Statements/PrintStatementNode.hpp"
#include "Statements/LetVarNode.hpp"
#include "Statements/BlockNode.hpp"
#include "Statements/ExpressionStatementNode.hpp"
#include "AST/Program.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

// Utility function for creating entry block allocas
static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(PrintStatementNode& node) {
    node.expression->accept(*this);
    llvm::Value* val = lastValue;

    // Declarar printf si no existe
    llvm::Function* printfFunc = module.getFunction("printf");
    if (!printfFunc) {
        std::vector<llvm::Type*> printfArgs;
        printfArgs.push_back(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(ctx), printfArgs, true);
        printfFunc = llvm::Function::Create(
            printfType, llvm::Function::ExternalLinkage, "printf", module);
    }

    // Crear formato adecuado según el tipo
    llvm::Value* formatStr;

    if (val->getType()->isPointerTy()) {
        // Si es un puntero (probablemente una cadena)
        formatStr = builder.CreateGlobalStringPtr("%s\n");
    } else if (val->getType()->isIntegerTy(1)) {
        // Si es un booleano (i1 en LLVM)
        // Convertir el valor booleano a una cadena "true" o "false"
        llvm::Value* trueStr = builder.CreateGlobalStringPtr("true");
        llvm::Value* falseStr = builder.CreateGlobalStringPtr("false");
        // Seleccionar la cadena correcta según el valor booleano
        val = builder.CreateSelect(val, trueStr, falseStr);
        formatStr = builder.CreateGlobalStringPtr("%s\n");
    } else if (val->getType()->isIntegerTy()) {
        // Si es otro tipo de entero
        formatStr = builder.CreateGlobalStringPtr("%d\n");
    } else {
        // Manejo de otros tipos (opcional)
        formatStr = builder.CreateGlobalStringPtr("Unsupported type\n");
    }

    builder.CreateCall(printfFunc, {formatStr, val});
    lastValue = nullptr;
}

void LLVMCodegenVisitor::visit(LetVarNode& node) {
    // Nuevo scope de variables locales
    localVarsStack.push_back(localVarsStack.empty() ? std::map<std::string, llvm::AllocaInst*>() : localVarsStack.back());

    // Evaluar expresión y asignar a variable local
    node.expr->accept(*this);
    llvm::Value* val = lastValue;

    // Track the type of the variable
    if (auto typeInst = dynamic_cast<TypeInstantiationNode*>(node.expr)) {
        variableTypes[node.identifier] = typeInst->typeName;
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

    // Ejecutar el cuerpo
    node.body->accept(*this);
    // Keep the lastValue from the body (don't set it to nullptr)
    llvm::Value* bodyResult = lastValue;

    // Salir del scope
    localVarsStack.pop_back();
    
    // Return the body's result value
    lastValue = bodyResult;
}

void LLVMCodegenVisitor::visit(BlockNode& node) {
    // A block's value is the value of its last expression
    llvm::Value* blockResult = nullptr;
    
    for (auto stmt : node.statements) {
        stmt->accept(*this);
        // Keep track of the last expression value
        if (lastValue) {
            blockResult = lastValue;
        }
    }
    
    // The block returns the last expression value (or nullptr if no expressions)
    lastValue = blockResult;
}

void LLVMCodegenVisitor::visit(ExpressionStatementNode& node) {
    // Simply visit the expression
    if (node.expression) {
        node.expression->accept(*this);
    }
    // lastValue is set by the expression's accept method
}

void LLVMCodegenVisitor::visit(Program& node) {
    // Create a 'main' function with signature: i32 main()
    // This function will serve as the entry point for the program's top-level statements.
    llvm::FunctionType* mainFuncType = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", module);

    // Create an entry basic block for the main function.
    // Instructions for top-level statements will be inserted here.
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(ctx, "entry", mainFunc);
    builder.SetInsertPoint(entryBlock);

    // Set up the initial scope for local variables in the main function.
    // Although the current example might not use local variables at the top level,
    // this maintains consistency with how scopes are handled for other constructs like functions or let-expressions.
    localVarsStack.clear(); // Clear any prior state from other potential visitor uses (if any).
    localVarsStack.push_back(std::map<std::string, llvm::AllocaInst*>()); // Push the global/main scope.

    // Visit each statement in the program.
    for (StatementNode* stmt : node.Statements) {
        stmt->accept(*this);
    }

    // Ensure the main function has a return statement.
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