#include <fstream>
#include <iostream>
#include <sstream>

#include "compiler/Compiler.h"

namespace chronovyan {
namespace compiler {

// Concrete implementations for abstract classes
class ConcreteSemanticAnalyzer : public SemanticAnalyzer {
public:
  ConcreteSemanticAnalyzer(DiagnosticReporter &diagnostics)
      : SemanticAnalyzer(diagnostics) {}

  void
  visitParallelExecutionStmtNode(ParallelExecutionStmtNode *node) override {
    // Visit the thread count expression
    node->getThreadCount().accept(*this);

    // Visit the body
    if (node->getBody()) {
      node->getBody().accept(*this);
    }
  }

  void visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) override {
    // Visit the branch count expression
    node->getBranchCount().accept(*this);

    // Visit the branch body
    if (node->getBody()) {
      node->getBody().accept(*this);
    }
  }
};

class ConcreteChronFileGenerator : public ChronFileGenerator {
public:
  void
  visitParallelExecutionStmtNode(ParallelExecutionStmtNode *node) override {
    uint32_t parallelId = getNextNodeId();
    registerNodeId(node, parallelId);

    BytecodeInstruction parallelInstr(BytecodeNodeType::PARALLEL_EXECUTION,
                                      parallelId);
    // Add thread count as a field
    parallelInstr.addField("threadCount",
                           static_cast<int32_t>(1)); // Default to 1 thread

    addInstruction(parallelInstr);

    // Visit thread count expression
    node->getThreadCount().accept(*this);
    uint32_t threadCountId = getNodeId(&node->getThreadCount());
    parallelInstr.addChild(threadCountId);

    // Visit the body
    node->getBody().accept(*this);
    uint32_t bodyId = getNodeId(&node->getBody());
    parallelInstr.addChild(bodyId);
  }

  void visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) override {
    uint32_t branchId = getNextNodeId();
    registerNodeId(node, branchId);

    BytecodeInstruction branchInstr(BytecodeNodeType::BRANCH_TIMELINE,
                                    branchId);

    // Visit branch count expression
    node->getBranchCount().accept(*this);
    uint32_t branchCountId = getNodeId(&node->getBranchCount());
    branchInstr.addField("hasBranchCount", true);
    branchInstr.addChild(branchCountId);

    addInstruction(branchInstr);

    // Visit the branch body
    node->getBody().accept(*this);

    // Add body ID to the branch timeline node
    uint32_t bodyId = getNodeId(&node->getBody());
    branchInstr.addChild(bodyId);
  }
};

class ConcreteCodeGenerator : public CodeGenerator {
public:
  void
  visitParallelExecutionStmtNode(ParallelExecutionStmtNode *node) override {
    printLine("// Parallel execution");
    printLine("{");
    indent();

    // Add Chronovyan runtime parallel execution initialization
    printLine("// Create parallel execution context");
    printLine("ChronovyanParallelExecution* parallel_ctx = "
              "chronovyan_create_parallel_execution(runtime);");

    // Generate code for thread count
    node->getThreadCount().accept(*this);
    std::string threadCountCode = getNodeCode(&node->getThreadCount());
    printLine("// Set thread count");
    printLine("chronovyan_set_thread_count(parallel_ctx, " + threadCountCode +
              ");");

    // Generate code for the body
    printLine("");
    printLine("// Parallel body");
    node->getBody().accept(*this);

    // Execute all branches
    printLine("");
    printLine("// Execute parallel branches");
    printLine("chronovyan_execute_parallel(parallel_ctx);");
    printLine("chronovyan_cleanup_parallel(parallel_ctx);");

    dedent();
    printLine("}");
  }

  void visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) override {
    printLine("// Branch timeline");
    printLine("{");
    indent();

    // Generate code for branch count expression
    node->getBranchCount().accept(*this);
    std::string branchCountCode = getNodeCode(&node->getBranchCount());

    printLine("// Create timeline branch");
    printLine("ChronovyanTimeline* timeline_branch = "
              "chronovyan_branch_timeline(runtime, " +
              branchCountCode + ");");

    // Generate code for the branch body
    printLine("");
    printLine("// Branch body");
    node->getBody().accept(*this);

    // Merge the timeline back (for now we just do a simple merge)
    printLine("");
    printLine("// Merge timeline branch back");
    printLine("chronovyan_merge_timeline(runtime, timeline_branch);");

    dedent();
    printLine("}");
  }
};

Compiler::Compiler() {}

