#include <iostream>
#include <string>

int main() {
    std::cout << "Running super minimal Chronovyan test" << std::endl;
    std::cout << "====================================" << std::endl;

    // Define a simple string for our "source code"
    std::string source = "var x = 42;";
    std::cout << "Source: '" << source << "'" << std::endl;

    // Manually parse the string to verify basic string operations
    if (source.find("var") == 0) {
        std::cout << "Found 'var' keyword at position 0" << std::endl;
    }

    size_t pos = source.find("x");
    if (pos != std::string::npos) {
        std::cout << "Found variable name 'x' at position " << pos << std::endl;
    }

    pos = source.find("42");
    if (pos != std::string::npos) {
        std::cout << "Found integer literal '42' at position " << pos << std::endl;
    }

    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}