#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/FuncCallNode.hpp"
#include "Expressions/BaseCallNode.hpp"
#include "Expressions/ConcatenationNode.hpp"
#include "Statements/DefFuncNode.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Statements/PrintStatementNode.hpp"
#include "AST/Program.hpp"
#include "Globals.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <iostream>

extern Program program;

static llvm::Type* getLLVMTypeFromName(const std::string& typeName, llvm::LLVMContext& ctx) {
    if (typeName == "Number") {
        return llvm::Type::getDoubleTy(ctx);
    }
    if (typeName == "String") {
        return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    }
    if (typeName == "Boolean") {
        return llvm::Type::getInt1Ty(ctx);
    }
    if (typeName == "Object") {
        return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
    }
    if (typeName == "Void" || typeName.empty()) {
        return llvm::Type::getVoidTy(ctx);
    }

    // For user-defined types, check if we have a struct type
    extern std::map<std::string, llvm::StructType*> structTypes;
    auto structIt = structTypes.find(typeName);
    if (structIt != structTypes.end()) {
        return llvm::PointerType::get(structIt->second, 0);
    }

    return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
}

static llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, varName);
}

void LLVMCodegenVisitor::visit(DefFuncNode& node) {

    std::vector<llvm::Type*> paramTypes;
    for (const auto& param : node.parameters) {
        std::string typeName = "Number";
        if (param.type) {
            typeName = param.type->toString();
        }
        llvm::Type* paramType = ::getLLVMTypeFromName(typeName, ctx);
        

        if (param.name == "self" && !currentTypeName.empty()) {


            extern std::map<std::string, llvm::StructType*> structTypes;
            auto structIt = structTypes.find(currentTypeName);
            if (structIt != structTypes.end()) {
                paramType = llvm::PointerType::get(structIt->second, 0);
            } else {
                paramType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
            }
            std::cerr << "DEBUG: Setting 'self' parameter type to pointer for type '" << currentTypeName << "'" << std::endl;
        }
        
        paramTypes.push_back(paramType);
    }
    

    llvm::Type* returnType = llvm::Type::getDoubleTy(ctx);
    if (node.returnType) {
        returnType = ::getLLVMTypeFromName(node.returnType->toString(), ctx);
    } else {
        if (!node.isBlockBody && node.expr) {
            if (dynamic_cast<ConcatenationNode*>(node.expr)) {
                returnType = llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0);
                std::cerr << "DEBUG: Inferred return type as String for function '" << node.identifier << "'" << std::endl;
            }
        }
    }
    

    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    
    // Check if function already exists (from forward declaration)
    llvm::Function* function = module.getFunction(node.identifier);
    
    if (function) {
        // Function already exists, just verify the signature matches
        if (function->getFunctionType() != funcType) {
            std::cerr << "Error: Function '" << node.identifier << "' signature mismatch" << std::endl;
            lastValue = nullptr;
            return;
        }
    } else {
        // Create new function
        function = llvm::Function::Create(
            funcType, 
            llvm::Function::ExternalLinkage, 
            node.identifier, 
            module
        );
    }
    

    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(ctx, "entry", function);
    

    llvm::BasicBlock* prevBB = builder.GetInsertBlock();
    

    builder.SetInsertPoint(entryBB);
    

    localVarsStack.push_back(std::map<std::string, llvm::AllocaInst*>());
    

    auto argIt = function->arg_begin();
    for (size_t i = 0; i < node.parameters.size(); ++i, ++argIt) {
        llvm::Argument* arg = &*argIt;
        arg->setName(node.parameters[i].name);
        

        llvm::AllocaInst* alloca = createEntryBlockAlloca(function, arg->getType(), node.parameters[i].name);
        

        builder.CreateStore(arg, alloca);
        

        localVarsStack.back()[node.parameters[i].name] = alloca;
    }
    
    llvm::Value* returnValue = nullptr;

    if (node.isBlockBody) {

        for (auto statement : node.statements) {
            statement->accept(*this);


            if (dynamic_cast<PrintStatementNode*>(statement) == nullptr) {
                returnValue = lastValue;
            }
        }


    } else {

        node.expr->accept(*this);
        returnValue = lastValue;
    }


    if (returnValue) {
        if (returnType->isVoidTy()) {
            builder.CreateRetVoid();
        } else {

            if (returnValue->getType() != returnType) {
                if (returnValue->getType()->isIntegerTy() && returnType->isIntegerTy()) {

                    if (returnValue->getType()->getIntegerBitWidth() < returnType->getIntegerBitWidth()) {
                        returnValue = builder.CreateZExt(returnValue, returnType, "int_extend");
                    } else if (returnValue->getType()->getIntegerBitWidth() > returnType->getIntegerBitWidth()) {
                        returnValue = builder.CreateTrunc(returnValue, returnType, "int_trunc");
                    }
                } else if (returnValue->getType()->isIntegerTy() && returnType->isDoubleTy()) {

                    returnValue = builder.CreateSIToFP(returnValue, returnType, "int_to_double");
                } else if (returnValue->getType()->isDoubleTy() && returnType->isIntegerTy()) {

                    returnValue = builder.CreateFPToSI(returnValue, returnType, "double_to_int");
                } else if (returnValue->getType()->isPointerTy() && returnType->isPointerTy()) {
                    returnValue = builder.CreateBitCast(returnValue, returnType, "ptr_cast");
                } else if (returnValue->getType()->isPointerTy() && returnType->isDoubleTy()) {
                    // For assignment expressions that return pointers but function expects double,
                    // return a default double value (this is a workaround for setter methods)
                    std::cerr << "DEBUG: Converting pointer return to double for function '" << node.identifier << "'" << std::endl;
                    returnValue = llvm::ConstantFP::get(returnType, 0.0);
                } else {
                    // Try a general bitcast as last resort
                    try {
                        returnValue = builder.CreateBitCast(returnValue, returnType, "general_cast");
                    } catch (...) {
                        // If bitcast fails, use a default value
                        std::cerr << "DEBUG: Using default return value for function '" << node.identifier << "'" << std::endl;
                        if (returnType->isIntegerTy()) {
                            returnValue = llvm::ConstantInt::get(returnType, 0);
                        } else if (returnType->isDoubleTy()) {
                            returnValue = llvm::ConstantFP::get(returnType, 0.0);
                        } else if (returnType->isPointerTy()) {
                            returnValue = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(returnType));
                        }
                    }
                }
            }
            builder.CreateRet(returnValue);
        }
    } else {
        if (returnType->isVoidTy()) {
            builder.CreateRetVoid();
        } else {

            llvm::Value* defaultRet = nullptr;
            if (returnType->isIntegerTy()) {
                defaultRet = llvm::ConstantInt::get(returnType, 0);
            } else if (returnType->isDoubleTy()) {
                defaultRet = llvm::ConstantFP::get(returnType, 0.0);
            } else if (returnType->isPointerTy()) {
                defaultRet = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(returnType));
            }
            builder.CreateRet(defaultRet);
        }
    }
    

    localVarsStack.pop_back();
    

    if (llvm::verifyFunction(*function, &llvm::errs())) {
        std::cerr << "Error: Function verification failed for '" << node.identifier << "'" << std::endl;
        function->eraseFromParent();
        lastValue = nullptr;
    } else {
        lastValue = function;
    }
    

    if (prevBB) {
        builder.SetInsertPoint(prevBB);
    }
}

