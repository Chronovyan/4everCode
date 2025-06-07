#include <iostream>
#include <memory>

#include "include/error_handler.h"
#include "include/interpreter.h"
#include "include/parser.h"
#include "include/source_file.h"

using namespace chronovyan;

int main() {
    // Create a test source without indentation
    std::string source = "DEFINE_TYPE Person AS STRUCT { name: STRING; age: INT; }";

    // Create a source file from the string
    auto sourceFile = std::make_shared<SourceFile>("test", source);
    auto lexer = std::make_shared<Lexer>(sourceFile);

    // Create parser
    Parser parser(lexer);
    auto program = parser.parse();

    if (program == nullptr) {
        std::cerr << "Failed to parse program" << std::endl;
        return 1;
    }

    std::cout << "Successfully parsed program" << std::endl;

    return 0;
}