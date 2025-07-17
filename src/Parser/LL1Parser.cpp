#include "Parser/LL1Parser.hpp"
#include "Parser/ParserUtils.hpp"
#include <iostream>
#include <stack>
#include <stdexcept>

namespace Parser {

// ============================================================================
// STATIC LOADING METHOD
// ============================================================================

LL1Parser LL1Parser::loadFromFile(const std::string& filename) {
    LL1Parser parser;

    // Use GrammarLoader to load grammar data
    if (parser.grammarLoader.loadFromFile(filename)) {
        parser.copyFromLoader(parser.grammarLoader);
        parser.initializeGrammar();
    }

    return parser;
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

            // Calculate FIRST for the RHS using ParserUtils
            std::unordered_set<std::string> rhsFirst = ParserUtils::computeFirst(prod.right, firstSets);

            // Add to FIRST(A)
            firstSets[A].insert(rhsFirst.begin(), rhsFirst.end());

            if (firstSets[A].size() > beforeSize) {
                changed = true;
            }
        }
    }
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
                std::unordered_set<std::string> firstBeta = ParserUtils::computeFirst(beta, firstSets);

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
        std::unordered_set<std::string> firstAlpha = ParserUtils::computeFirst(prod.right, firstSets);

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

void LL1Parser::initializeGrammar() {
    calculateFirst();
    calculateFollow();
    buildParsingTable();
}

// ============================================================================
// PARSING ALGORITHM (CORE FUNCTIONALITY)
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
    std::string lookaheadSymbol = tokenMapper.getTerminalFromToken(lookahead);

    size_t productionIndex = findProductionInTable(node->symbol.value, lookaheadSymbol, lookahead);
    const Production& prod = productions[productionIndex];

    stack.pop();
    expandNonTerminal(node, prod, stack);
}

void LL1Parser::processTerminal(ParseNode* node, const std::vector<Token>& tokens,
                             size_t& tokenIndex, std::stack<ParseNode*>& stack) {
    Token lookahead = getLookaheadToken(tokens, tokenIndex);
    std::string lookaheadSymbol = tokenMapper.getTerminalFromToken(lookahead);

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

// ============================================================================
// ERROR HANDLING METHODS
// ============================================================================

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
// VALIDATION AND DEBUG METHODS (USING PARSERUTILS)
// ============================================================================

bool LL1Parser::isLL1() const {
    return ParserUtils::isLL1Grammar(parsingTable);
}

std::vector<std::string> LL1Parser::getConflicts() const {
    return ParserUtils::findConflicts(nonTerminals, productions, firstSets, followSets);
}

void LL1Parser::printFirst() const {
    ParserUtils::printFirstSets(firstSets, nonTerminals);
}

void LL1Parser::printFollow() const {
    ParserUtils::printFollowSets(followSets);
}

void LL1Parser::printParsingTable() const {
    ParserUtils::printParsingTable(parsingTable);
}

// ============================================================================
// PRIVATE HELPER METHODS
// ============================================================================

void LL1Parser::copyFromLoader(const GrammarLoader& loader) {
    terminals = loader.terminals;
    nonTerminals = loader.nonTerminals;
    productions = loader.productions;
    startSymbol = loader.startSymbol;
}

} // namespace Parser
