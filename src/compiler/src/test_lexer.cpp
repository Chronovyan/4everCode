#include <iostream>
#include <memory>
#include <string>

#include "compiler/Diagnostic.h"
#include "compiler/Lexer.h"

using namespace chronovyan::compiler;

void testCharacterLiterals() {
  std::string source = "'a' '\\n' '\\'' '\\0' '\\x41'";
  std::string filename = "test.chr";
  auto reporter = std::make_shared<DiagnosticReporter>();

  Lexer lexer(source, filename, reporter);
  std::vector<Token> tokens = lexer.tokenize();

  std::cout << "Testing character literals...\n";
  for (const auto &token : tokens) {
    std::cout << "Token: " << token.toString() << "\n";
    if (token.getType() == TokenType::CHARACTER) {
      int charValue = token.getAs<int>();
      std::cout << "  Character value: " << charValue << " ('"
                << static_cast<char>(charValue) << "')" << std::endl;
    }
  }

  if (reporter->hasErrors()) {
    std::cerr << "Errors found during lexing:\n";
    for (const auto &error : reporter->getErrors()) {
      std::cerr << error.toString() << "\n";
    }
  }
}

int main() {
  testCharacterLiterals();
  return 0;
}
