#include "Parser/Grammar.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>
#include <algorithm>
#include <stdexcept>

namespace Parser {

// ============================================================================
// GRAMMAR LOADING METHODS
// ============================================================================

Grammar Grammar::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open grammar file: " + filename);
    }
    
    Grammar grammar;
    std::vector<std::string> productionLines;
    
    grammar.parseGrammarFile(file, productionLines);
    grammar.processProductions(productionLines);
    grammar.initializeGrammar();
    
    return grammar;
}

void Grammar::parseGrammarFile(std::ifstream& file, std::vector<std::string>& productionLines) {
    std::string line;
    bool readingProductions = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        if (line.find("Terminals:") == 0) {
            parseTerminals(line);
        }
        else if (line.find("NonTerminals:") == 0) {
            parseNonTerminals(line);
        }
        else if (line.find("Productions:") == 0) {
            readingProductions = true;
        }
        else if (readingProductions) {
            productionLines.push_back(line);
        }
    }
}

void Grammar::parseTerminals(const std::string& line) {
    size_t colonPos = line.find(":");
    std::string termsStr = trim(line.substr(colonPos + 1));
    std::istringstream iss(termsStr);
    std::string name;
    
    while (std::getline(iss, name, ',')) {
        name = trim(name);
        if (!name.empty()) {
            terminals.insert(Symbol(name, SymbolType::Terminal));
        }
    }
}

void Grammar::parseNonTerminals(const std::string& line) {
    size_t colonPos = line.find(":");
    std::string ntsStr = trim(line.substr(colonPos + 1));
    std::istringstream iss(ntsStr);
    std::string name;
    
    while (std::getline(iss, name, ',')) {
        name = trim(name);
        if (!name.empty()) {
            nonTerminals.insert(Symbol(name, SymbolType::NonTerminal));
        }
    }
}

void Grammar::processProductions(const std::vector<std::string>& productionLines) {
    for (const auto& prodLine : productionLines) {
        processProductionLine(prodLine);
    }
}

void Grammar::processProductionLine(const std::string& prodLine) {
    size_t arrowPos = prodLine.find("->");
    if (arrowPos == std::string::npos) return;
    
    std::string lhsStr = trim(prodLine.substr(0, arrowPos));
    Symbol lhsSymbol(lhsStr, SymbolType::NonTerminal);
    std::string rhsStr = trim(prodLine.substr(arrowPos + 2));
    
    std::vector<std::string> alternatives = splitAlternatives(rhsStr);
    
    for (const auto& altStr : alternatives) {
        std::vector<Symbol> rhsSymbols = parseRightHandSide(altStr);
        productions.push_back(Production(lhsSymbol, rhsSymbols));
    }
}

std::vector<std::string> Grammar::splitAlternatives(const std::string& rhsStr) {
    std::vector<std::string> alternatives;
    std::istringstream altStream(rhsStr);
    std::string alt;
    
    while (std::getline(altStream, alt, '|')) {
        alternatives.push_back(trim(alt));
    }
    
    return alternatives;
}

std::vector<Symbol> Grammar::parseRightHandSide(const std::string& altStr) {
    std::vector<Symbol> rhsSymbols;
    
    if (altStr == "ε" || altStr.empty()) {
        // Epsilon production - return empty vector
        return rhsSymbols;
    }
    
    std::istringstream tokenStream(altStr);
    std::string token;
    
    while (tokenStream >> token) {
        if (token == "ε") continue;
        
        SymbolType type = determineSymbolType(token);
        rhsSymbols.push_back(Symbol(token, type));
    }
    
    return rhsSymbols;
}

SymbolType Grammar::determineSymbolType(const std::string& token) {
    if (nonTerminals.find(Symbol(token, SymbolType::NonTerminal)) != nonTerminals.end()) {
        return SymbolType::NonTerminal;
    }
    return SymbolType::Terminal;
}

void Grammar::initializeGrammar() {
    setStartSymbol();
    calculateFirst();
    calculateFollow();
    buildParsingTable();
}

void Grammar::setStartSymbol() {
    if (!productions.empty()) {
        startSymbol = productions[0].left;
    }
}

// ============================================================================
// FIRST AND FOLLOW SET CALCULATION
// ============================================================================

void Grammar::calculateFirst() {
    initializeFirstSets();
    computeFirstSetsIteratively();
}

