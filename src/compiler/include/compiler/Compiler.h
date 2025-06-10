#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "compiler/ChronFileGenerator.h"
#include "compiler/CodeGenerator.h"
#include "compiler/Diagnostic.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"
#include "compiler/SemanticAnalyzer.h"

namespace chronovyan {
namespace compiler {

/**
 * @class Compiler
 * @brief Main compiler driver class.
 *
 * This class coordinates the entire compilation process, from lexical analysis
 * to code generation, producing either C code or .chron bytecode files.
 */
class Compiler {
public:
  /**
   * @brief Default constructor.
   */
  Compiler();

  /**
   * @brief Compile a file.
   *
   * @param inputPath The path to the input file.
   * @param outputPath The path to the output file. If empty, uses <inputPath>.c
   * or <inputPath>.chron.
   * @param generateChron Whether to generate a .chron file instead of C code.
   * @return True if compilation was successful, false otherwise.
   */
  bool compile(const std::filesystem::path &inputPath,
               const std::filesystem::path &outputPath = {},
               bool generateChron = false);

  /**
   * @brief Compile a string.
   *
   * @param sourceCode The source code to compile.
   * @param outputPath The path to the output file.
   * @param generateChron Whether to generate a .chron file instead of C code.
   * @return True if compilation was successful, false otherwise.
   */
  bool compileString(const std::string &sourceCode,
                     const std::filesystem::path &outputPath,
                     bool generateChron = false);

  /**
   * @brief Get the diagnostics.
   *
   * @return The diagnostics.
   */
  const std::vector<DiagnosticMessage> &getDiagnostics() const;

private:
  /**
   * @brief Compile the source code.
   *
   * @param sourceCode The source code to compile.
   * @return The generated C code, or empty string on error.
   */
  std::string compileInternal(const std::string &sourceCode);

  /**
   * @brief Generate a .chron file from the AST.
   *
   * @param ast The AST to generate bytecode from.
   * @param outputPath The path to write the .chron file to.
   * @return True if generation was successful, false otherwise.
   */
  bool generateChronFile(std::unique_ptr<AstNode> &ast,
                         const std::filesystem::path &outputPath);

  /**
   * @brief Save the generated code to a file.
   *
   * @param generatedCode The generated code.
   * @param outputPath The path to write the file to.
   * @return True if the file was written successfully, false otherwise.
   */
  bool saveToFile(const std::string &generatedCode,
                  const std::filesystem::path &outputPath);

  // Diagnostic reporter
  DiagnosticReporter diagnostics;
};

} // namespace compiler
} // namespace chronovyan