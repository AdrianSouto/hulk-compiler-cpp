#include "Types/Type.hpp"


Type* Type::getObjectType() { 
    static Type objectType(TypeKind::OBJECT, "Object");
    return &objectType;
}

Type* Type::getNumberType() { 
    static Type numberType(TypeKind::NUMBER, "Number", nullptr, getObjectType());
    return &numberType;
}

Type* Type::getStringType() { 
    static Type stringType(TypeKind::STRING, "String", nullptr, getObjectType());
    return &stringType;
}

Type* Type::getBooleanType() { 
    static Type booleanType(TypeKind::BOOLEAN, "Boolean", nullptr, getObjectType());
    return &booleanType;
}

Type* Type::getVoidType() { 
    static Type voidType(TypeKind::VOID, "Void", nullptr, getObjectType());
    return &voidType;
}

Type* Type::getUnknownType() { 
    static Type unknownType(TypeKind::UNKNOWN, "Unknown");
    return &unknownType;
}