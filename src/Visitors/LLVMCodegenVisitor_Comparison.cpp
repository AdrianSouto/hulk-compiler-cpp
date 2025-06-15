#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/EqualNode.hpp"
#include "Expressions/NotEqualNode.hpp"
#include "Expressions/GreaterNode.hpp"
#include "Expressions/GreaterEqNode.hpp"
#include "Expressions/LessNode.hpp"
#include "Expressions/LessEqNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>


static std::pair<llvm::Value*, llvm::Value*> convertToCompatibleTypes(
    llvm::Value* l, llvm::Value* r, llvm::IRBuilder<>& builder) {
    
    if (l->getType() == r->getType()) {
        return {l, r};
    }
    

    if (l->getType()->isIntegerTy() && r->getType()->isIntegerTy()) {
        if (l->getType()->getIntegerBitWidth() < r->getType()->getIntegerBitWidth()) {
            l = builder.CreateZExt(l, r->getType(), "conv_to_larger_int");
        } else if (r->getType()->getIntegerBitWidth() < l->getType()->getIntegerBitWidth()) {
            r = builder.CreateZExt(r, l->getType(), "conv_to_larger_int");
        }
        return {l, r};
    }
    

    if ((l->getType()->isIntegerTy() && r->getType()->isDoubleTy()) ||
        (l->getType()->isDoubleTy() && r->getType()->isIntegerTy())) {
        
        if (l->getType()->isIntegerTy()) {
            l = builder.CreateSIToFP(l, llvm::Type::getDoubleTy(builder.getContext()), "int_to_double");
        }
        if (r->getType()->isIntegerTy()) {
            r = builder.CreateSIToFP(r, llvm::Type::getDoubleTy(builder.getContext()), "int_to_double");
        }
        return {l, r};
    }
    

    if (l->getType()->isDoubleTy() && r->getType()->isDoubleTy()) {
        return {l, r};
    }
    

    if (!l->getType()->isIntegerTy()) {
        if (l->getType()->isDoubleTy()) {
            l = builder.CreateFPToSI(l, llvm::Type::getInt32Ty(builder.getContext()), "double_to_int");
        } else {
            l = builder.CreatePtrToInt(l, llvm::Type::getInt32Ty(builder.getContext()), "ptr_to_int");
        }
    }
    if (!r->getType()->isIntegerTy()) {
        if (r->getType()->isDoubleTy()) {
            r = builder.CreateFPToSI(r, llvm::Type::getInt32Ty(builder.getContext()), "double_to_int");
        } else {
            r = builder.CreatePtrToInt(r, llvm::Type::getInt32Ty(builder.getContext()), "ptr_to_int");
        }
    }
    
    return {l, r};
}

void LLVMCodegenVisitor::visit(EqualNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;


    auto converted = convertToCompatibleTypes(l, r, builder);
    l = converted.first;
    r = converted.second;
    

    if (l->getType()->isDoubleTy()) {
        lastValue = builder.CreateFCmpOEQ(l, r, "eqtmp");
    } else {
        lastValue = builder.CreateICmpEQ(l, r, "eqtmp");
    }
}

void LLVMCodegenVisitor::visit(NotEqualNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;


    auto converted = convertToCompatibleTypes(l, r, builder);
    l = converted.first;
    r = converted.second;
    

    if (l->getType()->isDoubleTy()) {
        lastValue = builder.CreateFCmpONE(l, r, "netmp");
    } else {
        lastValue = builder.CreateICmpNE(l, r, "netmp");
    }
}

void LLVMCodegenVisitor::visit(GreaterNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;


    auto converted = convertToCompatibleTypes(l, r, builder);
    l = converted.first;
    r = converted.second;
    

    if (l->getType()->isDoubleTy()) {
        lastValue = builder.CreateFCmpOGT(l, r, "gttmp");
    } else {
        lastValue = builder.CreateICmpSGT(l, r, "gttmp");
    }
}

void LLVMCodegenVisitor::visit(GreaterEqNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;


    auto converted = convertToCompatibleTypes(l, r, builder);
    l = converted.first;
    r = converted.second;
    

    if (l->getType()->isDoubleTy()) {
        lastValue = builder.CreateFCmpOGE(l, r, "getmp");
    } else {
        lastValue = builder.CreateICmpSGE(l, r, "getmp");
    }
}

void LLVMCodegenVisitor::visit(LessNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;


    auto converted = convertToCompatibleTypes(l, r, builder);
    l = converted.first;
    r = converted.second;
    

    if (l->getType()->isDoubleTy()) {
        lastValue = builder.CreateFCmpOLT(l, r, "lttmp");
    } else {
        lastValue = builder.CreateICmpSLT(l, r, "lttmp");
    }
}

void LLVMCodegenVisitor::visit(LessEqNode& node) {
    node.left->accept(*this);
    llvm::Value* l = lastValue;
    node.right->accept(*this);
    llvm::Value* r = lastValue;


    auto converted = convertToCompatibleTypes(l, r, builder);
    l = converted.first;
    r = converted.second;
    

    if (l->getType()->isDoubleTy()) {
        lastValue = builder.CreateFCmpOLE(l, r, "letmp");
    } else {
        lastValue = builder.CreateICmpSLE(l, r, "letmp");
    }
}