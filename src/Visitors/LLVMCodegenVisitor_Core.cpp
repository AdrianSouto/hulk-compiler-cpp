#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <iostream>


LLVMCodegenVisitor::LLVMCodegenVisitor(llvm::LLVMContext& context, llvm::IRBuilder<>& b, llvm::Module& m)
    : ctx(context), builder(b), module(m), lastValue(nullptr) {
}


static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}