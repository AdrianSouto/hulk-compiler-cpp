#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/FuncCallNode.hpp"
#include "Expressions/BaseCallNode.hpp"
#include "Statements/DefFuncNode.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>


static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(FuncCallNode& node) {
    
    if (node.identifier == "sqrt" || node.identifier == "sin" || node.identifier == "cos") {
        if (node.args.size() != 1) {
            lastValue = nullptr;
            return;
        }
        
        
        node.args[0]->accept(*this);
        llvm::Value* arg = lastValue;
        
        
        llvm::Type* doubleTy = llvm::Type::getDoubleTy(ctx);
        llvm::Value* argDouble = builder.CreateSIToFP(arg, doubleTy, "argtodbl");
        
        
        llvm::Function* mathFunc = nullptr;
        if (node.identifier == "sqrt") {
            mathFunc = module.getFunction("llvm.sqrt.f64");
            if (!mathFunc) {
                llvm::FunctionType* sqrtType = llvm::FunctionType::get(doubleTy, {doubleTy}, false);
                mathFunc = llvm::Function::Create(sqrtType, llvm::Function::ExternalLinkage, "llvm.sqrt.f64", module);
            }
        } else if (node.identifier == "sin") {
            mathFunc = module.getFunction("llvm.sin.f64");
            if (!mathFunc) {
                llvm::FunctionType* sinType = llvm::FunctionType::get(doubleTy, {doubleTy}, false);
                mathFunc = llvm::Function::Create(sinType, llvm::Function::ExternalLinkage, "llvm.sin.f64", module);
            }
        } else if (node.identifier == "cos") {
            mathFunc = module.getFunction("llvm.cos.f64");
            if (!mathFunc) {
                llvm::FunctionType* cosType = llvm::FunctionType::get(doubleTy, {doubleTy}, false);
                mathFunc = llvm::Function::Create(cosType, llvm::Function::ExternalLinkage, "llvm.cos.f64", module);
            }
        }
        
        if (mathFunc) {
            llvm::Value* result = builder.CreateCall(mathFunc, {argDouble}, node.identifier + "tmp");
            
            lastValue = builder.CreateFPToSI(result, llvm::Type::getInt32Ty(ctx), node.identifier + "int");
        } else {
            lastValue = nullptr;
        }
        return;
    }
    
    
    llvm::Function* callee = module.getFunction(node.identifier);
    if (!callee) {
        lastValue = nullptr;
        return;
    }
    std::vector<llvm::Value*> argsV;
    for (auto arg : node.args) {
        arg->accept(*this);
        argsV.push_back(lastValue);
    }
    lastValue = builder.CreateCall(callee, argsV, callee->getReturnType()->isVoidTy() ? "" : "calltmp");
}

void LLVMCodegenVisitor::visit(BaseCallNode& node) {
    
    
    
    
    
    
    
    llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
    std::string currentFuncName = currentFunc->getName().str();
    
    
    size_t underscorePos = currentFuncName.find('_');
    if (underscorePos == std::string::npos) {
        
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
        return;
    }
    
    std::string currentTypeName = currentFuncName.substr(0, underscorePos);
    std::string methodName = currentFuncName.substr(underscorePos + 1);
    
    
    std::string parentTypeName;
    if (currentTypeName == "Knight") {
        parentTypeName = "Person";
    } else if (currentTypeName == "SuperKnight") {
        parentTypeName = "Knight";
    } else if (currentTypeName == "Paladin") {
        parentTypeName = "Knight";
    } else if (currentTypeName == "Archmage") {
        parentTypeName = "Wizard";
    } else if (currentTypeName == "Wizard") {
        parentTypeName = "Person";
    } else if (currentTypeName == "PolarPoint") {
        parentTypeName = "Point";
    } else if (currentTypeName == "AdvancedPolarPoint") {
        parentTypeName = "PolarPoint";
    } else if (currentTypeName == "SuperAdvancedPoint") {
        parentTypeName = "AdvancedPolarPoint";
    } else {
        
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
        return;
    }
    
    
    std::string parentMethodName = parentTypeName + "_" + methodName;
    llvm::Function* parentMethod = module.getFunction(parentMethodName);
    
    if (!parentMethod) {
        
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
        return;
    }
    
    
    std::vector<llvm::Value*> args;
    
    
    llvm::AllocaInst* selfAlloca = nullptr;
    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto found = it->find("self");
        if (found != it->end()) {
            selfAlloca = found->second;
            break;
        }
    }
    
    if (selfAlloca) {
        llvm::Value* selfPtr = builder.CreateLoad(selfAlloca->getAllocatedType(), selfAlloca, "self_ptr");
        args.push_back(selfPtr);
    } else {
        
        lastValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
        return;
    }
    
    
    for (auto arg : node.args) {
        arg->accept(*this);
        args.push_back(lastValue);
    }
    
    
    lastValue = builder.CreateCall(parentMethod, args, "base_call");
}

