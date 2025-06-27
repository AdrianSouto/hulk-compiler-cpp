#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Expressions/VariableNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

void LLVMCodegenVisitor::visit(NumberNode& node) {
    if (node.isInteger) {
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), static_cast<int>(node.value), true);
    } else {
        lastValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), node.value);
    }
}

void LLVMCodegenVisitor::visit(StringLiteralNode& node) {
    lastValue = builder.CreateGlobalStringPtr(node.value);
}

void LLVMCodegenVisitor::visit(BooleanNode& node) {
    lastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(ctx), node.value ? 1 : 0);
}

void LLVMCodegenVisitor::visit(VariableNode& node) {
    if (node.identifier == "PI") {
        lastValue = llvm::ConstantFP::get(builder.getContext(), llvm::APFloat(3.141592653589793));
        return;
    }

    llvm::AllocaInst* alloca = nullptr;
    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto found = it->find(node.identifier);
        if (found != it->end()) {
            alloca = found->second;
            break;
        }
    }
    if (alloca) {
        lastValue = builder.CreateLoad(alloca->getAllocatedType(), alloca, node.identifier);
    } else {
        lastValue = nullptr;
    }
}