void Grammar::initializeFirstSets() {
    Symbol epsilon("ε", SymbolType::Epsilon);
    firstSets[epsilon].insert(epsilon);
    
    // For each terminal, FIRST(terminal) = {terminal}
    for (const auto& terminal : terminals) {
        firstSets[terminal].insert(terminal);
    }
    
    // For each non-terminal, initialize with empty set
    for (const auto& nonTerminal : nonTerminals) {
        firstSets[nonTerminal] = std::set<Symbol>();
    }
}

void Grammar::computeFirstSetsIteratively() {
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& prod : productions) {
            const Symbol& A = prod.left;
            size_t beforeSize = firstSets[A].size();
            
            // Calculate FIRST for the RHS
            std::set<Symbol> rhsFirst = computeFirst(prod.right);
            
            // Add to FIRST(A)
            firstSets[A].insert(rhsFirst.begin(), rhsFirst.end());
            
            if (firstSets[A].size() > beforeSize) {
                changed = true;
            }
        }
    }
}

std::set<Symbol> Grammar::computeFirst(const std::vector<Symbol>& symbols) const {
    std::set<Symbol> result;
    Symbol epsilon("ε", SymbolType::Epsilon);
    
    if (symbols.empty()) {
        result.insert(epsilon);
        return result;
    }
    
    bool allCanDeriveEpsilon = true;
    
    for (const auto& symbol : symbols) {
        // Add FIRST(symbol) - {ε} to result
        auto it = firstSets.find(symbol);
        if (it != firstSets.end()) {
            for (const auto& s : it->second) {
                if (!s.isEpsilon()) {
                    result.insert(s);
                }
            }
            
            // Check if symbol can derive epsilon
            if (it->second.find(epsilon) == it->second.end()) {
                allCanDeriveEpsilon = false;
                break;
            }
        } else {
            // Symbol not found in firstSets (shouldn't happen)
            allCanDeriveEpsilon = false;
            break;
        }
    }
    
    // If all symbols can derive epsilon, add epsilon to result
    if (allCanDeriveEpsilon) {
        result.insert(epsilon);
    }
    
    return result;
}

void Grammar::calculateFollow() {
    Symbol epsilon("ε", SymbolType::Epsilon);
    Symbol eof("EOF", SymbolType::Terminal);
    
    // Initialize FOLLOW sets
    for (const auto& nonTerminal : nonTerminals) {
        followSets[nonTerminal] = std::set<Symbol>();
    }
    
    // FOLLOW(start) contains EOF
    followSets[startSymbol].insert(eof);
    
    // Iterate until no changes
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (const auto& prod : productions) {
            const Symbol& A = prod.left;
            const std::vector<Symbol>& alpha = prod.right;
            
            for (size_t i = 0; i < alpha.size(); ++i) {
                const Symbol& B = alpha[i];
                
                if (!B.isNonTerminal()) continue;
                
                // Calculate FIRST(β) where β is the rest after B
                std::vector<Symbol> beta(alpha.begin() + i + 1, alpha.end());
                std::set<Symbol> firstBeta = computeFirst(beta);
                
                size_t beforeSize = followSets[B].size();
                
                // Add FIRST(β) - {ε} to FOLLOW(B)
                for (const auto& symbol : firstBeta) {
                    if (!symbol.isEpsilon()) {
                        followSets[B].insert(symbol);
                    }
                }
                
                // If β can derive ε or B is at the end, add FOLLOW(A) to FOLLOW(B)
                if (firstBeta.find(epsilon) != firstBeta.end() || i == alpha.size() - 1) {
                    followSets[B].insert(followSets[A].begin(), followSets[A].end());
                }
                
                if (followSets[B].size() > beforeSize) {
                    changed = true;
                }
            }
        }
    }
}

void Grammar::buildParsingTable() {
    Symbol epsilon("ε", SymbolType::Epsilon);
    
    for (size_t i = 0; i < productions.size(); ++i) {
        const Production& prod = productions[i];
        const Symbol& A = prod.left;
        
        // Calculate FIRST(α) where α is the RHS
        std::set<Symbol> firstAlpha = computeFirst(prod.right);
        
        // For each terminal in FIRST(α) - {ε}
        for (const auto& terminal : firstAlpha) {
            if (!terminal.isEpsilon()) {
                if (parsingTable[A].find(terminal) != parsingTable[A].end()) {
                    // Conflict detected - grammar is not LL(1)
                    std::cerr << "LL(1) conflict at [" << A.value << ", " << terminal.value << "]" << std::endl;
                }
                parsingTable[A][terminal] = i;
            }
        }
        
        // If ε is in FIRST(α)
        if (firstAlpha.find(epsilon) != firstAlpha.end()) {
            // For each terminal in FOLLOW(A)
            for (const auto& terminal : followSets[A]) {
                if (parsingTable[A].find(terminal) != parsingTable[A].end()) {
                    // Conflict detected - grammar is not LL(1)
                    std::cerr << "LL(1) conflict at [" << A.value << ", " << terminal.value << "]" << std::endl;
                }
                parsingTable[A][terminal] = i;
            }
        }
    }
}

