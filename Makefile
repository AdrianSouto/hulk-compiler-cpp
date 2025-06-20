CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -fexceptions
LLVM_CONFIG = llvm-config
LLVM_CXXFLAGS = $(shell $(LLVM_CONFIG) --cxxflags | sed 's/-fno-exceptions/-fexceptions/g')
LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags --libs core)


SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
HULK_DIR = hulk


CPP_SOURCES = src/AST/ASTNode.cpp \
              src/AST/Attribute.cpp \
              src/AST/ExpressionNode.cpp \
              src/AST/Program.cpp \
              src/Context/Context.cpp \
              src/Expressions/AdditionNode.cpp \
              src/Expressions/AndNode.cpp \
              src/Expressions/AssignmentNode.cpp \
              src/Expressions/MemberAssignmentNode.cpp \
              src/Expressions/SelfMemberAssignmentNode.cpp \
              src/Expressions/BinaryOperatorNode.cpp \
              src/Expressions/BlockExprNode.cpp \
              src/Expressions/BooleanNode.cpp \
              src/Expressions/ConcatenationNode.cpp \
              src/Expressions/ConditionalNode.cpp \
              src/Expressions/DivisionNode.cpp \
              src/Expressions/EqualNode.cpp \
              src/Expressions/ForRangeNode.cpp \
              src/Expressions/FuncCallNode.cpp \
              src/Expressions/BaseCallNode.cpp \
              src/Expressions/GreaterEqNode.cpp \
              src/Expressions/GreaterNode.cpp \
              src/Expressions/IsNode.cpp \
              src/Expressions/AsNode.cpp \
              src/Expressions/LessEqNode.cpp \
              src/Expressions/LessNode.cpp \
              src/Expressions/LetExprNode.cpp \
              src/Expressions/MemberAccessNode.cpp \
              src/Expressions/MethodCallNode.cpp \
              src/Expressions/ModuloNode.cpp \
              src/Expressions/MultiplicationNode.cpp \
              src/Expressions/NotEqualNode.cpp \
              src/Expressions/NotNode.cpp \
              src/Expressions/UnaryMinusNode.cpp \
              src/Expressions/NumberNode.cpp \
              src/Expressions/OrNode.cpp \
              src/Expressions/PowerNode.cpp \
              src/Expressions/SelfMemberAccessNode.cpp \
              src/Expressions/StringLiteralNode.cpp \
              src/Expressions/SubtractionNode.cpp \
              src/Expressions/TypeInstantiationNode.cpp \
              src/Expressions/UnaryOperatorNode.cpp \
              src/Expressions/VariableNode.cpp \
              src/Expressions/WhileNode.cpp \
              src/Globals.cpp \
              src/Statements/BlockNode.cpp \
              src/Statements/DefFuncNode.cpp \
              src/Statements/ExpressionStatementNode.cpp \
              src/Statements/LetVarNode.cpp \
              src/Statements/PrintStatementNode.cpp \
              src/Statements/TypeDefNode.cpp \
              src/Expressions/PrintExpressionNode.cpp \
              src/Visitors/LLVMCodegenVisitor_Arithmetic.cpp \
              src/Visitors/LLVMCodegenVisitor_Comparison.cpp \
              src/Visitors/LLVMCodegenVisitor_Control.cpp \
              src/Visitors/LLVMCodegenVisitor_Core.cpp \
              src/Visitors/LLVMCodegenVisitor_Expressions.cpp \
              src/Visitors/LLVMCodegenVisitor_Functions.cpp \
              src/Visitors/LLVMCodegenVisitor_Literals.cpp \
              src/Visitors/LLVMCodegenVisitor_Logical.cpp \
              src/Visitors/LLVMCodegenVisitor_IsNode.cpp \
              src/Visitors/LLVMCodegenVisitor_AsNode.cpp \
              src/Visitors/LLVMCodegenVisitor_Statements.cpp \
              src/Visitors/LLVMCodegenVisitor_String.cpp \
              src/Visitors/LLVMCodegenVisitor_Types.cpp \
              src/Visitors/LLVMCodegenVisitor_PrintExpression.cpp

MAIN_SOURCE = main.cpp