bool Compiler::compile(const std::filesystem::path &inputPath,
                       const std::filesystem::path &outputPath,
                       bool generateChron) {
  // Check if input file exists
  if (!std::filesystem::exists(inputPath)) {
    SourceLocation location;
    diagnostics.addError(location,
                         "Input file does not exist: " + inputPath.string());
    return false;
  }

  // Read the input file
  std::ifstream inFile(inputPath);
  if (!inFile.is_open()) {
    SourceLocation location;
    diagnostics.addError(location,
                         "Failed to open input file: " + inputPath.string());
    return false;
  }

  std::stringstream buffer;
  buffer << inFile.rdbuf();
  std::string sourceCode = buffer.str();

  // Determine output path if not provided
  std::filesystem::path actualOutputPath = outputPath;
  if (actualOutputPath.empty()) {
    actualOutputPath = inputPath;
    if (generateChron) {
      actualOutputPath.replace_extension(".chron");
    } else {
      actualOutputPath.replace_extension(".c");
    }
  }

  // Compile the source code
  return compileString(sourceCode, actualOutputPath, generateChron);
}

bool Compiler::compileString(const std::string &sourceCode,
                             const std::filesystem::path &outputPath,
                             bool generateChron) {
  // Reset diagnostics
  diagnostics.clearDiagnostics();

  if (generateChron) {
    // Generate .chron file
    // Lexical Analysis
    Lexer lexer(sourceCode, "input");

    // Parsing
    Parser parser(lexer);
    std::unique_ptr<AstNode> ast = parser.parse();

    if (!ast || parser.hasErrors()) {
      // Add parser errors to diagnostics
      const auto &parserDiagnostics =
          parser.getDiagnosticReporter().getDiagnostics();
      for (const auto &diagnostic : parserDiagnostics) {
        if (diagnostic.getSeverity() == DiagnosticSeverity::ERROR) {
          diagnostics.addError(diagnostic.getLocation(),
                               diagnostic.getMessage());
        }
      }
      return false;
    }

    // Generate .chron file
    return generateChronFile(ast, outputPath);
  } else {
    // Generate C code
    std::string generatedCode = compileInternal(sourceCode);

    // If there are errors, return false
    if (diagnostics.hasErrors()) {
      return false;
    }

    // Save the generated code to the output file
    return saveToFile(generatedCode, outputPath);
  }
}

const std::vector<DiagnosticMessage> &Compiler::getDiagnostics() const {
  return diagnostics.getDiagnostics();
}

bool Compiler::saveToFile(const std::string &generatedCode,
                          const std::filesystem::path &outputPath) {
  std::ofstream outFile(outputPath);
  if (!outFile.is_open()) {
    SourceLocation location;
    diagnostics.addError(location,
                         "Failed to open output file: " + outputPath.string());
    return false;
  }

  outFile << generatedCode;
  outFile.close();

  return true;
}

bool Compiler::generateChronFile(std::unique_ptr<AstNode> &ast,
                                 const std::filesystem::path &outputPath) {
  // Convert to ProgramNode
  ProgramNode *programNode = dynamic_cast<ProgramNode *>(ast.get());
  if (!programNode) {
    SourceLocation location;
    diagnostics.addError(location,
                         "Internal error: AST root is not a ProgramNode");
    return false;
  }

  // Semantic Analysis
  ConcreteSemanticAnalyzer semanticAnalyzer(diagnostics);
  semanticAnalyzer.analyze(programNode);

  if (diagnostics.hasErrors()) {
    return false;
  }

  // Generate .chron file
  ConcreteChronFileGenerator chronGenerator;
  bool success = chronGenerator.generate(programNode, outputPath);

  if (!success) {
    SourceLocation location;
    diagnostics.addError(location, "Failed to generate .chron file: " +
                                       outputPath.string());
    return false;
  }

  return true;
}

std::string Compiler::compileInternal(const std::string &sourceCode) {
  // 1. Lexical Analysis
  Lexer lexer(sourceCode, "input");

  // 2. Parsing
  Parser parser(lexer);
  std::unique_ptr<AstNode> ast = parser.parse();

  if (!ast || parser.hasErrors()) {
    // Add parser errors to diagnostics
    const auto &parserDiagnostics =
        parser.getDiagnosticReporter().getDiagnostics();
    for (const auto &diagnostic : parserDiagnostics) {
      if (diagnostic.getSeverity() == DiagnosticSeverity::ERROR) {
        diagnostics.addError(diagnostic.getLocation(), diagnostic.getMessage());
      }
    }
    return "";
  }

  // Convert to ProgramNode
  ProgramNode *programNode = dynamic_cast<ProgramNode *>(ast.get());
  if (!programNode) {
    SourceLocation location;
    diagnostics.addError(location,
                         "Internal error: AST root is not a ProgramNode");
    return "";
  }

  // 3. Semantic Analysis
  ConcreteSemanticAnalyzer semanticAnalyzer(diagnostics);
  semanticAnalyzer.analyze(programNode);

  if (diagnostics.hasErrors()) {
    return "";
  }

  // 4. Code Generation
  ConcreteCodeGenerator codeGenerator;
  std::string generatedCode = codeGenerator.generate(programNode);

  return generatedCode;
}

} // namespace compiler
} // namespace chronovyan