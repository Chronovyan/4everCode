#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "SimpleLexer.h"

// Helper function to read a file into a string
std::string readFile(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path);
  }

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  return content;
}

int main(int argc, char *argv[]) {
  std::cout << "Forever Minimal Compiler v0.1" << std::endl;

  if (argc < 2) {
    std::cerr << "Error: No input file specified" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }

  std::string inputFile = argv[1];

  // Check file extension
  if (inputFile.size() < 4 || inputFile.substr(inputFile.size() - 3) != ".4e") {
    std::cerr << "Error: Input file must have a .4e extension" << std::endl;
    return 1;
  }

  std::cout << "Compiling: " << inputFile << std::endl;

  try {
    // Read the input file
    std::string source = readFile(inputFile);

    // Tokenize the source code
    SimpleLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    if (lexer.hadError()) {
      std::cerr << "Lexing failed with errors" << std::endl;
      return 1;
    }

    // Print the tokens
    std::cout << "\nTokens (" << tokens.size() << "):\n";
    for (const auto &token : tokens) {
      std::cout << token.toString() << '\n';
    }

    std::cout << "\nCompilation completed successfully" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
