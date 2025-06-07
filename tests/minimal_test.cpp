#include <iostream>
#include <memory>

#include "../include/source_file.h"

using namespace chronovyan;

int main() {
    std::cout << "Running minimal Chronovyan test" << std::endl;
    std::cout << "=============================" << std::endl;

    try {
        // Create a simple source file
        std::string source = "var x = 42;";
        std::cout << "Source: '" << source << "'" << std::endl;

        auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.chron");
        std::cout << "Created SourceFile" << std::endl;

        // Try to read content
        std::string content = sourceFile->getSource();
        std::cout << "Source file content: '" << content << "'" << std::endl;

        std::cout << "Test completed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }

    return 0;
}