// ============================================================================
// PARSING METHODS
// ============================================================================

std::unique_ptr<ParseTree> Grammar::parse(const std::vector<Token>& tokens) {
    auto root = std::make_unique<ParseNode>(startSymbol);
    auto tree = std::make_unique<ParseTree>(std::move(root));
    
    std::stack<ParseNode*> stack;
    initializeParsingStack(stack, tree->root.get());
    
    size_t tokenIndex = 0;
    
    while (!stack.empty() && stack.top() != nullptr) {
        ParseNode* currentNode = stack.top();
        
        if (currentNode->symbol.isNonTerminal()) {
            processNonTerminal(currentNode, tokens, tokenIndex, stack);
        }
        else if (currentNode->symbol.isTerminal()) {
            processTerminal(currentNode, tokens, tokenIndex, stack);
        }
        else { // Epsilon
            stack.pop();
        }
    }
    
    validateParsingCompletion(tokens, tokenIndex);
    return tree;
}

void Grammar::initializeParsingStack(std::stack<ParseNode*>& stack, ParseNode* root) {
    stack.push(nullptr); // Sentinel
    stack.push(root);
}

void Grammar::processNonTerminal(ParseNode* node, const std::vector<Token>& tokens, 
                                size_t& tokenIndex, std::stack<ParseNode*>& stack) {
    Token lookahead = getLookaheadToken(tokens, tokenIndex);
    Symbol lookaheadSymbol = getTerminalFromToken(lookahead);
    
    int productionIndex = findProductionInTable(node->symbol, lookaheadSymbol, lookahead);
    const Production& prod = productions[productionIndex];
    
    stack.pop();
    expandNonTerminal(node, prod, stack);
}

void Grammar::processTerminal(ParseNode* node, const std::vector<Token>& tokens, 
                             size_t& tokenIndex, std::stack<ParseNode*>& stack) {
    Token lookahead = getLookaheadToken(tokens, tokenIndex);
    Symbol lookaheadSymbol = getTerminalFromToken(lookahead);
    
    if (node->symbol == lookaheadSymbol) {
        node->token = lookahead;
        stack.pop();
        tokenIndex++;
    } else {
        throwTerminalMismatchError(node->symbol, lookahead);
    }
}

Token Grammar::getLookaheadToken(const std::vector<Token>& tokens, size_t index) const {
    return (index < tokens.size()) ? tokens[index] : Token("", TOKEN_EOF, 0, 0);
}

int Grammar::findProductionInTable(const Symbol& nonTerminal, const Symbol& terminal, 
                                  const Token& lookahead) const {
    auto nonTerminalIt = parsingTable.find(nonTerminal);
    if (nonTerminalIt == parsingTable.end()) {
        throwUnexpectedTokenError(lookahead);
    }
    
    auto terminalIt = nonTerminalIt->second.find(terminal);
    if (terminalIt == nonTerminalIt->second.end()) {
        throwUnexpectedTokenError(lookahead);
    }
    
    return terminalIt->second;
}

void Grammar::expandNonTerminal(ParseNode* node, const Production& prod, 
                               std::stack<ParseNode*>& stack) {
    std::vector<ParseNode*> childPtrs;
    
    // Add all children to the node in correct order
    for (size_t i = 0; i < prod.right.size(); ++i) {
        auto child = std::make_unique<ParseNode>(prod.right[i]);
        ParseNode* childPtr = child.get();
        childPtrs.push_back(childPtr);
        node->addChild(std::move(child));
    }
    
    // Push children to stack in reverse order for processing
    for (int i = childPtrs.size() - 1; i >= 0; --i) {
        stack.push(childPtrs[i]);
    }
}

void Grammar::validateParsingCompletion(const std::vector<Token>& tokens, size_t tokenIndex) const {
    if (tokenIndex < tokens.size() && tokens[tokenIndex].type != TOKEN_EOF) {
        throw std::runtime_error("Syntax error: unexpected tokens after end of program");
    }
}

