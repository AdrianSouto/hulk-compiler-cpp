#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/LetExprNode.hpp"
#include "Expressions/AssignmentNode.hpp"
#include "Expressions/BlockExprNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>


static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(LetExprNode& node) {
    
    localVarsStack.push_back(localVarsStack.empty() ? std::map<std::string, llvm::AllocaInst*>() : localVarsStack.back());

    
    for (const auto& decl : node.getDeclarations()) {
        decl.expr->accept(*this);
        llvm::Value* val = lastValue;

        
        if (auto typeInst = dynamic_cast<TypeInstantiationNode*>(decl.expr)) {
            variableTypes[decl.id] = typeInst->typeName;
        }

        llvm::Function* func = builder.GetInsertBlock()->getParent();
        llvm::AllocaInst* alloca = createEntryBlockAlloca(func, val->getType(), decl.id);
        builder.CreateStore(val, alloca);
        localVarsStack.back()[decl.id] = alloca;
    }

    
    node.getBody()->accept(*this);
    llvm::Value* result = lastValue; 

    
    localVarsStack.pop_back();

    
    lastValue = result;
}

void LLVMCodegenVisitor::visit(AssignmentNode& node) {
    
    node.value->accept(*this);
    llvm::Value* val = lastValue;

    
    llvm::AllocaInst* alloca = nullptr;
    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto found = it->find(node.identifier);
        if (found != it->end()) {
            alloca = found->second;
            break;
        }
    }

    if (alloca) {
        
        builder.CreateStore(val, alloca);
        
        lastValue = val;
    } else {
        
        lastValue = nullptr;
    }
}

void LLVMCodegenVisitor::visit(BlockExprNode& node) {
    
    llvm::Value* blockResult = nullptr;
    
    for (auto expr : node.expressions) {
        expr->accept(*this);
        
        if (lastValue) {
            blockResult = lastValue;
        }
    }
    
    
    lastValue = blockResult;
}