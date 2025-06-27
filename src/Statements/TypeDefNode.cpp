#include "Statements/TypeDefNode.hpp"
#include "Context/IContext.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Expressions/NumberNode.hpp"
#include "Expressions/StringLiteralNode.hpp"
#include "Expressions/BooleanNode.hpp"
#include "Expressions/VariableNode.hpp"
#include "Globals.hpp"
#include <iostream>
#include <unordered_set>

TypeDefNode::TypeDefNode(const std::string& name, 
                         const std::vector<Parameter>& args,
                         const std::vector<Attribute>& attrs,
                         const std::vector<StatementNode*>& meths)
    : typeName(name), typeArguments(args), attributes(attrs), methods(meths), parentTypeName("") {
    
    for (const auto& attr : attributes) {
        if (attr.initExpression) {
            attributeExpressions.push_back(attr.initExpression);
        }
    }
}

TypeDefNode::TypeDefNode(const std::string& name, 
                         const std::vector<Parameter>& args,
                         const std::vector<ASTNode*>& members)
    : typeName(name), typeArguments(args), parentTypeName("") {
    
    for (ASTNode* member : members) {
        if (Attribute* attr = dynamic_cast<Attribute*>(member)) {
            attributes.push_back(*attr);
            if (attr->initExpression) {
                attributeExpressions.push_back(attr->initExpression);
            }
        } else if (StatementNode* stmt = dynamic_cast<StatementNode*>(member)) {
            methods.push_back(stmt);
        }
        
    }
}

TypeDefNode::TypeDefNode(const std::string& name, 
                         const std::vector<Parameter>& args,
                         const std::string& parentName,
                         const std::vector<ExpressionNode*>& parentArguments,
                         const std::vector<ASTNode*>& members)
    : typeName(name), typeArguments(args), parentTypeName(parentName), parentArgs(parentArguments) {
    
    for (ASTNode* member : members) {
        if (Attribute* attr = dynamic_cast<Attribute*>(member)) {
            attributes.push_back(*attr);
            if (attr->initExpression) {
                attributeExpressions.push_back(attr->initExpression);
            }
        } else if (StatementNode* stmt = dynamic_cast<StatementNode*>(member)) {
            methods.push_back(stmt);
        }
        
    }
}



bool TypeDefNode::validate(IContext* context) {
    
    types[typeName] = const_cast<TypeDefNode*>(this);
    
    
    Type* newType = createUserDefinedType(typeName, const_cast<TypeDefNode*>(this), parentTypeName);
    
    
    IContext* typeContext = context->CreateChildContext();
    
    
    for (const auto& arg : typeArguments) {
        typeContext->Define(arg.name);
    }
    
    for (const auto& attr : attributes) {
        typeContext->Define(attr.name);
    }
    
    
    if (!validateTypeArguments(typeContext)) {
        delete typeContext;
        return false;
    }
    
    
    if (!validateAttributes(typeContext)) {
        delete typeContext;
        return false;
    }
    
    
    for (auto method : methods) {
        
        IContext* methodContext = typeContext->CreateChildContext();
        
        
        methodContext->Define("self");
        
        if (!method->validate(methodContext)) {
            errorMessage = "Error in method of type '" + typeName + "': " + method->getErrorMessage();
            delete methodContext;
            delete typeContext;
            return false;
        }
        
        delete methodContext;
    }
    
    
    
    
    
    delete typeContext;
    return true;
}

bool TypeDefNode::validateTypeArguments(IContext* context) {
    
    std::unordered_set<std::string> argSet;
    for (const auto& arg : typeArguments) {
        if (argSet.find(arg.name) != argSet.end()) {
            errorMessage = "Error in type '" + typeName + "': Duplicate type argument name '" + arg.name + "'";
            return false;
        }
        argSet.insert(arg.name);
        
        
        if (arg.hasType()) {
            if (arg.type->getKind() == TypeKind::UNKNOWN) {
                errorMessage = "Error in type '" + typeName + "': Invalid type for argument '" + arg.name + "'";
                return false;
            }
        }
    }
    return true;
}

