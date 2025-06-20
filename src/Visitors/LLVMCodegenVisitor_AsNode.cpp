#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/AsNode.hpp"
#include "Expressions/IsNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Expressions/TypeInstantiationNode.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Types/RuntimeTypeInfo.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <iostream>



// Runtime cast function that performs type checking and casting
llvm::Function* createRuntimeCastFunction(llvm::Module& module, llvm::LLVMContext& ctx) {
    // Check if function already exists
    llvm::Function* existingFunc = module.getFunction("runtime_cast");
    if (existingFunc) {
        return existingFunc;
    }
    
    // Function signature: i8* @runtime_cast(i8* obj_ptr, i8* target_type_name)
    std::vector<llvm::Type*> paramTypes = {
        llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0), // object pointer
        llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)  // target type name
    };
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0), paramTypes, false);
    
    llvm::Function* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, "runtime_cast", module);
    
    // Create function body
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(ctx, "entry", func);
    llvm::BasicBlock* nullCheckBB = llvm::BasicBlock::Create(ctx, "null_check", func);
    llvm::BasicBlock* successBB = llvm::BasicBlock::Create(ctx, "success", func);
    llvm::BasicBlock* failureBB = llvm::BasicBlock::Create(ctx, "failure", func);
    
    llvm::IRBuilder<> builder(entryBB);
    
    auto args = func->arg_begin();
    llvm::Value* objPtr = &*args++;
    llvm::Value* targetTypeName = &*args;
    
    // Check if object is null
    llvm::Value* isNull = builder.CreateIsNull(objPtr, "is_null");
    builder.CreateCondBr(isNull, failureBB, nullCheckBB);
    
    // For now, implement a simplified version that always succeeds for non-null objects
    // In a full implementation, this would extract type info and traverse the hierarchy
    builder.SetInsertPoint(nullCheckBB);
    builder.CreateBr(successBB);
    
    // Cast successful - return the object pointer
    builder.SetInsertPoint(successBB);
    builder.CreateRet(objPtr);
    
    // Cast failed - print error and exit
    builder.SetInsertPoint(failureBB);
    
    // Get printf and exit functions
    llvm::Function* printfFunc = module.getFunction("printf");
    if (!printfFunc) {
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(ctx),
            {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
            true
        );
        printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);
    }
    
    llvm::Function* exitFunc = module.getFunction("exit");
    if (!exitFunc) {
        llvm::FunctionType* exitType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(ctx),
            {llvm::Type::getInt32Ty(ctx)},
            false
        );
        exitFunc = llvm::Function::Create(exitType, llvm::Function::ExternalLinkage, "exit", module);
    }
    
    // Print error message
    llvm::Value* errorStr = builder.CreateGlobalStringPtr("Runtime error: Invalid cast operation\\n", "cast_error");
    builder.CreateCall(printfFunc, {errorStr});
    builder.CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
    
    // Return null (unreachable)
    builder.CreateRet(llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)));
    
    return func;
}