PARSER_SOURCE = hulk/parser.cpp
PARSER_HEADER = hulk/parser.hpp
LEXER_SOURCE = hulk/lexer.cpp


OBJECTS = $(CPP_SOURCES:src/%.cpp=build/%.o)
MAIN_OBJECT = build/main.o
PARSER_OBJECT = build/parser.o
LEXER_OBJECT = build/lexer.o


TARGET = hulk/hulk_compiler.exe


INPUT_FILE = script.hulk

.PHONY: all compile execute clean force-regenerate


all: compile


compile: $(TARGET)
	@echo "Compilation completed with native lexer. Artifacts stored in hulk/"


$(TARGET): $(HULK_DIR) $(LEXER_SOURCE) $(PARSER_SOURCE) $(OBJECTS) $(MAIN_OBJECT) $(PARSER_OBJECT) $(LEXER_OBJECT)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -o $@ $(OBJECTS) $(MAIN_OBJECT) $(PARSER_OBJECT) $(LEXER_OBJECT) $(LLVM_LDFLAGS)
	@echo "Copying additional artifacts to hulk/"
	@cp $(INPUT_FILE) hulk/ 2>/dev/null || echo "No input file to copy"
	@echo "Hulk compiler with native lexer built successfully in hulk/"


$(HULK_DIR):
	@mkdir -p hulk


$(LEXER_SOURCE): lexer_definition.txt lexer/lexer_generator | $(HULK_DIR)
	@echo "Generating lexer from definition..."
	./lexer/lexer_generator lexer_definition.txt $(LEXER_SOURCE)


lexer/lexer_generator: lexer/lexer_generator.cpp
	@echo "Building lexer generator..."
	$(CXX) -std=c++17 -o lexer/lexer_generator lexer/lexer_generator.cpp


$(PARSER_SOURCE) $(PARSER_HEADER): parser.y | $(HULK_DIR)
	bison -d -o $(PARSER_SOURCE) parser.y


$(BUILD_DIR):
	@mkdir -p build
	@mkdir -p build/AST
	@mkdir -p build/Context
	@mkdir -p build/Expressions
	@mkdir -p build/Statements
	@mkdir -p build/Types
	@mkdir -p build/Visitors
	@mkdir -p build/Lexer


$(MAIN_OBJECT): $(MAIN_SOURCE) $(PARSER_HEADER) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -c $< -o $@


$(PARSER_OBJECT): $(PARSER_SOURCE) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -c $< -o $@

$(LEXER_OBJECT): $(LEXER_SOURCE) $(PARSER_HEADER) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -c $< -o $@

build/%.o: src/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -c $< -o $@



execute: force-regenerate $(TARGET)
	@echo "--- Running Hulk Compiler Workflow with Native Lexer ---"
	@cd hulk && \
	    echo "Step 1: Generating LLVM IR (./hulk_compiler.exe $(INPUT_FILE) -> output.ll)..." && \
	    ./hulk_compiler.exe $(INPUT_FILE) && \
	    echo "Step 2: Compiling LLVM IR to assembly (llc output.ll -> output.s)..." && \
	    llc output.ll -o output.s && \
	    echo "Step 3: Assembling and linking to executable ($(CXX) output.s -o output_exec -lm)..." && \
	    $(CXX) output.s -o output_exec -lm && \
	    echo "Step 4: Executing the generated program (./output_exec)..." && \
	    ./output_exec
	@echo "--- Hulk Compiler Workflow with Native Lexer Completed ---"


force-regenerate:
	@echo "Forcing regeneration of parser and lexer files..."
	@rm -f $(PARSER_SOURCE) $(PARSER_HEADER)
	@rm -f $(LEXER_SOURCE)
	@rm -f $(PARSER_OBJECT) $(LEXER_OBJECT)


clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build
	@rm -rf hulk
	@rm -f lexer/lexer_generator lexer/lexer_generator.exe
	@rm -f output.ll output.s output_exec output_exec.exe
	@echo "Clean completed."


help:
	@echo "Available targets:"
	@echo "  compile  - Compile the Hulk compiler with native lexer and generate hulk/ directory"
	@echo "  execute  - Execute the compiled Hulk program (depends on compile)"
	@echo "  clean    - Remove all build artifacts"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "This Makefile uses a native C++ lexer instead of Flex."

