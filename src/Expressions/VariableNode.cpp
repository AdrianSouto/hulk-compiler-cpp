#include "Expressions/VariableNode.hpp"
#include "Context/IContext.hpp"
#include "Context/Context.hpp"
#include "Types/Type.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Globals.hpp"
#include <iostream>

VariableNode::VariableNode(const std::string& id) : identifier(id) {}



bool VariableNode::validate(IContext* context) {
    if (!context->IsDefined(identifier)) {
        errorMessage = "Error: Variable '" + identifier + "' is not defined in the current scope";
        return false;
    }
    return true;
}

Type* VariableNode::inferType(IContext* context) const {
    // Intentar obtener el tipo del contexto
    Context* ctx = dynamic_cast<Context*>(context);
    if (ctx) {
        Type* varType = ctx->GetVariableType(identifier);
        if (varType) {
            return varType;
        }
    }
    
    // Si no se encuentra el tipo, asumir Number por defecto
    return Type::getNumberType();
}

void VariableNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