void LLVMCodegenVisitor::visit(AsNode& node) {
    node.expression->accept(*this);
    llvm::Value* exprValue = lastValue;
    
    if (!exprValue) {
        std::cerr << "Error: Invalid expression in 'as' cast" << std::endl;
        lastValue = nullptr;
        return;
    }

    // Determine expression type
    std::string exprTypeName = "";
    
    if (auto varNode = dynamic_cast<VariableNode*>(node.expression)) {
        auto typeIt = variableTypes.find(varNode->identifier);
        if (typeIt != variableTypes.end()) {
            exprTypeName = typeIt->second;
        }
    } else if (auto typeInstNode = dynamic_cast<TypeInstantiationNode*>(node.expression)) {
        exprTypeName = typeInstNode->typeName;
    } else {
        // Handle built-in types
        llvm::Type* llvmType = exprValue->getType();
        if (llvmType->isDoubleTy() || llvmType->isIntegerTy(32)) {
            exprTypeName = "Number";
        } else if (llvmType->isIntegerTy(1)) {
            exprTypeName = "Boolean";
        } else if (llvmType->isPointerTy()) {
            exprTypeName = "String"; // Default assumption
        }
    }

    // Handle built-in type casts
    if (exprTypeName == "Number" || exprTypeName == "Boolean" || exprTypeName == "String") {
        // Check if cast is valid
        bool canCast = false;
        
        if (exprTypeName == node.typeName) {
            canCast = true;
        } else if (node.typeName == "Object") {
            // All types can be cast to Object
            canCast = true;
        } else {
            // Check for valid built-in type conversions
            if ((exprTypeName == "Number" && (node.typeName == "Boolean" || node.typeName == "String")) ||
                (exprTypeName == "Boolean" && (node.typeName == "Number" || node.typeName == "String")) ||
                (exprTypeName == "String" && (node.typeName == "Number" || node.typeName == "Boolean"))) {
                canCast = true;
            }
        }
        
        if (canCast) {
            // Perform type conversion if needed
            llvm::Type* targetType = nullptr;
            if (node.typeName == "Number") {
                targetType = llvm::Type::getDoubleTy(ctx);
            } else if (node.typeName == "Boolean") {
                targetType = llvm::Type::getInt1Ty(ctx);
            } else if (node.typeName == "String") {
                targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
            } else if (node.typeName == "Object") {
                targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
            }
            
            if (targetType && exprValue->getType() != targetType) {
                // Perform necessary conversions
                if (exprValue->getType()->isDoubleTy() && targetType->isIntegerTy(1)) {
                    // Number to Boolean
                    llvm::Value* zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(ctx), 0.0);
                    lastValue = builder.CreateFCmpONE(exprValue, zero, "num_to_bool");
                } else if (exprValue->getType()->isIntegerTy(1) && targetType->isDoubleTy()) {
                    // Boolean to Number
                    lastValue = builder.CreateUIToFP(exprValue, targetType, "bool_to_num");
                } else if (exprValue->getType()->isPointerTy() && targetType->isPointerTy()) {
                    // Pointer cast
                    lastValue = builder.CreateBitCast(exprValue, targetType, "ptr_cast");
                } else {
                    lastValue = exprValue;
                }
            } else {
                lastValue = exprValue;
            }
        } else {
            // Invalid cast - generate runtime error
            llvm::Function* printfFunc = module.getFunction("printf");
            if (!printfFunc) {
                llvm::FunctionType* printfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx),
                    {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
                    true
                );
                printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);
            }
            
            llvm::Function* exitFunc = module.getFunction("exit");
            if (!exitFunc) {
                llvm::FunctionType* exitType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(ctx),
                    {llvm::Type::getInt32Ty(ctx)},
                    false
                );
                exitFunc = llvm::Function::Create(exitType, llvm::Function::ExternalLinkage, "exit", module);
            }
            
            std::string errorMsg = "Runtime error: Cannot cast " + exprTypeName + " to " + node.typeName + "\\n";
            llvm::Value* errorStr = builder.CreateGlobalStringPtr(errorMsg, "cast_error_msg");
            
            builder.CreateCall(printfFunc, {errorStr});
            builder.CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
            
            lastValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
        }
        return;
    }

    // For custom types, use compile-time hierarchy checking for now
    bool canCast = false;
    
    if (exprTypeName == node.typeName) {
        canCast = true;
    } else if (!exprTypeName.empty()) {
        // Check inheritance hierarchy using compile-time information
        auto typeIt = types.find(exprTypeName);
        if (typeIt != types.end()) {
            TypeDefNode* currentType = typeIt->second;
            
            std::string currentParentName = currentType->parentTypeName;
            while (!currentParentName.empty() && !canCast) {
                if (currentParentName == node.typeName) {
                    canCast = true;
                    break;
                }
                
                auto parentTypeIt = types.find(currentParentName);
                if (parentTypeIt != types.end()) {
                    currentParentName = parentTypeIt->second->parentTypeName;
                } else {
                    break;
                }
            }
        }
        
        // Also check the reverse - if the expression type is a parent of the target type
        if (!canCast) {
            auto targetTypeIt = types.find(node.typeName);
            if (targetTypeIt != types.end()) {
                TypeDefNode* targetType = targetTypeIt->second;
                
                std::string currentParentName = targetType->parentTypeName;
                while (!currentParentName.empty() && !canCast) {
                    if (currentParentName == exprTypeName) {
                        canCast = true;
                        break;
                    }
                    
                    auto parentTypeIt = types.find(currentParentName);
                    if (parentTypeIt != types.end()) {
                        currentParentName = parentTypeIt->second->parentTypeName;
                    } else {
                        break;
                    }
                }
            }
        }
    }

    if (canCast) {
        // Perform the cast (for now, just a bitcast)
        llvm::Type* targetType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
        
        if (exprValue->getType() != targetType) {
            lastValue = builder.CreateBitCast(exprValue, targetType, "static_cast");
        } else {
            lastValue = exprValue;
        }
    } else {
        // Invalid cast - generate runtime error
        llvm::Function* printfFunc = module.getFunction("printf");
        if (!printfFunc) {
            llvm::FunctionType* printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx),
                {llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0)},
                true
            );
            printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module);
        }
        
        llvm::Function* exitFunc = module.getFunction("exit");
        if (!exitFunc) {
            llvm::FunctionType* exitType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(ctx),
                {llvm::Type::getInt32Ty(ctx)},
                false
            );
            exitFunc = llvm::Function::Create(exitType, llvm::Function::ExternalLinkage, "exit", module);
        }
        
        std::string errorMsg = "Runtime error: Cannot cast " + exprTypeName + " to " + node.typeName + "\\n";
        llvm::Value* errorStr = builder.CreateGlobalStringPtr(errorMsg, "cast_error_msg");
        
        builder.CreateCall(printfFunc, {errorStr});
        builder.CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
        
        lastValue = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0));
    }
    
    std::cerr << "DEBUG: Enhanced AsNode - Expression type: '" << exprTypeName 
              << "', Target type: '" << node.typeName 
              << "', Can cast: " << (canCast ? "true" : "false") << std::endl;
}