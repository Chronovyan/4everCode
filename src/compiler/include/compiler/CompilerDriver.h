#pragma once

#include <memory>
#include <string>
#include <vector>

#include "compiler/BytecodeGenerator.h"
#include "compiler/Diagnostic.h"
#include "compiler/IRGenerator.h"
#include "compiler/OptimizationManager.h"

namespace chronovyan {
namespace compiler {

// Forward declarations
namespace proving_grounds {
class TemporalTestFramework;
}

/**
 * @class CompilerDriver
 * @brief Main driver class for the Chronovyan compiler.
 *
 * This class orchestrates the entire compilation process, from parsing source code
 * to generating bytecode. It manages the different compiler components and provides
 * a simple interface for compiling Chronovyan programs.
 */
class CompilerDriver {
public:
    /**
     * @brief Constructor
     * @param diagnostics Diagnostic reporter for reporting errors
     */
    explicit CompilerDriver(DiagnosticReporter& diagnostics);

    /**
     * @brief Set the optimization level.
     * @param level The optimization level to use.
     */
    void setOptimizationLevel(OptimizationLevel level);

    /**
     * @brief Get the current optimization level.
     * @return The current optimization level.
     */
    OptimizationLevel getOptimizationLevel() const;

    /**
     * @brief Register an optimization pass.
     * @param pass The optimization pass to register.
     * @param level The minimum optimization level at which to run this pass.
     */
    void registerOptimizationPass(std::unique_ptr<OptimizationPass> pass,
                                  OptimizationLevel level = OptimizationLevel::STANDARD);

    /**
     * @brief Register a temporal optimization pass.
     * @param pass The optimization pass to register.
     * @param level The minimum optimization level at which to run this pass.
     */
    void registerTemporalOptimizationPass(std::unique_ptr<OptimizationPass> pass,
                                          OptimizationLevel level = OptimizationLevel::TEMPORAL);

    /**
     * @brief Set the output file path for the compiled bytecode.
     * @param outputFile Path to the output file.
     */
    void setOutputFile(const std::string& outputFile);

    /**
     * @brief Get the output file path.
     * @return The output file path.
     */
    std::string getOutputFile() const;

    /**
     * @brief Enable or disable the Temporal Proving Grounds validation.
     * @param enable Whether to enable validation.
     */
    void enableProvingGrounds(bool enable);

    /**
     * @brief Check if Temporal Proving Grounds validation is enabled.
     * @return True if validation is enabled, false otherwise.
     */
    bool isProvingGroundsEnabled() const;

    /**
     * @brief Set the maximum allowed paradox severity.
     * @param severity The maximum allowed paradox severity (1-10).
     */
    void setMaxParadoxSeverity(int severity);

    /**
     * @brief Get the maximum allowed paradox severity.
     * @return The maximum allowed paradox severity.
     */
    int getMaxParadoxSeverity() const;

    /**
     * @brief Enable or disable Temporal Proving Grounds validation reporting.
     * @param enable Whether to enable reporting.
     * @param reportFile Path to save the report to (empty for console output only).
     * @param format The report format (text, html, json).
     */
    void enableValidationReporting(bool enable, const std::string& reportFile = "",
                                   const std::string& format = "text");

    /**
     * @brief Get the Temporal Proving Grounds framework.
     * @return The Temporal Proving Grounds framework.
     */
    std::shared_ptr<proving_grounds::TemporalTestFramework> getProvingGroundsFramework() const;

    /**
     * @brief Compile a Chronovyan source file.
     * @param sourceFile Path to the source file.
     * @return True if compilation was successful, false otherwise.
     */
    bool compile(const std::string& sourceFile);

    /**
     * @brief Compile Chronovyan source code from a string.
     * @param sourceCode The source code to compile.
     * @param fileName Optional name for the source (for error reporting).
     * @return True if compilation was successful, false otherwise.
     */
    bool compileString(const std::string& sourceCode, const std::string& fileName = "<string>");

private:
    DiagnosticReporter& diagnostics;          ///< Diagnostic reporter
    OptimizationManager optimizationManager;  ///< Optimization manager
    std::string outputFile;                   ///< Output file path
    bool dumpIR;                              ///< Whether to dump IR to a file
    bool dumpAST;                             ///< Whether to dump AST to a file

    // Temporal Proving Grounds related fields
    bool provingGroundsEnabled;          ///< Whether Temporal Proving Grounds validation is enabled
    int maxParadoxSeverity;              ///< Maximum allowed paradox severity (1-10)
    bool validationReportingEnabled;     ///< Whether to generate validation reports
    std::string validationReportFile;    ///< File to save validation reports to
    std::string validationReportFormat;  ///< Format for validation reports (text, html, json)
    std::shared_ptr<proving_grounds::TemporalTestFramework>
        provingGroundsFramework;  ///< Temporal Proving Grounds framework

    /**
     * @brief Parse a Chronovyan source file.
     * @param sourceFile Path to the source file.
     * @return AST root node, or nullptr if parsing failed.
     */
    std::unique_ptr<AstNode> parseFile(const std::string& sourceFile);

    /**
     * @brief Parse Chronovyan source code from a string.
     * @param sourceCode The source code to parse.
     * @param fileName Optional name for the source (for error reporting).
     * @return AST root node, or nullptr if parsing failed.
     */
    std::unique_ptr<AstNode> parseString(const std::string& sourceCode,
                                         const std::string& fileName);

    /**
     * @brief Perform semantic analysis on the AST.
     * @param ast The AST to analyze.
     * @return True if analysis was successful, false otherwise.
     */
    bool performSemanticAnalysis(AstNode* ast);

    /**
     * @brief Optimize the AST.
     * @param ast The AST to optimize.
     * @return True if any optimizations were applied, false otherwise.
     */
    bool optimizeAST(AstNode* ast);

    /**
     * @brief Generate IR from the AST.
     * @param ast The AST to generate IR from.
     * @return The generated IR program.
     */
    IRProgram generateIR(AstNode* ast);

    /**
     * @brief Generate bytecode from the IR.
     * @param irProgram The IR program to generate bytecode from.
     * @return True if bytecode generation was successful, false otherwise.
     */
    bool generateBytecode(const IRProgram& irProgram);

    /**
     * @brief Write bytecode to a file
     * @param bytecodeModule The bytecode module to write
     * @param outputFile The file to write to
     * @return True if successful, false otherwise
     */
    bool writeBytecodeToFile(const chronovyan::compiler::bytecode::BytecodeModule& bytecodeModule,
                             const std::string& outputFile);

    /**
     * @brief Validate bytecode using the proving grounds
     * @param bytecodeModule The bytecode module to validate
     * @param sourceName The name of the source file (for error reporting)
     * @return True if validation passed, false otherwise
     */
    bool validateBytecodeWithProvingGrounds(
        const chronovyan::compiler::bytecode::BytecodeModule& bytecodeModule,
        const std::string& sourceName);
};

}  // namespace compiler
}  // namespace chronovyan