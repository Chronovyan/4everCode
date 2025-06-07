#include <fstream>
#include <iostream>
#include <sstream>

#include "compiler/AstPrinter.h"
#include "compiler/BytecodeGenerator.h"
#include "compiler/CompilerDriver.h"
#include "compiler/DiagnosticReporter.h"
#include "compiler/IRGenerator.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"
#include "compiler/SemanticAnalyzer.h"

namespace chronovyan {
namespace compiler {

CompilerDriver::CompilerDriver(DiagnosticReporter& diagnostics)
    : diagnostics(diagnostics)
    , dumpIR(false)
    , dumpAST(false)
    , provingGroundsEnabled(false)
    , maxParadoxSeverity(5)
    , validationReportingEnabled(false)
    , validationReportFormat("text") {
    // Initialize the Temporal Proving Grounds framework
    // TODO: Uncomment when the TemporalTestFramework class is implemented
    // provingGroundsFramework =
    // std::make_shared<proving_grounds::TemporalTestFramework>(diagnostics);
}

void CompilerDriver::setOptimizationLevel(OptimizationLevel level) {
    optimizationManager.setOptimizationLevel(level);
}

OptimizationLevel CompilerDriver::getOptimizationLevel() const {
    return optimizationManager.getOptimizationLevel();
}

void CompilerDriver::registerOptimizationPass(std::unique_ptr<OptimizationPass> pass,
                                              OptimizationLevel level) {
    optimizationManager.registerPass(std::move(pass), level);
}

void CompilerDriver::registerTemporalOptimizationPass(std::unique_ptr<OptimizationPass> pass,
                                                      OptimizationLevel level) {
    optimizationManager.registerTemporalPass(std::move(pass), level);
}

void CompilerDriver::setOutputFile(const std::string& file) { outputFile = file; }

std::string CompilerDriver::getOutputFile() const { return outputFile; }

void CompilerDriver::enableProvingGrounds(bool enable) { provingGroundsEnabled = enable; }

bool CompilerDriver::isProvingGroundsEnabled() const { return provingGroundsEnabled; }

void CompilerDriver::setMaxParadoxSeverity(int severity) {
    // Ensure the severity is within valid range (1-10)
    if (severity < 1 || severity > 10) {
        diagnostics.addWarning(SourceLocation(),
                               "Invalid paradox severity: " + std::to_string(severity) +
                                   ". Using default value (5).");
        maxParadoxSeverity = 5;
    } else {
        maxParadoxSeverity = severity;
    }

    // Update the paradox detector if it exists
    // TODO: Uncomment when the TemporalTestFramework class is implemented
    /*
    if (provingGroundsFramework && provingGroundsFramework->getParadoxDetector()) {
        provingGroundsFramework->getParadoxDetector()->setSeverityThreshold(maxParadoxSeverity);
    }
    */
}

int CompilerDriver::getMaxParadoxSeverity() const { return maxParadoxSeverity; }

void CompilerDriver::enableValidationReporting(bool enable, const std::string& reportFile,
                                               const std::string& format) {
    validationReportingEnabled = enable;
    validationReportFile = reportFile;

    // Validate report format
    if (format != "text" && format != "html" && format != "json") {
        diagnostics.addWarning(SourceLocation(), "Invalid report format: " + format +
                                                     ". Using default format (text).");
        validationReportFormat = "text";
    } else {
        validationReportFormat = format;
    }
}

std::shared_ptr<proving_grounds::TemporalTestFramework> CompilerDriver::getProvingGroundsFramework()
    const {
    // TODO: Uncomment when the TemporalTestFramework class is implemented
    // return provingGroundsFramework;
    return nullptr;
}

bool CompilerDriver::compile(const std::string& sourceFile) {
    // Parse the source file
    std::unique_ptr<AstNode> ast = parseFile(sourceFile);
    if (!ast) {
        return false;
    }

    // Dump AST if requested
    if (dumpAST) {
        std::string astDumpFile = sourceFile + ".ast";
        std::ofstream astFile(astDumpFile);
        if (astFile.is_open()) {
            // Create an AST printer with the output file stream
            AstPrinter printer(astFile);
            // Use accept() to visit the AST and print it
            ast->accept(printer);
            astFile.close();
        }
    }

    // Perform semantic analysis
    if (!performSemanticAnalysis(ast.get())) {
        return false;
    }

    // Optimize the AST
    optimizeAST(ast.get());

    // Generate IR
    IRProgram irProgram = generateIR(ast.get());

    // Dump IR if requested
    if (dumpIR) {
        std::string irDump = irProgram.toString();
        std::string irDumpFile = sourceFile + ".ir";
        std::ofstream irFile(irDumpFile);
        if (irFile.is_open()) {
            irFile << irDump;
            irFile.close();
        }
    }

    // Generate bytecode
    if (!generateBytecode(irProgram)) {
        return false;
    }

    return true;
}

bool CompilerDriver::compileString(const std::string& sourceCode, const std::string& fileName) {
    // Parse the source code
    std::unique_ptr<AstNode> ast = parseString(sourceCode, fileName);
    if (!ast) {
        return false;
    }

    // Dump AST if requested
    if (dumpAST) {
        std::string astDumpFile = fileName + ".ast";
        std::ofstream astFile(astDumpFile);
        if (astFile.is_open()) {
            // Create an AST printer with the output file stream
            AstPrinter printer(astFile);
            // Use accept() to visit the AST and print it
            ast->accept(printer);
            astFile.close();
        }
    }

    // Perform semantic analysis
    if (!performSemanticAnalysis(ast.get())) {
        return false;
    }

    // Optimize the AST
    optimizeAST(ast.get());

    // Generate IR
    IRProgram irProgram = generateIR(ast.get());

    // Dump IR if requested
    if (dumpIR) {
        std::string irDump = irProgram.toString();
        std::string irDumpFile = fileName + ".ir";
        std::ofstream irFile(irDumpFile);
        if (irFile.is_open()) {
            irFile << irDump;
            irFile.close();
        }
    }

    // Generate bytecode
    if (!generateBytecode(irProgram)) {
        return false;
    }

    return true;
}

std::unique_ptr<AstNode> CompilerDriver::parseFile(const std::string& sourceFile) {
    // Read the source file
    std::ifstream file(sourceFile);
    if (!file.is_open()) {
        diagnostics.addError(SourceLocation(), "Failed to open source file: " + sourceFile);
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // Parse the source code
    return parseString(buffer.str(), sourceFile);
}

std::unique_ptr<AstNode> CompilerDriver::parseString(const std::string& sourceCode,
                                                     const std::string& fileName) {
    // Create a lexer to tokenize the source code
    Lexer lexer(sourceCode, fileName);

    // Create a parser with the lexer
    Parser parser(lexer);
    std::unique_ptr<AstNode> ast = parser.parse();

    // Check for syntax errors
    if (parser.hasErrors() || !ast) {
        // Parser already reports errors to the diagnostics
        return nullptr;
    }

    return std::move(ast);
}

bool CompilerDriver::performSemanticAnalysis(AstNode* ast) {
    // Create a semantic analyzer
    SemanticAnalyzer analyzer(diagnostics);

    // Analyze the AST
    // Cast to ProgramNode* if needed
    if (auto programNode = dynamic_cast<ProgramNode*>(ast)) {
        analyzer.analyze(programNode);
    } else {
        diagnostics.addError(SourceLocation(), "AST root is not a ProgramNode");
        return false;
    }

    // Check for semantic errors - analyzer reports errors to diagnostics
    if (diagnostics.hasErrors()) {
        return false;
    }

    return true;
}

bool CompilerDriver::optimizeAST(AstNode* ast) {
    // Run optimization passes
    return optimizationManager.optimize(ast, diagnostics);
}

IRProgram CompilerDriver::generateIR(AstNode* ast) {
    // Create an IR generator
    IRGenerator irGenerator(diagnostics);

    // Generate IR
    return irGenerator.generate(ast);
}

bool CompilerDriver::generateBytecode(const IRProgram& irProgram) {
    // Create a bytecode generator
    BytecodeGenerator bytecodeGenerator(diagnostics);

    // Generate bytecode
    bytecode::BytecodeModule bytecodeModule = bytecodeGenerator.generateBytecode(irProgram);

    // Validate bytecode with Temporal Proving Grounds if enabled
    if (provingGroundsEnabled) {
        std::string sourceName = !outputFile.empty() ? outputFile : "unnamed_source";
        // TODO: Uncomment when the TemporalTestFramework class is implemented
        /*
        if (!validateBytecodeWithProvingGrounds(bytecodeModule, sourceName)) {
            // Validation failed, but we still continue with compilation
            // The errors are already reported in the diagnostics
            diagnostics.addWarning(SourceLocation(),
                "Temporal validation issues detected. See validation report for details.");
        }
        */
        // Add warning that validation is not available
        diagnostics.addWarning(SourceLocation(),
                               "Temporal validation is not available in this build.");
    }

    // Write bytecode to file
    if (!outputFile.empty()) {
        return writeBytecodeToFile(bytecodeModule, outputFile);
    }

    return true;
}

bool CompilerDriver::writeBytecodeToFile(const bytecode::BytecodeModule& bytecodeModule,
                                         const std::string& outputFile) {
    // Open the output file
    std::ofstream file(outputFile, std::ios::binary);
    if (!file.is_open()) {
        diagnostics.addError(SourceLocation(), "Failed to open output file: " + outputFile);
        return false;
    }

    // Serialize the bytecode module to a binary format
    std::vector<uint8_t> bytecode = bytecodeModule.serialize();

    // Write the bytecode to the file
    file.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());

    // Check for errors
    if (file.fail()) {
        diagnostics.addError(SourceLocation(), "Failed to write to output file: " + outputFile);
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool CompilerDriver::validateBytecodeWithProvingGrounds(
    const bytecode::BytecodeModule& bytecodeModule, const std::string& sourceName) {
    // TODO: Implement when the TemporalTestFramework class is available
    /*
    if (!provingGroundsFramework) {
        diagnostics.addError(SourceLocation(), "Temporal Proving Grounds framework not
    initialized"); return false;
    }

    // Create a test case from the bytecode module
    auto testCase = provingGroundsFramework->createTestCaseFromBytecode(bytecodeModule,
    "Validation_" + sourceName);

    // Configure the test case
    testCase->setMaxParadoxSeverity(maxParadoxSeverity);
    testCase->enableBenchmarking(true);

    // Register the test case
    provingGroundsFramework->registerTestCase(testCase);

    // Run the test
    auto results = provingGroundsFramework->runAllTests();

    // Generate and save reports if enabled
    if (validationReportingEnabled) {
        std::string report = provingGroundsFramework->generateReport(results,
    validationReportFormat);

        if (!validationReportFile.empty()) {
            if (!provingGroundsFramework->saveReport(report, validationReportFile)) {
                diagnostics.addWarning(SourceLocation(),
                    "Failed to save validation report to: " + validationReportFile);
            }
        } else {
            // Print report to console
            std::cout << "\nTemporal Validation Report:\n" << report << std::endl;
        }
    }

    // Check if all tests passed
    bool allPassed = true;
    for (const auto& result : results) {
        if (!result.isPassed()) {
            allPassed = false;

            // Report paradoxes as warnings
            for (const auto& paradox : result.getParadoxes()) {
                diagnostics.addWarning(paradox.location,
                    "Temporal paradox detected: " + paradox.description +
                    " (Severity: " + std::to_string(paradox.severity) + ")");
            }
        }
    }

    return allPassed;
    */

    // Stub implementation
    return true;
}

}  // namespace compiler
}  // namespace chronovyan