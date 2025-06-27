#include "Expressions/ConditionalNode.hpp"
#include "Visitors/LLVMCodegenVisitor.hpp"
#include "Context/IContext.hpp"
#include "Types/Type.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Globals.hpp"

#include <iostream>
#include <AST/ASTNode.hpp> 

ConditionalNode::ConditionalNode() : elseBody(nullptr) {}

void ConditionalNode::addBranch(ExpressionNode* condition, ASTNode* body) { 
    branches.emplace_back(condition, body);
}

void ConditionalNode::setElse(ASTNode* elseBody_) { 
    elseBody = elseBody_;
}




bool ConditionalNode::validate(IContext* context) {
    for (auto& branch : branches) {
        if (!branch.condition->validate(context)) {
            errorMessage = "Invalid condition in if/elif: " + branch.condition->getErrorMessage();
            return false;
        }
        
        if (!branch.body->validate(context)) {
            errorMessage = "Invalid body in if/elif: " + branch.body->getErrorMessage();
            return false;
        }
    }
    if (elseBody && !elseBody->validate(context)) { 
        errorMessage = "Invalid else body: " + elseBody->getErrorMessage();
        return false;
    }
    return true;
}

Type* ConditionalNode::inferType(IContext* context) const {
    if (branches.empty()) {
        return Type::getNumberType(); // Default fallback
    }
    
    // Get the type of the first branch body
    Type* resultType = nullptr;
    
    // Check if the first branch body is an ExpressionNode
    if (auto exprBody = dynamic_cast<ExpressionNode*>(branches[0].body)) {
        resultType = exprBody->inferType(context);
    }
    
    if (!resultType) {
        return Type::getNumberType(); // Default fallback
    }
    
    // Check all other branches and find common base type
    for (size_t i = 1; i < branches.size(); ++i) {
        if (auto exprBody = dynamic_cast<ExpressionNode*>(branches[i].body)) {
            Type* branchType = exprBody->inferType(context);
            if (branchType) {
                resultType = findCommonBaseType(resultType, branchType);
            }
        }
    }
    
    // Check else branch if it exists
    if (elseBody) {
        if (auto exprElse = dynamic_cast<ExpressionNode*>(elseBody)) {
            Type* elseType = exprElse->inferType(context);
            if (elseType) {
                resultType = findCommonBaseType(resultType, elseType);
            }
        }
    }
    
    return resultType ? resultType : Type::getNumberType();
}

// Helper function to find common base type
Type* ConditionalNode::findCommonBaseType(Type* type1, Type* type2) const {
    if (!type1 || !type2) {
        return type1 ? type1 : type2;
    }
    
    // If types are the same, return that type
    if (type1->getTypeName() == type2->getTypeName()) {
        return type1;
    }
    
    // Check if one type is a subtype of the other
    if (type1->conformsTo(type2)) {
        return type2; // type2 is the base type
    }
    if (type2->conformsTo(type1)) {
        return type1; // type1 is the base type
    }
    
    // Find common ancestor in type hierarchy
    std::string type1Name = type1->getTypeName();
    std::string type2Name = type2->getTypeName();
    
    // Check if both types have a common parent in the type hierarchy
    auto type1It = types.find(type1Name);
    if (type1It != types.end()) {
        TypeDefNode* type1Def = type1It->second;
        std::string currentParent = type1Def->parentTypeName;
        
        while (!currentParent.empty()) {
            // Check if type2 conforms to this parent
            Type* parentType = getTypeByName(currentParent);
            if (parentType && type2->conformsTo(parentType)) {
                return parentType;
            }
            
            // Move up the hierarchy
            auto parentIt = types.find(currentParent);
            if (parentIt != types.end()) {
                currentParent = parentIt->second->parentTypeName;
            } else {
                break;
            }
        }
    }
    
    // If no common base type found, return Object type as ultimate base
    return getTypeByName("Object");
}

void ConditionalNode::accept(LLVMCodegenVisitor& visitor) {
    visitor.visit(*this);
}

ConditionalNode::~ConditionalNode() {
    for (auto& branch : branches) {
        delete branch.condition;
        delete branch.body; 
    }
    delete elseBody; 
}
