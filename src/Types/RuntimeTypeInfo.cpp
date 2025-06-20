#include "Types/RuntimeTypeInfo.hpp"
#include "Statements/TypeDefNode.hpp"
#include "Globals.hpp"
#include <iostream>

extern std::map<std::string, TypeDefNode*> types;

void RuntimeTypeInfo::buildAncestorChain() {
    ancestorTypes.clear();
    std::string currentParent = parentTypeName;
    
    while (!currentParent.empty()) {
        ancestorTypes.push_back(currentParent);
        
        // Look up the parent type to get its parent
        auto typeIt = types.find(currentParent);
        if (typeIt != types.end()) {
            currentParent = typeIt->second->parentTypeName;
        } else {
            break;
        }
    }
    
    // Add Object as the ultimate parent if not already present
    if (!ancestorTypes.empty() && ancestorTypes.back() != "Object") {
        ancestorTypes.push_back("Object");
    } else if (ancestorTypes.empty() && typeName != "Object") {
        ancestorTypes.push_back("Object");
    }
}

bool RuntimeTypeInfo::isCompatibleWith(const std::string& targetType) const {
    // Exact match
    if (typeName == targetType) {
        return true;
    }
    
    // Build ancestor chain if not already built
    if (ancestorTypes.empty() && !parentTypeName.empty()) {
        const_cast<RuntimeTypeInfo*>(this)->buildAncestorChain();
    }
    
    // Check if target is in ancestor chain
    for (const std::string& ancestor : ancestorTypes) {
        if (ancestor == targetType) {
            return true;
        }
    }
    
    return false;
}

bool RuntimeTypeInfo::isExactType(const std::string& targetType) const {
    return typeName == targetType;
}

RuntimeTypeRegistry& RuntimeTypeRegistry::getInstance() {
    static RuntimeTypeRegistry instance;
    return instance;
}

void RuntimeTypeRegistry::registerType(const std::string& typeName, const std::string& parentTypeName) {
    if (typeInfoMap.find(typeName) == typeInfoMap.end()) {
        typeInfoMap[typeName] = new RuntimeTypeInfo(typeName, parentTypeName);
    }
}

RuntimeTypeInfo* RuntimeTypeRegistry::getTypeInfo(const std::string& typeName) {
    auto it = typeInfoMap.find(typeName);
    if (it != typeInfoMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool RuntimeTypeRegistry::isTypeCompatible(const std::string& sourceType, const std::string& targetType) {
    RuntimeTypeInfo* sourceInfo = getTypeInfo(sourceType);
    if (sourceInfo) {
        return sourceInfo->isCompatibleWith(targetType);
    }
    
    // Fallback for built-in types
    if (sourceType == targetType) {
        return true;
    }
    
    // All types are compatible with Object
    if (targetType == "Object") {
        return true;
    }
    
    return false;
}