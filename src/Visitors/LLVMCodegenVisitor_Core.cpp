#include "Visitors/LLVMCodegenVisitor.hpp"


LLVMCodegenVisitor::LLVMCodegenVisitor(llvm::LLVMContext& context, llvm::IRBuilder<>& b, llvm::Module& m)
    : ctx(context), builder(b), module(m), lastValue(nullptr) {
}