void Grammar::throwUnexpectedTokenError(const Token& lookahead) const {
    throw std::runtime_error("Syntax error: unexpected token '" + lookahead.lexeme + 
                           "' at line " + std::to_string(lookahead.line));
}

void Grammar::throwTerminalMismatchError(const Symbol& expected, const Token& found) const {
    throw std::runtime_error("Syntax error: expected '" + expected.value + 
                           "' but found '" + found.lexeme + 
                           "' at line " + std::to_string(found.line));
}

// ============================================================================
// TOKEN TO SYMBOL MAPPING AND UTILITY METHODS
// ============================================================================

Symbol Grammar::getTerminalFromToken(const Token& token) const {
    // Map token types to terminal symbols
    switch (token.type) {
        case TOKEN_FUNCTION: return Symbol("FUNCTION", SymbolType::Terminal);
        case TOKEN_TYPE: return Symbol("TYPE", SymbolType::Terminal);
        case TOKEN_INHERITS: return Symbol("INHERITS", SymbolType::Terminal);
        case TOKEN_NEW: return Symbol("NEW", SymbolType::Terminal);
        case TOKEN_BASE: return Symbol("BASE", SymbolType::Terminal);
        case TOKEN_IF: return Symbol("IF", SymbolType::Terminal);
        case TOKEN_ELIF: return Symbol("ELIF", SymbolType::Terminal);
        case TOKEN_ELSE: return Symbol("ELSE", SymbolType::Terminal);
        case TOKEN_WHILE: return Symbol("WHILE", SymbolType::Terminal);
        case TOKEN_FOR: return Symbol("FOR", SymbolType::Terminal);
        case TOKEN_IN: return Symbol("IN", SymbolType::Terminal);
        case TOKEN_IS: return Symbol("IS", SymbolType::Terminal);
        case TOKEN_AS: return Symbol("AS", SymbolType::Terminal);
        case TOKEN_LET: return Symbol("LET", SymbolType::Terminal);
        case TOKEN_PRINT: return Symbol("PRINT", SymbolType::Terminal);
        case TOKEN_TRUE: return Symbol("TRUE", SymbolType::Terminal);
        case TOKEN_FALSE: return Symbol("FALSE", SymbolType::Terminal);
        case TOKEN_TYPE_NUMBER: return Symbol("TYPE_NUMBER", SymbolType::Terminal);
        case TOKEN_TYPE_STRING: return Symbol("TYPE_STRING", SymbolType::Terminal);
        case TOKEN_TYPE_BOOLEAN: return Symbol("TYPE_BOOLEAN", SymbolType::Terminal);
        case TOKEN_IDENTIFIER: return Symbol("IDENTIFIER", SymbolType::Terminal);
        case TOKEN_NUMBER: return Symbol("NUMBER", SymbolType::Terminal);
        case TOKEN_STRING: return Symbol("STRING", SymbolType::Terminal);
        case TOKEN_LPAREN: return Symbol("LPAREN", SymbolType::Terminal);
        case TOKEN_RPAREN: return Symbol("RPAREN", SymbolType::Terminal);
        case TOKEN_LBRACE: return Symbol("LBRACE", SymbolType::Terminal);
        case TOKEN_RBRACE: return Symbol("RBRACE", SymbolType::Terminal);
        case TOKEN_SEMICOLON: return Symbol("SEMICOLON", SymbolType::Terminal);
        case TOKEN_COMMA: return Symbol("COMMA", SymbolType::Terminal);
        case TOKEN_COLON: return Symbol("COLON", SymbolType::Terminal);
        case TOKEN_DOT: return Symbol("DOT", SymbolType::Terminal);
        case TOKEN_PLUS: return Symbol("PLUS", SymbolType::Terminal);
        case TOKEN_MINUS: return Symbol("MINUS", SymbolType::Terminal);
        case TOKEN_MULTIPLY: return Symbol("MULTIPLY", SymbolType::Terminal);
        case TOKEN_DIVIDE: return Symbol("DIVIDE", SymbolType::Terminal);
        case TOKEN_MODULO: return Symbol("MODULO", SymbolType::Terminal);
        case TOKEN_POWER: return Symbol("POWER", SymbolType::Terminal);
        case TOKEN_CONCAT: return Symbol("CONCAT", SymbolType::Terminal);
        case TOKEN_CONCAT_SPACE: return Symbol("CONCAT_SPACE", SymbolType::Terminal);
        case TOKEN_EQUALS: return Symbol("EQUALS", SymbolType::Terminal);
        case TOKEN_ASSIGN: return Symbol("ASSIGN", SymbolType::Terminal);
        case TOKEN_EQEQ: return Symbol("EQEQ", SymbolType::Terminal);
        case TOKEN_NOTEQ: return Symbol("NOTEQ", SymbolType::Terminal);
        case TOKEN_LESS: return Symbol("LESS", SymbolType::Terminal);
        case TOKEN_LESSEQ: return Symbol("LESSEQ", SymbolType::Terminal);
        case TOKEN_GREATER: return Symbol("GREATER", SymbolType::Terminal);
        case TOKEN_GREATEREQ: return Symbol("GREATEREQ", SymbolType::Terminal);
        case TOKEN_AND: return Symbol("AND", SymbolType::Terminal);
        case TOKEN_OR: return Symbol("OR", SymbolType::Terminal);
        case TOKEN_NOT: return Symbol("NOT", SymbolType::Terminal);
        case TOKEN_ARROW: return Symbol("ARROW", SymbolType::Terminal);
        case TOKEN_EOF: return Symbol("EOF", SymbolType::Terminal);
        default: return Symbol("UNKNOWN", SymbolType::Terminal);
    }
}

