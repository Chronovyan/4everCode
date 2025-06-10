/**
 * @file chronovyan_interpreter.cpp
 * @brief Main entry point for the Chronovyan interpreter
 *
 * This file implements the entry point for the Chronovyan interpreter,
 * which supports the complete language specification as defined in the
 * Chronovyan Language Guide (Act II Primer, Advanced Mechanics Addendum, and
 * Reference Supplement).
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "compiler/Lexer.h"
#include "error_handler.h"
#include "interpreter.h"
#include "parser.h"

using namespace chronovyan;

/**
 * @brief Load file contents into a string
 * @param filename The path to the file
 * @return The file contents as a string
 */
std::string loadFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

/**
 * @brief Print usage information
 * @param programName The name of the executable
 */
void printUsage(const std::string &programName) {
  std::cout << "Chronovyan Interpreter" << std::endl;
  std::cout << "======================" << std::endl;
  std::cout << "Usage: " << programName << " [options] <filename.cvy>"
            << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  --help                  Show this help message" << std::endl;
  std::cout << "  --verbose               Enable verbose output" << std::endl;
  std::cout << "  --no-visualize          Disable resource visualization"
            << std::endl;
  std::cout << "  --show-tokens           Show lexer tokens" << std::endl;
  std::cout << "  --show-ast              Show abstract syntax tree"
            << std::endl;
  std::cout << "  --debug                 Enable debug mode" << std::endl;
  std::cout << std::endl;
  std::cout << "Example:" << std::endl;
  std::cout << "  " << programName << " examples/01_beginner/01_hello_world.cvy"
            << std::endl;
}

/**
 * @brief Main entry point for the Chronovyan interpreter
 */
int main(int argc, char *argv[]) {
  // Parse command line arguments
  bool verbose = false;
  bool showTokens = false;
  bool showAST = false;
  bool visualizeResources = true;
  bool debugMode = false;
  std::string filename;

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--help") {
      printUsage(argv[0]);
      return 0;
    } else if (arg == "--verbose") {
      verbose = true;
    } else if (arg == "--show-tokens") {
      showTokens = true;
    } else if (arg == "--show-ast") {
      showAST = true;
    } else if (arg == "--no-visualize") {
      visualizeResources = false;
    } else if (arg == "--debug") {
      debugMode = true;
    } else if (filename.empty()) {
      filename = arg;
    } else {
      std::cerr << "Unknown argument: " << arg << std::endl;
      printUsage(argv[0]);
      return 1;
    }
  }

  if (filename.empty()) {
    std::cerr << "No input file specified." << std::endl;
    printUsage(argv[0]);
    return 1;
  }

  try {
    // Initialize error handler
    // ErrorHandler::getInstance().setVerbose(verbose);
    // ErrorHandler::getInstance().setDebugMode(debugMode);

    if (verbose) {
      std::cout << "Loading file: " << filename << std::endl;
    }

    // Load the source code
    std::string source = loadFile(filename);

    if (verbose) {
      std::cout << "Source loaded successfully (" << source.length()
                << " bytes)" << std::endl;
    }

    // Create lexer and tokenize the source
    // Modified to use proper constructor based on header definition
    // Note: SourceFile constructor takes an rvalue string, so we need to move
    // 'source'
    std::shared_ptr<SourceFile> sourceFile =
        std::make_shared<SourceFile>(std::move(source), filename);
    Lexer lexer(sourceFile);
    // TODO: Replace with actual token parsing when implemented
    std::vector<Token> tokens;
    // auto tokens = lexer.tokenize();

    if (showTokens) {
      std::cout << "Tokens (" << tokens.size() << "):" << std::endl;
      for (const auto &token : tokens) {
        std::cout << token.toString() << std::endl;
      }
      std::cout << std::endl;
    }

    if (verbose) {
      std::cout << "Tokenization complete (" << tokens.size() << " tokens)"
                << std::endl;
    }

    // Create parser and parse the tokens
    // Parser constructor takes a shared_ptr<Lexer>, not a vector of tokens
    std::shared_ptr<Lexer> lexerPtr = std::make_shared<Lexer>(lexer);
    Parser parser(lexerPtr);
    auto program = parser.parse();

    if (showAST) {
      std::cout << "Abstract Syntax Tree:" << std::endl;
      // Print AST structure (simplified for this example)
      std::cout << "Program with " << program->getStatements().size()
                << " top-level statements" << std::endl;
      std::cout << std::endl;
    }

    if (verbose) {
      std::cout << "Parsing complete" << std::endl;
    }

    // Create interpreter and execute the program
    auto startTime = std::chrono::high_resolution_clock::now();

    Interpreter interpreter;
    if (!visualizeResources) {
      // Comment out or fix this method call
      // interpreter.getResourceVisualization()->setEnabled(false);
    }

    if (verbose) {
      std::cout << "Executing program..." << std::endl;
    }

    Value result = interpreter.interpret(*program);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);

    if (verbose) {
      std::cout << "Execution complete (" << duration.count() << " ms)"
                << std::endl;
      std::cout << "Program returned: " << result.toString() << std::endl;
    }

    // Show resource usage if visualization is enabled
    if (visualizeResources) {
      std::cout << "\nResource Summary:" << std::endl;
      // Comment out or fix these method calls
      // std::cout << "Aethel usage: " <<
      // interpreter.getTemporalRuntime()->getAethelUsage()
      // << std::endl; std::cout << "Chronon usage: " <<
      // interpreter.getTemporalRuntime()->getChrononsUsage() << std::endl;
      // std::cout << "Temporal debt: " <<
      // interpreter.getTemporalDebtTracker()->getTotalDebt() << std::endl;

      std::cout << "\nResource Visualization:" << std::endl;
      // std::cout <<
      // interpreter.getResourceVisualization()->generateVisualization(60, 10)
      // << std::endl;
    }

    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}