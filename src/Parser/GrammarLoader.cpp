#include "Parser/GrammarLoader.hpp"
#include "Parser/ParserUtils.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace Parser {

bool GrammarLoader::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open grammar file: " + filename);
    }

    std::vector<std::string> productionLines;

    parseGrammarFile(file, productionLines);
    processProductions(productionLines);
    initializeAfterLoading();

    return true;
}

void GrammarLoader::parseGrammarFile(std::ifstream& file, std::vector<std::string>& productionLines) {
    std::string line;
    bool readingProductions = false;

    while (std::getline(file, line)) {
        line = ParserUtils::trim(line);
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

void GrammarLoader::parseTerminals(const std::string& line) {
    size_t colonPos = line.find(":");
    std::string termsStr = ParserUtils::trim(line.substr(colonPos + 1));
    std::istringstream iss(termsStr);
    std::string name;

    while (std::getline(iss, name, ',')) {
        name = ParserUtils::trim(name);
        if (!name.empty()) {
            terminals.insert(name);
        }
    }
}

void GrammarLoader::parseNonTerminals(const std::string& line) {
    size_t colonPos = line.find(":");
    std::string ntsStr = ParserUtils::trim(line.substr(colonPos + 1));
    std::istringstream iss(ntsStr);
    std::string name;

    while (std::getline(iss, name, ',')) {
        name = ParserUtils::trim(name);
        if (!name.empty()) {
            nonTerminals.insert(name);
        }
    }
}

void GrammarLoader::processProductions(const std::vector<std::string>& productionLines) {
    for (const auto& prodLine : productionLines) {
        processProductionLine(prodLine);
    }
}

void GrammarLoader::processProductionLine(const std::string& prodLine) {
    size_t arrowPos = prodLine.find("->");
    if (arrowPos == std::string::npos) return;

    std::string lhsStr = ParserUtils::trim(prodLine.substr(0, arrowPos));
    Symbol lhsSymbol(lhsStr, SymbolType::NonTerminal);
    std::string rhsStr = ParserUtils::trim(prodLine.substr(arrowPos + 2));

    std::vector<std::string> alternatives = splitAlternatives(rhsStr);

    for (const auto& altStr : alternatives) {
        std::vector<Symbol> rhsSymbols = parseRightHandSide(altStr);
        productions.push_back(Production(lhsSymbol, rhsSymbols));
    }
}

std::vector<std::string> GrammarLoader::splitAlternatives(const std::string& rhsStr) {
    std::vector<std::string> alternatives;
    std::istringstream altStream(rhsStr);
    std::string alt;

    while (std::getline(altStream, alt, '|')) {
        alternatives.push_back(ParserUtils::trim(alt));
    }

    return alternatives;
}

std::vector<Symbol> GrammarLoader::parseRightHandSide(const std::string& altStr) {
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

SymbolType GrammarLoader::determineSymbolType(const std::string& token) {
    if (nonTerminals.find(token) != nonTerminals.end()) {
        return SymbolType::NonTerminal;
    }
    return SymbolType::Terminal;
}

void GrammarLoader::setStartSymbol() {
    if (!productions.empty()) {
        startSymbol = productions[0].left;
    }
}

void GrammarLoader::initializeAfterLoading() {
    setStartSymbol();
}

} // namespace Parser
