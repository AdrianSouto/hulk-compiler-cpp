#ifndef TYPE_HPP
#define TYPE_HPP

#include <string>
#include <vector>
#include <memory>


class TypeDefNode;


enum class TypeKind {
    OBJECT,     
    NUMBER,
    STRING,
    BOOLEAN,
    VOID,
    FUNCTION,
    USER_DEFINED, 
    UNKNOWN
};

class Type {
private:
    TypeKind kind;
    std::string typeName;        
    TypeDefNode* typeDefinition; 
    Type* parentType;            
    
public:
    Type(TypeKind k, const std::string& name = "", TypeDefNode* def = nullptr, Type* parent = nullptr) 
        : kind(k), typeName(name), typeDefinition(def), parentType(parent) {}
    
    TypeKind getKind() const { return kind; }
    std::string getTypeName() const { return typeName; }
    TypeDefNode* getTypeDefinition() const { return typeDefinition; }
    Type* getParentType() const { return parentType; }
    
    void setParentType(Type* parent) { parentType = parent; }
    
    
    static Type* getObjectType() { 
        static Type objectType(TypeKind::OBJECT, "Object");
        return &objectType;
    }
    
    static Type* getNumberType() { 
        static Type numberType(TypeKind::NUMBER, "Number", nullptr, getObjectType());
        return &numberType;
    }
    
    static Type* getStringType() { 
        static Type stringType(TypeKind::STRING, "String", nullptr, getObjectType());
        return &stringType;
    }
    
    static Type* getBooleanType() { 
        static Type booleanType(TypeKind::BOOLEAN, "Boolean", nullptr, getObjectType());
        return &booleanType;
    }
    
    static Type* getVoidType() { 
        static Type voidType(TypeKind::VOID, "Void", nullptr, getObjectType());
        return &voidType;
    }
    
    static Type* getUnknownType() { 
        static Type unknownType(TypeKind::UNKNOWN, "Unknown");
        return &unknownType;
    }
    
    
    static Type* createUserDefinedType(const std::string& name, TypeDefNode* def, Type* parent = nullptr) {
        if (parent == nullptr) {
            parent = getObjectType(); 
        }
        return new Type(TypeKind::USER_DEFINED, name, def, parent);
    }
    
    std::string toString() const {
        switch (kind) {
            case TypeKind::OBJECT: return "Object";
            case TypeKind::NUMBER: return "Number";
            case TypeKind::STRING: return "String";
            case TypeKind::BOOLEAN: return "Boolean";
            case TypeKind::VOID: return "Void";
            case TypeKind::FUNCTION: return "Function";
            case TypeKind::USER_DEFINED: return typeName;
            case TypeKind::UNKNOWN: return "Unknown";
            default: return "Invalid";
        }
    }
    
    
    bool conformsTo(const Type* other) const {
        if (other == nullptr) return false;
        
        
        if (this == other || (kind == other->kind && typeName == other->typeName)) {
            return true;
        }
        
        
        if (other->kind == TypeKind::OBJECT) {
            return true;
        }
        
        
        if (kind == TypeKind::UNKNOWN || other->kind == TypeKind::UNKNOWN) {
            return true; 
        }
        
        
        if (kind == TypeKind::NUMBER || kind == TypeKind::STRING || kind == TypeKind::BOOLEAN) {
            return false; 
        }
        
        
        if (kind == TypeKind::USER_DEFINED) {
            Type* current = parentType;
            while (current != nullptr) {
                if (current->conformsTo(other)) {
                    return true;
                }
                current = current->parentType;
            }
        }
        
        return false;
    }
    
    
    bool isCompatibleWith(const Type* other) const {
        return conformsTo(other);
    }
    
    
    static Type* findLowestCommonAncestor(Type* type1, Type* type2) {
        if (type1 == nullptr || type2 == nullptr) {
            return getObjectType();
        }
        
        
        if (type1->conformsTo(type2)) {
            return type2;
        }
        if (type2->conformsTo(type1)) {
            return type1;
        }
        
        
        std::vector<Type*> ancestors1 = getAncestors(type1);
        std::vector<Type*> ancestors2 = getAncestors(type2);
        
        
        for (Type* ancestor1 : ancestors1) {
            for (Type* ancestor2 : ancestors2) {
                if (ancestor1 == ancestor2 || 
                    (ancestor1->kind == ancestor2->kind && ancestor1->typeName == ancestor2->typeName)) {
                    return ancestor1;
                }
            }
        }
        
        
        return getObjectType();
    }
    
private:
    
    static std::vector<Type*> getAncestors(Type* type) {
        std::vector<Type*> ancestors;
        Type* current = type;
        
        while (current != nullptr) {
            ancestors.push_back(current);
            current = current->parentType;
        }
        
        
        if (ancestors.empty() || ancestors.back()->kind != TypeKind::OBJECT) {
            ancestors.push_back(getObjectType());
        }
        
        return ancestors;
    }
};

#endif 
