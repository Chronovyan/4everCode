#include "compiler/CompilerDriver.h"
#include "compiler/DiagnosticReporter.h"
// Removing bytecode includes since we don't use them directly
// #include "compiler/bytecode/Bytecode.h"
// #include "compiler/bytecode/BytecodeModule.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void printUsage() {
  std::cout << "Chronovyan Compiler Usage:" << std::endl;
  std::cout << "  chronovyan_compiler [options] <input_file> [-o <output_file>]"
            << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help                Display this help message"
            << std::endl;
  std::cout << "  -o, --output <file>       Specify output file" << std::endl;
  std::cout << "  -O<level>                 Set optimization level (0-3)"
            << std::endl;
  std::cout << "  --dump-ast                Dump AST to file" << std::endl;
  std::cout << "  --dump-ir                 Dump IR to file" << std::endl;
  std::cout << "  -v, --verbose             Enable verbose output" << std::endl;
  std::cout << std::endl;
  std::cout << "Temporal Proving Grounds Options:" << std::endl;
  std::cout << "  --validate                Enable Temporal Proving Grounds "
               "validation"
            << std::endl;
  std::cout << "  --paradox-severity <1-10> Set maximum allowed paradox "
               "severity (default: 5)"
            << std::endl;
  std::cout << "  --validation-report <file> Generate validation report and "
               "save to file"
            << std::endl;
  std::cout << "  --report-format <format>  Set report format (text, html, "
               "json, default: text)"
            << std::endl;
  std::cout << std::endl;
}

// Simple function to read a file into a string
std::string readFile(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

// Simple function to write a string to a file
void writeFile(const std::string &filePath, const std::string &content) {
  std::ofstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for writing: " + filePath);
  }

  file << content;

  if (file.fail()) {
    throw std::runtime_error("Failed to write to file: " + filePath);
  }
}

// Simple function to check if a string ends with a specific suffix
bool endsWith(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
         0;
}

// Simple placeholder implementation of bytecode generation
// This function will read the input file, count some metrics, and write a
// placeholder output
bool compilePlaceholder(const std::string &inputFile,
                        const std::string &outputFile, bool verbose) {
  try {
    // Open the input file
    std::ifstream input(inputFile);
    if (!input.is_open()) {
      std::cerr << "Error: Could not open input file: " << inputFile
                << std::endl;
      return false;
    }

    // Read the entire file
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string content = buffer.str();
    input.close();

    if (verbose) {
      std::cout << "Read input file: " << inputFile << std::endl;
      std::cout << "File size: " << content.size() << " bytes" << std::endl;

      // Debug information for quantum_weaver.cvy
      if (inputFile.find("quantum_weaver.cvy") != std::string::npos) {
        std::cout << "Special debug for quantum_weaver.cvy:" << std::endl;

        // Print first 20 characters with their ASCII values
        std::cout << "First 20 characters:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(20), content.size()); ++i) {
          std::cout << "  [" << i << "] '" << content[i]
                    << "' (ASCII: " << static_cast<int>(content[i]) << ")"
                    << std::endl;
        }

        // Look for potential problematic characters
        size_t problematicCount = 0;
        for (size_t i = 0; i < content.size(); ++i) {
          unsigned char c = static_cast<unsigned char>(content[i]);
          if (c > 127 || c < 32) {
            if (c != '\n' && c != '\r' && c != '\t') {
              std::cout << "  Non-ASCII or control char at pos " << i << ": "
                        << static_cast<int>(c) << std::endl;
              problematicCount++;
              if (problematicCount > 10) {
                std::cout << "  (More non-ASCII characters omitted...)"
                          << std::endl;
                break;
              }
            }
          }
        }
      }
    }

    // Filter out non-ASCII characters
    std::string filteredContent;
    filteredContent.reserve(content.size());
    for (unsigned char c : content) {
      // Keep only printable ASCII characters and basic control characters
      // (newline, tab, etc.)
      if (c <= 127) {
        filteredContent.push_back(c);
      }
    }

    // Count lines and simple metrics
    int lines = 0;
    int statements = 0;
    bool inComment = false;
    bool inString = false;
    bool afterSemicolon = true;

    for (size_t i = 0; i < filteredContent.size(); ++i) {
      char c = filteredContent[i];

      try {
        if (c == '\n') {
          lines++;
          if (!inComment && !inString) {
            afterSemicolon = true;
          }
        } else if (c == '/' && i + 1 < filteredContent.size() &&
                   filteredContent[i + 1] == '*' && !inString) {
          inComment = true;
          i++; // Skip the '*'
        } else if (c == '*' && i + 1 < filteredContent.size() &&
                   filteredContent[i + 1] == '/' && inComment) {
          inComment = false;
          i++; // Skip the '/'
        } else if (c == '"' && !inComment) {
          inString = !inString;
        } else if (c == ';' && !inComment && !inString) {
          statements++;
          afterSemicolon = true;
        } else if (!std::isspace(c) && afterSemicolon && !inComment &&
                   !inString) {
          afterSemicolon = false;
        }
      } catch (const std::exception &e) {
        std::cerr << "Error processing character at position " << i << " ('"
                  << c << "'): " << e.what() << std::endl;
        // Continue processing despite errors
      }
    }

    // Ensure we count the last line if it doesn't end with a newline
    if (!filteredContent.empty() && filteredContent.back() != '\n') {
      lines++;
    }

    // Write output file with placeholder bytecode
    std::ofstream output(outputFile);
    if (!output.is_open()) {
      std::cerr << "Error: Could not open output file: " << outputFile
                << std::endl;
      return false;
    }

    output << "// Chronovyan Compiled Output\n";
    output << "// Input: " << inputFile << "\n";
    output << "// Metrics:\n";
    output << "//   Lines: " << lines << "\n";
    output << "//   Characters: " << content.size() << "\n";
    output << "//   Statements: " << statements << "\n";
    output << "// Note: The actual compiler would generate bytecode here.\n";

    output.close();

    if (verbose) {
      std::cout << "Wrote output file: " << outputFile << std::endl;
      std::cout << "Metrics: " << lines << " lines, " << content.size()
                << " characters, " << statements << " statements" << std::endl;
    }

    return true;
  } catch (const std::exception &e) {
    std::cerr << "Error during compilation: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error during compilation" << std::endl;
    return false;
  }
}

