#include "Expressions/FuncCallNode.hpp"
#include "Context/IContext.hpp"
#include "Context/Context.hpp"
#include "Types/Type.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

FuncCallNode::FuncCallNode(const std::string& id, const std::vector<ExpressionNode*>& arguments)
    : identifier(id), args(arguments) {}




bool FuncCallNode::validate(IContext* context) {

    for (size_t i = 0; i < args.size(); i++) {
        if (!args[i]->validate(context)) {
            errorMessage = "Error in argument " + std::to_string(i+1) + " of function '" +
                           identifier + "': " + args[i]->getErrorMessage();
            return false;
        }
    }


    if (!context->IsDefined(identifier, args.size())) {
        errorMessage = "Error: Function '" + identifier + "' with " +
                       std::to_string(args.size()) + " argument(s) is not defined";
        return false;
    }


    Context* ctx = dynamic_cast<Context*>(context);
    if (ctx) {
        FunctionSignature* signature = ctx->GetFunctionSignature(identifier, args.size());
        if (signature) {

            for (size_t i = 0; i < args.size(); i++) {
                Type* argType = args[i]->inferType(context);
                Type* expectedType = signature->parameterTypes[i];
                
                if (!argType->conformsTo(expectedType)) {
                    errorMessage = "Error: Function '" + identifier + "' expects argument " + 
                                   std::to_string(i+1) + " to be of type " + expectedType->toString() + 
                                   " but got " + argType->toString();
                    return false;
                }
            }
        }
    }

    return true;
}

Type* FuncCallNode::inferType(IContext* context) const {

    Context* ctx = dynamic_cast<Context*>(context);
    if (ctx) {
        FunctionSignature* signature = ctx->GetFunctionSignature(identifier, args.size());
        if (signature) {
            return signature->returnType;
        }
    }
    

    return Type::getNumberType();
}

void FuncCallNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

FuncCallNode::~FuncCallNode() {
    for (auto arg : args) {
        delete arg;
    }
}