bool TypeDefNode::validateAttributes(IContext* context) {
    
    std::unordered_set<std::string> attrSet;
    for (const auto& attr : attributes) {
        if (attrSet.find(attr.name) != attrSet.end()) {
            errorMessage = "Error in type '" + typeName + "': Duplicate attribute name '" + attr.name + "'";
            return false;
        }
        attrSet.insert(attr.name);
        
        
        if (!validateAttributeInitialization(attr, context)) {
            return false;
        }
    }
    return true;
}

bool TypeDefNode::validateAttributeInitialization(const Attribute& attr, IContext* context) {
    if (!attr.hasInitExpression()) {
        
        return true;
    }
    
    
    if (!attr.initExpression->validate(context)) {
        errorMessage = "Error in attribute '" + attr.name + "' of type '" + typeName + 
                       "': " + attr.initExpression->getErrorMessage();
        return false;
    }
    
    
    if (attr.hasType()) {
        Type* inferredType = inferExpressionType(attr.initExpression);
        
        
        if (auto varNode = dynamic_cast<VariableNode*>(attr.initExpression)) {
            
            Type* argType = getTypeArgumentType(varNode->identifier);
            if (argType != nullptr) {
                
                if (argType->getKind() != TypeKind::UNKNOWN && 
                    !argType->isCompatibleWith(attr.type)) {
                    errorMessage = "Error in attribute '" + attr.name + "' of type '" + typeName + 
                                   "': Type mismatch. Expected " + attr.type->toString() + 
                                   ", but type argument '" + varNode->identifier + "' has type " + argType->toString();
                    return false;
                }
                
                return true;
            }
        }
        
        
        if (inferredType && inferredType->getKind() != TypeKind::UNKNOWN) {
            if (!inferredType->isCompatibleWith(attr.type)) {
                errorMessage = "Error in attribute '" + attr.name + "' of type '" + typeName + 
                               "': Type mismatch. Expected " + attr.type->toString() + 
                               ", but got " + inferredType->toString();
                return false;
            }
        }
        
        
        if (!validateTypeArgumentUsage(attr.initExpression, context)) {
            return false;
        }
    }
    
    return true;
}

bool TypeDefNode::validateTypeArgumentUsage(ExpressionNode* expr, IContext* context) {
    
    if (auto varNode = dynamic_cast<VariableNode*>(expr)) {
        
        Type* argType = getTypeArgumentType(varNode->identifier);
        if (argType != nullptr) {
            
            
            return true;
        }
    }
    
    
    
    return true;
}

Type* TypeDefNode::inferExpressionType(ExpressionNode* expr) {
    
    if (dynamic_cast<NumberNode*>(expr)) {
        return Type::getNumberType();
    }
    if (dynamic_cast<StringLiteralNode*>(expr)) {
        return Type::getStringType();
    }
    if (dynamic_cast<BooleanNode*>(expr)) {
        return Type::getBooleanType();
    }
    if (auto varNode = dynamic_cast<VariableNode*>(expr)) {
        
        Type* argType = getTypeArgumentType(varNode->identifier);
        if (argType != nullptr) {
            return argType;
        }
        
        
        
        return Type::getUnknownType();
    }
    
    
    return Type::getUnknownType();
}

Type* TypeDefNode::getTypeArgumentType(const std::string& argName) {
    
    for (const auto& arg : typeArguments) {
        if (arg.name == argName) {
            return arg.hasType() ? arg.type : Type::getUnknownType();
        }
    }
    return nullptr; 
}

void TypeDefNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

TypeDefNode::~TypeDefNode() {
    
    for (auto expr : attributeExpressions) {
        delete expr;
    }
    
    
    for (auto method : methods) {
        delete method;
    }
}