int main(int argc, char *argv[]) {
  // Check if no arguments were provided
  if (argc < 2) {
    printUsage();
    return 1;
  }

  // Parse command-line arguments
  std::string inputFile;
  std::string outputFile;
  int optimizationLevel = 1; // Default optimization level
  bool dumpAST = false;
  bool dumpIR = false;
  bool verbose = false;

  // Temporal Proving Grounds options
  bool enableValidation = false;
  int paradoxSeverity = 5; // Default severity threshold
  bool generateReport = false;
  std::string reportFile;
  std::string reportFormat = "text"; // Default format

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      printUsage();
      return 0;
    } else if (arg == "-o" || arg == "--output") {
      if (i + 1 < argc) {
        outputFile = argv[++i];
      } else {
        std::cerr << "Error: Missing output file" << std::endl;
        return 1;
      }
    } else if (arg.substr(0, 2) == "-O" && arg.size() > 2) {
      try {
        optimizationLevel = std::stoi(arg.substr(2));
        if (optimizationLevel < 0 || optimizationLevel > 3) {
          std::cerr
              << "Error: Invalid optimization level. Must be between 0 and 3."
              << std::endl;
          return 1;
        }
      } catch (const std::exception &) {
        std::cerr << "Error: Invalid optimization level" << std::endl;
        return 1;
      }
    } else if (arg == "--dump-ast") {
      dumpAST = true;
    } else if (arg == "--dump-ir") {
      dumpIR = true;
    } else if (arg == "-v" || arg == "--verbose") {
      verbose = true;
    } else if (arg == "--validate") {
      enableValidation = true;
    } else if (arg == "--paradox-severity") {
      if (i + 1 < argc) {
        try {
          paradoxSeverity = std::stoi(argv[++i]);
          if (paradoxSeverity < 1 || paradoxSeverity > 10) {
            std::cerr
                << "Error: Invalid paradox severity. Must be between 1 and 10."
                << std::endl;
            return 1;
          }
        } catch (const std::exception &) {
          std::cerr << "Error: Invalid paradox severity" << std::endl;
          return 1;
        }
      } else {
        std::cerr << "Error: Missing paradox severity value" << std::endl;
        return 1;
      }
    } else if (arg == "--validation-report") {
      if (i + 1 < argc) {
        generateReport = true;
        reportFile = argv[++i];
      } else {
        std::cerr << "Error: Missing validation report file" << std::endl;
        return 1;
      }
    } else if (arg == "--report-format") {
      if (i + 1 < argc) {
        reportFormat = argv[++i];
        if (reportFormat != "text" && reportFormat != "html" &&
            reportFormat != "json") {
          std::cerr << "Error: Invalid report format. Must be 'text', 'html', "
                       "or 'json'."
                    << std::endl;
          return 1;
        }
      } else {
        std::cerr << "Error: Missing report format" << std::endl;
        return 1;
      }
    } else if (arg[0] == '-') {
      std::cerr << "Error: Unknown option '" << arg << "'" << std::endl;
      return 1;
    } else if (inputFile.empty()) {
      // The first non-option argument is the input file
      inputFile = arg;
    } else {
      std::cerr << "Error: Multiple input files specified" << std::endl;
      return 1;
    }
  }

  // Ensure input file was specified
  if (inputFile.empty()) {
    std::cerr << "Error: No input file specified" << std::endl;
    return 1;
  }

  // Check if the input file has a .cvy extension
  if (!endsWith(inputFile, ".cvy")) {
    std::cerr << "Warning: Input file does not have a .cvy extension"
              << std::endl;
  }

  // If no output file is specified, use the input file name with .chron
  // extension
  if (outputFile.empty()) {
    size_t dotPos = inputFile.find_last_of('.');
    if (dotPos != std::string::npos) {
      outputFile = inputFile.substr(0, dotPos) + ".chron";
    } else {
      outputFile = inputFile + ".chron";
    }
  }

  // For now, use the placeholder implementation since the full compiler isn't
  // ready
  if (verbose) {
    std::cout << "Compiling " << inputFile << " to " << outputFile << "..."
              << std::endl;
  }

  bool success = compilePlaceholder(inputFile, outputFile, verbose);

  // Check for errors
  if (!success) {
    std::cerr << "Compilation failed!" << std::endl;
    return 1;
  }

  std::cout << "Compilation successful: " << inputFile << " -> " << outputFile
            << std::endl;

  return 0;
}
