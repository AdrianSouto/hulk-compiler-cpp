#ifndef LLVM_CODEGEN_VISITOR_HPP
#define LLVM_CODEGEN_VISITOR_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <map>
#include <vector>
#include <string>


class NumberNode;
class StringLiteralNode;
class BooleanNode;
class VariableNode;
class AdditionNode;
class SubtractionNode;
class MultiplicationNode;
class DivisionNode;
class PowerNode;
class ConcatenationNode;
class FuncCallNode;
class BaseCallNode;
class PrintStatementNode;
class LetVarNode;
class DefFuncNode;
class Program;
class BlockNode;
class LetExprNode;
class ConditionalNode;
class AndNode;
class OrNode;
class NotNode;
class EqualNode;
class NotEqualNode;
class GreaterNode;
class GreaterEqNode;
class LessNode;
class LessEqNode;
class ModuloNode;
class AssignmentNode;
class ExpressionStatementNode;
class WhileNode;
class BlockExprNode;
class ForRangeNode;
class TypeDefNode;
class SelfMemberAccessNode;
class TypeInstantiationNode;
class MemberAccessNode;
class MethodCallNode;
class IsNode;
class AsNode;

class LLVMCodegenVisitor {
public:

    llvm::LLVMContext &ctx;
    llvm::IRBuilder<> &builder;
    llvm::Module &module;
    llvm::Value* lastValue;
    

    std::vector<std::map<std::string, llvm::AllocaInst*>> localVarsStack;
    

    std::map<std::string, std::string> variableTypes;
    

    std::string currentTypeName;


    LLVMCodegenVisitor() : ctx(*new llvm::LLVMContext()),
                           builder(*new llvm::IRBuilder<>(ctx)),
                           module(*new llvm::Module("module", ctx)),
                           lastValue(nullptr) {}


    LLVMCodegenVisitor(llvm::LLVMContext& context, llvm::IRBuilder<>& b, llvm::Module& m);

    void visit(NumberNode& node);
    void visit(StringLiteralNode& node);
    void visit(BooleanNode& node);
    void visit(VariableNode& node);
    void visit(AdditionNode& node);
    void visit(SubtractionNode& node);
    void visit(MultiplicationNode& node);
    void visit(DivisionNode& node);
    void visit(PowerNode& node);
    void visit(ConcatenationNode& node);
    void visit(FuncCallNode& node);
    void visit(BaseCallNode& node);
    void visit(PrintStatementNode& node);
    void visit(LetVarNode& node);
    void visit(DefFuncNode& node);
    void visit(Program& node);
    void visit(LetExprNode& node);
    void visit(BlockNode& node);
    void visit(ConditionalNode& node);
    void visit(AndNode& node);
    void visit(OrNode& node);
    void visit(NotNode& node);
    void visit(EqualNode& node);
    void visit(NotEqualNode& node);
    void visit(GreaterNode& node);
    void visit(GreaterEqNode& node);
    void visit(LessNode& node);
    void visit(LessEqNode& node);
    void visit(ModuloNode& node);
    void visit(AssignmentNode& node);
    void visit(ExpressionStatementNode& node);
    void visit(WhileNode& node);
    void visit(BlockExprNode& node);
    void visit(ForRangeNode& node);
    void visit(TypeDefNode& node);
    void visit(SelfMemberAccessNode& node);
    void visit(TypeInstantiationNode& node);
    void visit(MemberAccessNode& node);
    void visit(MethodCallNode& node);
    void visit(IsNode& node);
    void visit(AsNode& node);
};

#endif