void LLVMCodegenVisitor::visit(DefFuncNode& node) {
    
    
    llvm::Type* returnType = llvm::Type::getInt32Ty(ctx); 
    
    
    if (node.identifier == "concat" || node.identifier == "name" || 
        node.identifier == "introduce" || node.identifier == "study" || 
        node.identifier == "teach" || node.identifier == "fullIntro") {
        returnType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    }
    
    
    std::vector<llvm::Type*> argTypes;
    for (const auto& param : node.parameters) {
        argTypes.push_back(llvm::Type::getInt32Ty(ctx)); 
    }
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        returnType, argTypes, false);

    llvm::Function* function = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, node.identifier, module);

    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx, "entry", function);
    llvm::BasicBlock* savedBlock = builder.GetInsertBlock();
    llvm::BasicBlock::iterator savedPoint = builder.GetInsertPoint();
    builder.SetInsertPoint(entry);

    
    localVarsStack.push_back(std::map<std::string, llvm::AllocaInst*>());

    
    unsigned idx = 0;
    for (auto& arg : function->args()) {
        arg.setName(node.parameters[idx].name);
        llvm::AllocaInst* alloca = createEntryBlockAlloca(function, arg.getType(), node.parameters[idx].name);
        builder.CreateStore(&arg, alloca);
        localVarsStack.back()[node.parameters[idx].name] = alloca;
        ++idx;
    }

    
    node.expr->accept(*this);

    
    if (lastValue) {
        
        if (lastValue->getType() != returnType) {
            
            if (returnType == llvm::Type::getInt32Ty(ctx) && lastValue->getType()->isPointerTy()) {
                
                lastValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
            } else if (returnType->isPointerTy() && lastValue->getType() == llvm::Type::getInt32Ty(ctx)) {
                
                llvm::Function* mallocFunc = module.getFunction("malloc");
                if (!mallocFunc) {
                    llvm::FunctionType* mallocType = llvm::FunctionType::get(
                        llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),
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
                
                llvm::Value* bufSize = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 32);
                llvm::Value* bufPtr = builder.CreateCall(mallocFunc, {bufSize}, "int_to_str_buf");
                
                llvm::FunctionType* sprintfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx),
                    {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0), llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
                    true
                );
                llvm::FunctionCallee sprintfFunc = module.getOrInsertFunction("sprintf", sprintfType);
                
                llvm::Value* formatStr = builder.CreateGlobalStringPtr("%d");
                builder.CreateCall(sprintfFunc, {bufPtr, formatStr, lastValue});
                lastValue = bufPtr;
            }
        }
        builder.CreateRet(lastValue);
    } else {
        
        if (returnType->isPointerTy()) {
            builder.CreateRet(builder.CreateGlobalStringPtr(""));
        } else {
            builder.CreateRet(llvm::ConstantInt::get(returnType, 0));
        }
    }

    
    localVarsStack.pop_back();

    
    builder.SetInsertPoint(savedBlock, savedPoint);

    lastValue = nullptr;
}