void LLVMCodegenVisitor::visit(FuncCallNode& node) {

    if (node.identifier == "sqrt" || node.identifier == "sin" || node.identifier == "cos" || 
        node.identifier == "exp" || node.identifier == "log" || node.identifier == "pow" ||
        node.identifier == "rand") {

        llvm::Type* doubleTy = llvm::Type::getDoubleTy(ctx);
        std::vector<llvm::Value*> args;
        

        for (auto arg : node.args) {
            arg->accept(*this);
            llvm::Value* argVal = lastValue;
            
            if (!argVal) {
                lastValue = nullptr;
                return;
            }
            

            if (argVal->getType()->isIntegerTy()) {
                argVal = builder.CreateSIToFP(argVal, doubleTy, "arg_to_double");
            } else if (argVal->getType()->isIntegerTy(1)) {
                llvm::Value* intVal = builder.CreateZExt(argVal, llvm::Type::getInt32Ty(ctx));
                argVal = builder.CreateSIToFP(intVal, doubleTy, "bool_to_double");
            }
            
            args.push_back(argVal);
        }
        

        if ((node.identifier == "sqrt" || node.identifier == "sin" || node.identifier == "cos" ||
             node.identifier == "exp" || node.identifier == "log") && args.size() != 1) {
            std::cerr << "Error: " << node.identifier << " expects 1 argument" << std::endl;
            lastValue = nullptr;
            return;
        }
        
        if (node.identifier == "pow" && args.size() != 2) {
            std::cerr << "Error: pow expects 2 arguments" << std::endl;
            lastValue = nullptr;
            return;
        }
        
        if (node.identifier == "rand") {

            if (!node.args.empty()) {
                std::cerr << "Error: rand expects 0 arguments" << std::endl;
                lastValue = nullptr;
                return;
            }
            

            static bool seeded = false;
            if (!seeded) {

                llvm::Function* srandFunc = module.getFunction("srand");
                if (!srandFunc) {
                    llvm::FunctionType* srandType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(ctx), 
                        {llvm::Type::getInt32Ty(ctx)}, 
                        false
                    );
                    srandFunc = llvm::Function::Create(srandType, llvm::Function::ExternalLinkage, "srand", module);
                }
                

                llvm::Function* timeFunc = module.getFunction("time");
                if (!timeFunc) {
                    llvm::FunctionType* timeType = llvm::FunctionType::get(
                        llvm::Type::getInt64Ty(ctx), 
                        {llvm::PointerType::get(llvm::Type::getInt64Ty(ctx), 0)}, 
                        false
                    );
                    timeFunc = llvm::Function::Create(timeType, llvm::Function::ExternalLinkage, "time", module);
                }
                

                llvm::Value* nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt64Ty(ctx), 0));
                llvm::Value* currentTime = builder.CreateCall(timeFunc, {nullPtr}, "current_time");
                

                llvm::Value* seed = builder.CreateTrunc(currentTime, llvm::Type::getInt32Ty(ctx), "seed");
                

                builder.CreateCall(srandFunc, {seed});
                seeded = true;
            }
            
            llvm::Function* randFunc = module.getFunction("rand");
            if (!randFunc) {
                llvm::FunctionType* randType = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), false);
                randFunc = llvm::Function::Create(randType, llvm::Function::ExternalLinkage, "rand", module);
            }
            

            llvm::Value* randResult = builder.CreateCall(randFunc, {}, "rand_result");
            

            llvm::Value* ten = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 10);
            llvm::Value* modResult = builder.CreateSRem(randResult, ten, "rand_mod_10");
            

            llvm::Value* one = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1);
            lastValue = builder.CreateAdd(modResult, one, "rand_1_to_10");
            return;
        }


        llvm::Function* mathFunc = nullptr;
        std::string intrinsicName;
        
        if (node.identifier == "sqrt") {
            intrinsicName = "llvm.sqrt.f64";
        } else if (node.identifier == "sin") {
            intrinsicName = "llvm.sin.f64";
        } else if (node.identifier == "cos") {
            intrinsicName = "llvm.cos.f64";
        } else if (node.identifier == "exp") {
            intrinsicName = "llvm.exp.f64";
        } else if (node.identifier == "log") {
            intrinsicName = "llvm.log.f64";
        } else if (node.identifier == "pow") {
            intrinsicName = "llvm.pow.f64";
        }

        mathFunc = module.getFunction(intrinsicName);
        if (!mathFunc) {
            std::vector<llvm::Type*> paramTypes;
            if (node.identifier == "pow") {
                paramTypes = {doubleTy, doubleTy};
            } else {
                paramTypes = {doubleTy};
            }
            
            llvm::FunctionType* funcType = llvm::FunctionType::get(doubleTy, paramTypes, false);
            mathFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, intrinsicName, module);
        }
        
        if (mathFunc) {
            llvm::Value* result = builder.CreateCall(mathFunc, args, node.identifier + "_result");

            lastValue = result;
        } else {
            lastValue = nullptr;
        }
        return;
    }
    

    llvm::Function* callee = module.getFunction(node.identifier);
    if (!callee) {
        std::cerr << "Error: Unknown function '" << node.identifier << "'" << std::endl;
        lastValue = nullptr;
        return;
    }
    

    if (callee->arg_size() != node.args.size()) {
        std::cerr << "Error: Function '" << node.identifier << "' expects " 
                  << callee->arg_size() << " arguments, got " << node.args.size() << std::endl;
        lastValue = nullptr;
        return;
    }
    

    std::vector<llvm::Value*> argsV;
    auto paramIt = callee->arg_begin();
    
    for (size_t i = 0; i < node.args.size(); ++i, ++paramIt) {
        node.args[i]->accept(*this);
        llvm::Value* argVal = lastValue;
        
        if (!argVal) {
            lastValue = nullptr;
            return;
        }
        

        llvm::Type* expectedType = paramIt->getType();
        if (argVal->getType() != expectedType) {
            if (argVal->getType()->isIntegerTy() && expectedType->isIntegerTy()) {

                if (argVal->getType()->getIntegerBitWidth() < expectedType->getIntegerBitWidth()) {
                    argVal = builder.CreateZExt(argVal, expectedType, "arg_int_extend");
                } else if (argVal->getType()->getIntegerBitWidth() > expectedType->getIntegerBitWidth()) {
                    argVal = builder.CreateTrunc(argVal, expectedType, "arg_int_trunc");
                }
            } else if (argVal->getType()->isIntegerTy() && expectedType->isDoubleTy()) {

                argVal = builder.CreateSIToFP(argVal, expectedType, "arg_int_to_double");
            } else if (argVal->getType()->isDoubleTy() && expectedType->isIntegerTy()) {

                argVal = builder.CreateFPToSI(argVal, expectedType, "arg_double_to_int");
            } else if (argVal->getType()->isPointerTy() && expectedType->isPointerTy()) {
                argVal = builder.CreateBitCast(argVal, expectedType, "arg_ptr_cast");
            }
        }
        
        argsV.push_back(argVal);
    }
    

    lastValue = builder.CreateCall(callee, argsV, callee->getReturnType()->isVoidTy() ? "" : "call_result");
    
    // Track the return type for user-defined types
    if (lastValue && callee->getReturnType()->isPointerTy()) {
        // Check if this function returns a user-defined type
        // Look for the function definition to get the return type
        for (const auto& typePair : types) {
            for (const auto& method : typePair.second->methods) {
                if (auto defFunc = dynamic_cast<DefFuncNode*>(method)) {
                    if (defFunc->identifier == node.identifier || 
                        typePair.first + "_" + defFunc->identifier == node.identifier) {
                        if (defFunc->returnType) {
                            std::string returnTypeName = defFunc->returnType->toString();
                            // Store this information for later use
                            variableTypes["_last_call_result"] = returnTypeName;
                            std::cerr << "DEBUG: Function '" << node.identifier << "' returns type '" << returnTypeName << "'" << std::endl;
                        }
                    }
                }
            }
        }
        
        // Also check global functions by looking at the actual function in the module
        llvm::Function* func = module.getFunction(node.identifier);
        if (func && func->getReturnType()->isPointerTy()) {
            // Try to find the function definition in the global functions map
            for (const auto& stmt : program.Statements) {
                if (auto funcDef = dynamic_cast<DefFuncNode*>(stmt)) {
                    if (funcDef->identifier == node.identifier && funcDef->returnType) {
                        std::string returnTypeName = funcDef->returnType->toString();
                        variableTypes["_last_call_result"] = returnTypeName;
                        std::cerr << "DEBUG: Function '" << node.identifier << "' returns type '" << returnTypeName << "'" << std::endl;
                        break;
                    }
                }
            }
        }
    }
}

