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

LL1Parser LL1Parser::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open grammar file: " + filename);
    }

    LL1Parser grammar;
    std::vector<std::string> productionLines;

    grammar.parseGrammarFile(file, productionLines);
    grammar.processProductions(productionLines);
    grammar.initializeGrammar();

    return grammar;
}

void LL1Parser::parseGrammarFile(std::ifstream& file, std::vector<std::string>& productionLines) {
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

void LL1Parser::parseTerminals(const std::string& line) {
    size_t colonPos = line.find(":");
    std::string termsStr = trim(line.substr(colonPos + 1));
    std::istringstream iss(termsStr);
    std::string name;

    while (std::getline(iss, name, ',')) {
        name = trim(name);
        if (!name.empty()) {
            terminals.insert(name);
        }
    }
}

void LL1Parser::parseNonTerminals(const std::string& line) {
    size_t colonPos = line.find(":");
    std::string ntsStr = trim(line.substr(colonPos + 1));
    std::istringstream iss(ntsStr);
    std::string name;

    while (std::getline(iss, name, ',')) {
        name = trim(name);
        if (!name.empty()) {
            nonTerminals.insert(name);
        }
    }
}

void LL1Parser::processProductions(const std::vector<std::string>& productionLines) {
    for (const auto& prodLine : productionLines) {
        processProductionLine(prodLine);
    }
}

void LL1Parser::processProductionLine(const std::string& prodLine) {
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

std::vector<std::string> LL1Parser::splitAlternatives(const std::string& rhsStr) {
    std::vector<std::string> alternatives;
    std::istringstream altStream(rhsStr);
    std::string alt;

    while (std::getline(altStream, alt, '|')) {
        alternatives.push_back(trim(alt));
    }

    return alternatives;
}

std::vector<Symbol> LL1Parser::parseRightHandSide(const std::string& altStr) {
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

SymbolType LL1Parser::determineSymbolType(const std::string& token) {
    if (nonTerminals.find(token) != nonTerminals.end()) {
        return SymbolType::NonTerminal;
    }
    return SymbolType::Terminal;
}

void LL1Parser::initializeGrammar() {
    setStartSymbol();
    calculateFirst();
    calculateFollow();
    buildParsingTable();
}

void LL1Parser::setStartSymbol() {
    if (!productions.empty()) {
        startSymbol = productions[0].left;
    }
}

// ============================================================================
// FIRST AND FOLLOW SET CALCULATION
// ============================================================================

void LL1Parser::calculateFirst() {
    initializeFirstSets();
    computeFirstSetsIteratively();
}

void LL1Parser::initializeFirstSets() {
    // For each terminal, FIRST(terminal) = {terminal}
    for (const auto& terminal : terminals) {
        firstSets[terminal].insert(terminal);
    }

    // For each non-terminal, initialize with empty set
    for (const auto& nonTerminal : nonTerminals) {
        firstSets[nonTerminal] = std::unordered_set<std::string>();
    }

    // Add epsilon to its own FIRST set
    firstSets["ε"].insert("ε");
}

void LL1Parser::computeFirstSetsIteratively() {
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& prod : productions) {
            const std::string& A = prod.left.value;
            size_t beforeSize = firstSets[A].size();

            // Calculate FIRST for the RHS
            std::unordered_set<std::string> rhsFirst = computeFirst(prod.right);

            // Add to FIRST(A)
            firstSets[A].insert(rhsFirst.begin(), rhsFirst.end());

            if (firstSets[A].size() > beforeSize) {
                changed = true;
            }
        }
    }
}

std::unordered_set<std::string> LL1Parser::computeFirst(const std::vector<Symbol>& symbols) const {
    std::unordered_set<std::string> result;

    if (symbols.empty()) {
        result.insert("ε");
        return result;
    }

    bool allCanDeriveEpsilon = true;

    for (const auto& symbol : symbols) {
        auto it = firstSets.find(symbol.value);
        if (it != firstSets.end()) {
            // Add FIRST(symbol) - {ε} to result
            for (const auto& s : it->second) {
                if (s != "ε") {
                    result.insert(s);
                }
            }

            // Check if symbol can derive epsilon
            if (it->second.find("ε") == it->second.end()) {
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
        result.insert("ε");
    }

    return result;
}

void LL1Parser::calculateFollow() {
    // Initialize FOLLOW sets
    for (const auto& nonTerminal : nonTerminals) {
        followSets[nonTerminal] = std::unordered_set<std::string>();
    }

    // FOLLOW(start) contains EOF
    followSets[startSymbol.value].insert("EOF");

    // Iterate until no changes
    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& prod : productions) {
            const std::string& A = prod.left.value;
            const std::vector<Symbol>& alpha = prod.right;

            for (size_t i = 0; i < alpha.size(); ++i) {
                const Symbol& B = alpha[i];

                if (!B.isNonTerminal()) continue;

                // Calculate FIRST(β) where β is the rest after B
                std::vector<Symbol> beta(alpha.begin() + i + 1, alpha.end());
                std::unordered_set<std::string> firstBeta = computeFirst(beta);

                size_t beforeSize = followSets[B.value].size();

                // Add FIRST(β) - {ε} to FOLLOW(B)
                for (const auto& symbol : firstBeta) {
                    if (symbol != "ε") {
                        followSets[B.value].insert(symbol);
                    }
                }

                // If β can derive ε or B is at the end, add FOLLOW(A) to FOLLOW(B)
                if (firstBeta.find("ε") != firstBeta.end() || i == alpha.size() - 1) {
                    followSets[B.value].insert(followSets[A].begin(), followSets[A].end());
                }

                if (followSets[B.value].size() > beforeSize) {
                    changed = true;
                }
            }
        }
    }
}

void LL1Parser::buildParsingTable() {
    for (size_t i = 0; i < productions.size(); ++i) {
        const Production& prod = productions[i];
        const std::string& A = prod.left.value;

        // Calculate FIRST(α) where α is the RHS
        std::unordered_set<std::string> firstAlpha = computeFirst(prod.right);

        // For each terminal in FIRST(α) - {ε}
        for (const auto& terminal : firstAlpha) {
            if (terminal != "ε") {
                if (parsingTable[A].find(terminal) != parsingTable[A].end()) {
                    // Conflict detected - grammar is not LL(1)
                    std::cerr << "LL(1) conflict at [" << A << ", " << terminal << "]" << std::endl;
                }
                parsingTable[A][terminal] = i;
            }
        }

        // If ε is in FIRST(α)
        if (firstAlpha.find("ε") != firstAlpha.end()) {
            // For each terminal in FOLLOW(A)
            for (const auto& terminal : followSets[A]) {
                if (parsingTable[A].find(terminal) != parsingTable[A].end()) {
                    // Conflict detected - grammar is not LL(1)
                    std::cerr << "LL(1) conflict at [" << A << ", " << terminal << "]" << std::endl;
                }
                parsingTable[A][terminal] = i;
            }
        }
    }
}

// ============================================================================
// PARSING METHODS
// ============================================================================

std::unique_ptr<ParseTree> LL1Parser::parse(const std::vector<Token>& tokens) {
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

void LL1Parser::initializeParsingStack(std::stack<ParseNode*>& stack, ParseNode* root) {
    stack.push(nullptr); // Sentinel
    stack.push(root);
}

void LL1Parser::processNonTerminal(ParseNode* node, const std::vector<Token>& tokens,
                                size_t& tokenIndex, std::stack<ParseNode*>& stack) {
    Token lookahead = getLookaheadToken(tokens, tokenIndex);
    std::string lookaheadSymbol = getTerminalFromToken(lookahead);

    size_t productionIndex = findProductionInTable(node->symbol.value, lookaheadSymbol, lookahead);
    const Production& prod = productions[productionIndex];

    stack.pop();
    expandNonTerminal(node, prod, stack);
}

void LL1Parser::processTerminal(ParseNode* node, const std::vector<Token>& tokens,
                             size_t& tokenIndex, std::stack<ParseNode*>& stack) {
    Token lookahead = getLookaheadToken(tokens, tokenIndex);
    std::string lookaheadSymbol = getTerminalFromToken(lookahead);

    if (node->symbol.value == lookaheadSymbol) {
        node->token = lookahead;
        stack.pop();
        tokenIndex++;
    } else {
        throwTerminalMismatchError(node->symbol.value, lookahead);
    }
}

Token LL1Parser::getLookaheadToken(const std::vector<Token>& tokens, size_t index) const {
    return (index < tokens.size()) ? tokens[index] : Token("", TOKEN_EOF, 0, 0);
}

size_t LL1Parser::findProductionInTable(const std::string& nonTerminal, const std::string& terminal,
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

void LL1Parser::expandNonTerminal(ParseNode* node, const Production& prod,
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

void LL1Parser::validateParsingCompletion(const std::vector<Token>& tokens, size_t tokenIndex) const {
    if (tokenIndex < tokens.size() && tokens[tokenIndex].type != TOKEN_EOF) {
        throw std::runtime_error("Syntax error: unexpected tokens after end of program");
    }
}

void LL1Parser::throwUnexpectedTokenError(const Token& lookahead) const {
    throw std::runtime_error("Syntax error: unexpected token '" + lookahead.lexeme +
                           "' at line " + std::to_string(lookahead.line));
}

void LL1Parser::throwTerminalMismatchError(const std::string& expected, const Token& found) const {
    throw std::runtime_error("Syntax error: expected '" + expected +
                           "' but found '" + found.lexeme +
                           "' at line " + std::to_string(found.line));
}

// ============================================================================
// TOKEN TO SYMBOL MAPPING AND UTILITY METHODS
// ============================================================================

std::string LL1Parser::getTerminalFromToken(const Token& token) const {
    // Use cache for performance
    auto it = tokenSymbolCache.find(static_cast<int>(token.type));
    if (it != tokenSymbolCache.end()) {
        return it->second;
    }

    // Map token types to terminal symbols
    std::string result;
    switch (token.type) {
        case TOKEN_FUNCTION: result = "FUNCTION"; break;
        case TOKEN_TYPE: result = "TYPE"; break;
        case TOKEN_INHERITS: result = "INHERITS"; break;
        case TOKEN_NEW: result = "NEW"; break;
        case TOKEN_BASE: result = "BASE"; break;
        case TOKEN_IF: result = "IF"; break;
        case TOKEN_ELIF: result = "ELIF"; break;
        case TOKEN_ELSE: result = "ELSE"; break;
        case TOKEN_WHILE: result = "WHILE"; break;
        case TOKEN_FOR: result = "FOR"; break;
        case TOKEN_IN: result = "IN"; break;
        case TOKEN_IS: result = "IS"; break;
        case TOKEN_AS: result = "AS"; break;
        case TOKEN_LET: result = "LET"; break;
        case TOKEN_PRINT: result = "PRINT"; break;
        case TOKEN_TRUE: result = "TRUE"; break;
        case TOKEN_FALSE: result = "FALSE"; break;
        case TOKEN_TYPE_NUMBER: result = "TYPE_NUMBER"; break;
        case TOKEN_TYPE_STRING: result = "TYPE_STRING"; break;
        case TOKEN_TYPE_BOOLEAN: result = "TYPE_BOOLEAN"; break;
        case TOKEN_IDENTIFIER: result = "IDENTIFIER"; break;
        case TOKEN_NUMBER: result = "NUMBER"; break;
        case TOKEN_STRING: result = "STRING"; break;
        case TOKEN_LPAREN: result = "LPAREN"; break;
        case TOKEN_RPAREN: result = "RPAREN"; break;
        case TOKEN_LBRACE: result = "LBRACE"; break;
        case TOKEN_RBRACE: result = "RBRACE"; break;
        case TOKEN_SEMICOLON: result = "SEMICOLON"; break;
        case TOKEN_COMMA: result = "COMMA"; break;
        case TOKEN_COLON: result = "COLON"; break;
        case TOKEN_DOT: result = "DOT"; break;
        case TOKEN_PLUS: result = "PLUS"; break;
        case TOKEN_MINUS: result = "MINUS"; break;
        case TOKEN_MULTIPLY: result = "MULTIPLY"; break;
        case TOKEN_DIVIDE: result = "DIVIDE"; break;
        case TOKEN_MODULO: result = "MODULO"; break;
        case TOKEN_POWER: result = "POWER"; break;
        case TOKEN_CONCAT: result = "CONCAT"; break;
        case TOKEN_CONCAT_SPACE: result = "CONCAT_SPACE"; break;
        case TOKEN_EQUALS: result = "EQUALS"; break;
        case TOKEN_ASSIGN: result = "ASSIGN"; break;
        case TOKEN_EQEQ: result = "EQEQ"; break;
        case TOKEN_NOTEQ: result = "NOTEQ"; break;
        case TOKEN_LESS: result = "LESS"; break;
        case TOKEN_LESSEQ: result = "LESSEQ"; break;
        case TOKEN_GREATER: result = "GREATER"; break;
        case TOKEN_GREATEREQ: result = "GREATEREQ"; break;
        case TOKEN_AND: result = "AND"; break;
        case TOKEN_OR: result = "OR"; break;
        case TOKEN_NOT: result = "NOT"; break;
        case TOKEN_ARROW: result = "ARROW"; break;
        case TOKEN_EOF: result = "EOF"; break;
        default: result = "UNKNOWN"; break;
    }

    // Cache the result
    tokenSymbolCache[static_cast<int>(token.type)] = result;
    return result;
}

std::string LL1Parser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// ============================================================================
// VALIDATION AND CONFLICT DETECTION METHODS
// ============================================================================

bool LL1Parser::isLL1() const {
    // Check if there are any conflicts in the parsing table
    for (const auto& [nonTerminal, row] : parsingTable) {
        std::unordered_set<size_t> usedProductions;
        for (const auto& [terminal, production] : row) {
            if (usedProductions.find(production) != usedProductions.end()) {
                return false;
            }
            usedProductions.insert(production);
        }
    }
    return true;
}

std::vector<std::string> LL1Parser::getConflicts() const {
    std::vector<std::string> conflicts;

    // Check each non-terminal
    for (const auto& nonTerminal : nonTerminals) {
        std::unordered_map<std::string, std::vector<size_t>> entries;

        // Collect all productions for each terminal
        for (size_t i = 0; i < productions.size(); ++i) {
            if (productions[i].left.value == nonTerminal) {
                std::unordered_set<std::string> firstSet = computeFirst(productions[i].right);

                for (const auto& terminal : firstSet) {
                    if (terminal != "ε") {
                        entries[terminal].push_back(i);
                    }
                }

                // If epsilon is in FIRST, add FOLLOW terminals
                if (firstSet.find("ε") != firstSet.end()) {
                    auto followIt = followSets.find(nonTerminal);
                    if (followIt != followSets.end()) {
                        for (const auto& terminal : followIt->second) {
                            entries[terminal].push_back(i);
                        }
                    }
                }
            }
        }

        // Check for conflicts
        for (const auto& [terminal, prods] : entries) {
            if (prods.size() > 1) {
                std::stringstream ss;
                ss << "Conflict at [" << nonTerminal << ", " << terminal << "]: productions ";
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

void LL1Parser::printFirst() const {
    std::cout << "FIRST sets:" << std::endl;
    for (const auto& [symbol, firstSet] : firstSets) {
        if (nonTerminals.find(symbol) != nonTerminals.end()) {
            std::cout << "  FIRST(" << symbol << ") = { ";
            bool first = true;
            for (const auto& s : firstSet) {
                if (!first) std::cout << ", ";
                std::cout << s;
                first = false;
            }
            std::cout << " }" << std::endl;
        }
    }
}

void LL1Parser::printFollow() const {
    std::cout << "FOLLOW sets:" << std::endl;
    for (const auto& [symbol, followSet] : followSets) {
        std::cout << "  FOLLOW(" << symbol << ") = { ";
        bool first = true;
        for (const auto& s : followSet) {
            if (!first) std::cout << ", ";
            std::cout << s;
            first = false;
        }
        std::cout << " }" << std::endl;
    }
}

void LL1Parser::printParsingTable() const {
    std::cout << "LL(1) Parsing Table:" << std::endl;
    for (const auto& [nonTerminal, row] : parsingTable) {
        std::cout << "  " << nonTerminal << ":" << std::endl;
        for (const auto& [terminal, production] : row) {
            std::cout << "    [" << terminal << "] -> production " << production << std::endl;
        }
    }
}

} // namespace Parser
