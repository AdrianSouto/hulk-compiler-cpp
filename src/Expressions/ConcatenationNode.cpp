#include "Expressions/ConcatenationNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>

ConcatenationNode::ConcatenationNode(ExpressionNode* left, ExpressionNode* right)
    : BinaryOperatorNode(left, right) {}

int ConcatenationNode::evaluate() const {
    
    try {
        return std::stoi(evaluateString());
    } catch (...) {
        return 0; 
    }
}

std::string ConcatenationNode::evaluateString() const {
    return left->evaluateString() + right->evaluateString();
}

char ConcatenationNode::getOperator() const {
    return '@';
}

void ConcatenationNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

llvm::Value* ConcatenationNode::CodeGen(LLVMCodegenVisitor& visitor) {
    llvm::Module* module = &visitor.module;
    llvm::LLVMContext& context = visitor.ctx;
    llvm::IRBuilder<>& builder = visitor.builder;

    
    left->accept(visitor);
    llvm::Value* leftValue = visitor.lastValue;

    right->accept(visitor);
    llvm::Value* rightValue = visitor.lastValue;

    
    bool isLeftString = leftValue->getType()->iserTy();
    bool isRightString = rightValue->getType()->iserTy();
    
    
    
    llvm::Constant* emptyStr = builder.CreateGlobalString("", "concatbuf");
    llvm::Value* resultBuf = builder.CreateerCast(
        emptyStr, llvm::erType::get(llvm::Type::getInt8Ty(context), 0));
    
    
    llvm::FunctionType* strcpyType = llvm::FunctionType::get(
        llvm::erType::get(llvm::Type::getInt8Ty(context), 0),
        {llvm::erType::get(llvm::Type::getInt8Ty(context), 0),
         llvm::erType::get(llvm::Type::getInt8Ty(context), 0)},
        false
    );
    llvm::Function* strcpyFunc = llvm::cast<llvm::Function>(
        module->getOrInsertFunction("strcpy", strcpyType).getCallee()
    );
    
    llvm::FunctionType* strcatType = llvm::FunctionType::get(
        llvm::erType::get(llvm::Type::getInt8Ty(context), 0),
        {llvm::erType::get(llvm::Type::getInt8Ty(context), 0),
         llvm::erType::get(llvm::Type::getInt8Ty(context), 0)},
        false
    );
    llvm::Function* strcatFunc = llvm::cast<llvm::Function>(
        module->getOrInsertFunction("strcat", strcatType).getCallee()
    );
    
    
    if (isLeftString) {
        
        builder.CreateCall(strcpyFunc, {resultBuf, leftValue});
    } else {
        
        
        std::string numStr = std::to_string(
            llvm::cast<llvm::ConstantInt>(leftValue)->getSExtValue()
        );
        
        llvm::Constant* numStrConst = builder.CreateGlobalString(numStr, "num_str");
        llvm::Value* numStrVal = builder.CreateerCast(
            numStrConst, llvm::erType::get(llvm::Type::getInt8Ty(context), 0));
        builder.CreateCall(strcpyFunc, {resultBuf, numStrVal});
    }
    
    
    if (isRightString) {
        
        builder.CreateCall(strcatFunc, {resultBuf, rightValue});
    } else {
        
        std::string numStr = std::to_string(
            llvm::cast<llvm::ConstantInt>(rightValue)->getSExtValue()
        );
        
        llvm::Constant* numStrConst = builder.CreateGlobalString(numStr, "num_str2");
        llvm::Value* numStrVal = builder.CreateerCast(
            numStrConst, llvm::erType::get(llvm::Type::getInt8Ty(context), 0));
        builder.CreateCall(strcatFunc, {resultBuf, numStrVal});
    }
    
    return resultBuf;
}