std::string Grammar::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// ============================================================================
// VALIDATION AND CONFLICT DETECTION METHODS
// ============================================================================

bool Grammar::isLL1() const {
    // Check if there are any conflicts in the parsing table
    for (const auto& [nonTerminal, row] : parsingTable) {
        std::set<int> usedProductions;
        for (const auto& [terminal, production] : row) {
            if (usedProductions.find(production) != usedProductions.end()) {
                return false;
            }
            usedProductions.insert(production);
        }
    }
    return true;
}

std::vector<std::string> Grammar::getConflicts() const {
    std::vector<std::string> conflicts;
    
    // Check each non-terminal
    for (const auto& nonTerminal : nonTerminals) {
        std::map<Symbol, std::vector<int>> entries;
        
        // Collect all productions for each terminal
        for (size_t i = 0; i < productions.size(); ++i) {
            if (productions[i].left == nonTerminal) {
                std::set<Symbol> firstSet = computeFirst(productions[i].right);
                
                for (const auto& terminal : firstSet) {
                    if (!terminal.isEpsilon()) {
                        entries[terminal].push_back(i);
                    }
                }
                
                // If epsilon is in FIRST, add FOLLOW terminals
                if (firstSet.find(Symbol("ε", SymbolType::Epsilon)) != firstSet.end()) {
                    for (const auto& terminal : followSets.at(nonTerminal)) {
                        entries[terminal].push_back(i);
                    }
                }
            }
        }
        
        // Check for conflicts
        for (const auto& [terminal, prods] : entries) {
            if (prods.size() > 1) {
                std::stringstream ss;
                ss << "Conflict at [" << nonTerminal.value << ", " << terminal.value << "]: productions ";
                for (size_t i = 0; i < prods.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << prods[i];
                }
                conflicts.push_back(ss.str());
            }
        }
    }
    
    return conflicts;
}

void Grammar::printFirst() const {
    std::cout << "FIRST sets:" << std::endl;
    for (const auto& [symbol, firstSet] : firstSets) {
        if (symbol.isNonTerminal()) {
            std::cout << "  FIRST(" << symbol.value << ") = { ";
            bool first = true;
            for (const auto& s : firstSet) {
                if (!first) std::cout << ", ";
                std::cout << s.value;
                first = false;
            }
            std::cout << " }" << std::endl;
        }
    }
}

void Grammar::printFollow() const {
    std::cout << "FOLLOW sets:" << std::endl;
    for (const auto& [symbol, followSet] : followSets) {
        std::cout << "  FOLLOW(" << symbol.value << ") = { ";
        bool first = true;
        for (const auto& s : followSet) {
            if (!first) std::cout << ", ";
            std::cout << s.value;
            first = false;
        }
        std::cout << " }" << std::endl;
    }
}

void Grammar::printParsingTable() const {
    std::cout << "LL(1) Parsing Table:" << std::endl;
    for (const auto& [nonTerminal, row] : parsingTable) {
        std::cout << "  " << nonTerminal.value << ":" << std::endl;
        for (const auto& [terminal, production] : row) {
            std::cout << "    [" << terminal.value << "] -> production " << production << std::endl;
        }
    }
}

} // namespace Parser