void LLVMCodegenVisitor::visit(BaseCallNode& node) {

    

    llvm::Value* selfPtr = nullptr;
    std::string currentTypeName = "";
    

    for (auto it = localVarsStack.rbegin(); it != localVarsStack.rend(); ++it) {
        auto selfIt = it->find("self");
        if (selfIt != it->end()) {
            selfPtr = builder.CreateLoad(selfIt->second->getAllocatedType(), selfIt->second, "self");
            break;
        }
    }
    
    if (!selfPtr) {
        std::cerr << "Error: 'self' not found in current context for base() call" << std::endl;
        lastValue = nullptr;
        return;
    }
    


    llvm::Function* currentFunc = builder.GetInsertBlock()->getParent();
    std::string funcName = currentFunc->getName().str();
    

    size_t underscorePos = funcName.find('_');
    if (underscorePos != std::string::npos) {
        currentTypeName = funcName.substr(0, underscorePos);
        std::string methodName = funcName.substr(underscorePos + 1);
        

        auto typeIt = types.find(currentTypeName);
        if (typeIt != types.end() && !typeIt->second->parentTypeName.empty()) {
            std::string parentTypeName = typeIt->second->parentTypeName;
            std::string parentMethodName = parentTypeName + "_" + methodName;
            

            llvm::Function* parentMethod = module.getFunction(parentMethodName);
            if (parentMethod) {

                std::vector<llvm::Value*> args;
                args.push_back(selfPtr);
                
                for (auto arg : node.args) {
                    arg->accept(*this);
                    if (lastValue) {
                        args.push_back(lastValue);
                    }
                }
                

                lastValue = builder.CreateCall(parentMethod, args, "base_call_result");
                return;
            }
        }
    }
    

    std::cerr << "Warning: Could not resolve base() call for type " << currentTypeName << std::endl;
    lastValue = builder.CreateGlobalStringPtr("");
}

