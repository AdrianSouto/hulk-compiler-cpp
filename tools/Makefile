
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../include

GENERATOR_SOURCES = generate_lexer.cpp ../src/Lexer/LexerGenerator.cpp
TARGET = generate_lexer.exe

.PHONY: all clean generate

all: $(TARGET)

$(TARGET): $(GENERATOR_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

generate: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf generated

help:
	@echo "Available targets:"
	@echo "  all      - Build the lexer generator"
	@echo "  generate - Build and run the lexer generator"
	@echo "  clean    - Remove generated files"
	@echo "  help     - Show this help message"