#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "compiler/AstNode.h"
#include "compiler/CodeGenerator.h"
#include "compiler/Diagnostic.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"
#include "compiler/SemanticAnalyzer.h"
#include "compiler/Token.h"

using namespace chronovyan::compiler;

// Function to detect and convert encoding if necessary
std::string readAndConvertFile(const std::string& filename) {
    // Open the file in binary mode to detect the encoding
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }

    // Read the entire file into a buffer
    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
    file.close();

    // Check for UTF-16LE BOM (FF FE)
    if (buffer.size() >= 2 && static_cast<unsigned char>(buffer[0]) == 0xFF &&
        static_cast<unsigned char>(buffer[1]) == 0xFE) {
        std::cout << "Detected UTF-16LE encoding with BOM\n";

        // Convert UTF-16LE to UTF-8
        std::string utf8;
        // Skip the BOM
        for (size_t i = 2; i < buffer.size() - 1; i += 2) {
            // Basic conversion assuming ASCII/Latin-1 range
            // This is a simplistic conversion - a proper implementation would handle all Unicode
            unsigned short codePoint = static_cast<unsigned char>(buffer[i]) |
                                       (static_cast<unsigned char>(buffer[i + 1]) << 8);

            if (codePoint < 128) {
                utf8 += static_cast<char>(codePoint);
            } else if (codePoint < 0x800) {
                utf8 += static_cast<char>(0xC0 | (codePoint >> 6));
                utf8 += static_cast<char>(0x80 | (codePoint & 0x3F));
            } else {
                utf8 += static_cast<char>(0xE0 | (codePoint >> 12));
                utf8 += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
                utf8 += static_cast<char>(0x80 | (codePoint & 0x3F));
            }
        }
        return utf8;
    }
    // Check for UTF-8 BOM (EF BB BF)
    else if (buffer.size() >= 3 && static_cast<unsigned char>(buffer[0]) == 0xEF &&
             static_cast<unsigned char>(buffer[1]) == 0xBB &&
             static_cast<unsigned char>(buffer[2]) == 0xBF) {
        std::cout << "Detected UTF-8 encoding with BOM\n";
        // Skip the BOM and return the rest of the file
        return std::string(buffer.begin() + 3, buffer.end());
    } else {
        // Assume UTF-8 without BOM or ASCII
        std::cout << "Assuming UTF-8 encoding without BOM\n";
        return std::string(buffer.begin(), buffer.end());
    }
}

// Utility function to print a hex dump of the data
void printHexDump(const std::string& data, size_t max_bytes = 32) {
    size_t bytes_to_print = std::min(data.size(), max_bytes);
    std::cout << "Hex dump of first " << bytes_to_print << " bytes:\n";

    for (size_t i = 0; i < bytes_to_print; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(static_cast<unsigned char>(data[i])) << " ";
        if ((i + 1) % 16 == 0)
            std::cout << std::endl;
    }
    std::cout << std::dec << std::endl;
}

// Simple implementation of a semantic analyzer that handles basic Chronovyan constructs
class SimpleSemanticAnalyzer : public SemanticAnalyzer {
public:
    SimpleSemanticAnalyzer(DiagnosticReporter& diagnostics)
        : SemanticAnalyzer(diagnostics), diagnosticReporter(diagnostics) {}

    // Analyze the AST starting from the program node
    bool analyze(AstNode* node) {
        if (node) {
            node->accept(*this);
            return !diagnosticReporter.hasErrors();
        }
        return false;
    }

    // Implementation for temporal constructs
    void visitParallelExecutionStmtNode(ParallelExecutionStmtNode* node) override {
        if (node) {
            node->getThreadCount().accept(*this);
            node->getBody().accept(*this);
        }
    }

    void visitBranchTimelineStmtNode(BranchTimelineStmtNode* node) override {
        if (node) {
            node->getBranchCount().accept(*this);
            node->getBody().accept(*this);
        }
    }

    // Override other necessary visit methods for basic Chronovyan programs
    void visitProgramNode(ProgramNode* node) override {
        if (node) {
            for (const auto& decl : node->getDeclarations()) {
                decl->accept(*this);
            }
        }
    }

    void visitExprStmtNode(ExprStmtNode* node) override {
        if (node) {
            node->getExpression().accept(*this);
        }
    }

    void visitCallExprNode(CallExprNode* node) override {
        if (node) {
            // Check if this is a print function call
            auto& callee = node->getCallee();
            VariableExprNode* varExpr = dynamic_cast<VariableExprNode*>(&callee);

            if (varExpr && varExpr->getName().getLexeme() == "print") {
                // Print function is built-in and valid
                for (const auto& arg : node->getArguments()) {
                    arg->accept(*this);
                }
            } else {
                // For other function calls, check if the function exists
                callee.accept(*this);
                for (const auto& arg : node->getArguments()) {
                    arg->accept(*this);
                }
            }
        }
    }

    void visitLiteralExprNode(LiteralExprNode* node) override {
        // String literals are valid
    }

    void visitVariableExprNode(VariableExprNode* node) override {
        // Check if variable is in scope
    }

    DiagnosticReporter& getDiagnosticReporter() { return diagnosticReporter; }

private:
    DiagnosticReporter& diagnosticReporter;
};

// Simple implementation of a code generator that produces C code for basic Chronovyan programs
class SimpleCodeGenerator : public CodeGenerator {
public:
    SimpleCodeGenerator() : indentLevel(0) {}

    std::string generate(ProgramNode* programNode) {
        if (!programNode) {
            return "";
        }

        // Clear state
        output.str("");
        output.clear();
        indentLevel = 0;

        // Add standard C headers
        output << "#include <stdio.h>\n";
        output << "#include <stdlib.h>\n";
        output << "#include <stdbool.h>\n";
        output << "#include <string.h>\n\n";

        // Add a simple print function implementation
        output << "// Simple implementation of Chronovyan print function\n";
        output << "void chronovyan_print(const char* message) {\n";
        output << "    printf(\"%s\\n\", message);\n";
        output << "}\n\n";

        // Generate main function
        output << "int main() {\n";
        indentLevel++;

        // Generate code for each declaration in the program
        for (const auto& decl : programNode->getDeclarations()) {
            decl->accept(*this);
        }

        // Add return statement and close main function
        printIndent();
        output << "return 0;\n";
        indentLevel--;
        output << "}\n";

        return output.str();
    }

    // Visit methods for each node type
    void visitProgramNode(ProgramNode* node) override {
        for (const auto& decl : node->getDeclarations()) {
            decl->accept(*this);
        }
    }

    void visitExprStmtNode(ExprStmtNode* node) override {
        node->getExpression().accept(*this);
        printLine(getNodeCode(&node->getExpression()) + ";");
    }

    void visitCallExprNode(CallExprNode* node) override {
        auto& callee = node->getCallee();
        VariableExprNode* varExpr = dynamic_cast<VariableExprNode*>(&callee);

        if (varExpr && varExpr->getName().getLexeme() == "print") {
            // Handle print function specially
            if (node->getArguments().size() == 1) {
                auto& arg = node->getArguments()[0];
                arg->accept(*this);
                setNodeCode(node, "chronovyan_print(" + getNodeCode(arg.get()) + ")");
            } else {
                setNodeCode(node, "chronovyan_print(\"Error: print requires 1 argument\")");
            }
        } else {
            // Generate code for other function calls
            callee.accept(*this);
            std::string calleeCode = getNodeCode(&callee);

            std::string argsCode;
            bool first = true;
            for (const auto& arg : node->getArguments()) {
                if (!first) {
                    argsCode += ", ";
                }
                first = false;

                arg->accept(*this);
                argsCode += getNodeCode(arg.get());
            }

            setNodeCode(node, calleeCode + "(" + argsCode + ")");
        }
    }

    void visitLiteralExprNode(LiteralExprNode* node) override {
        const auto& token = node->getToken();

        if (token.getType() == TokenType::STRING) {
            // String literal
            setNodeCode(node, "\"" + std::get<std::string>(node->getValue()) + "\"");
        } else if (token.getType() == TokenType::INTEGER) {
            // Integer literal
            setNodeCode(node, std::to_string(std::get<int>(node->getValue())));
        } else if (token.getType() == TokenType::FLOAT) {
            // Float literal
            setNodeCode(node, std::to_string(std::get<double>(node->getValue())));
        } else if (token.getType() == TokenType::TRUE) {
            // Boolean true
            setNodeCode(node, "true");
        } else if (token.getType() == TokenType::FALSE) {
            // Boolean false
            setNodeCode(node, "false");
        } else {
            // Unknown literal
            setNodeCode(node, "/* unknown literal */");
        }
    }

    void visitVariableExprNode(VariableExprNode* node) override {
        setNodeCode(node, node->getName().getLexeme());
    }

private:
    std::ostringstream output;
    int indentLevel;
    std::unordered_map<const ExprNode*, std::string> exprCode;

    void printIndent() {
        for (int i = 0; i < indentLevel; i++) {
            output << "    ";
        }
    }

    void printLine(const std::string& line) {
        printIndent();
        output << line << "\n";
    }

    std::string getNodeCode(const ExprNode* node) {
        auto it = exprCode.find(node);
        if (it != exprCode.end()) {
            return it->second;
        }
        return "";
    }

    void setNodeCode(const ExprNode* node, const std::string& code) { exprCode[node] = code; }
};

// Function to save output to a file
bool saveToFile(const std::string& content, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file " << filename << std::endl;
        return false;
    }

    outFile << content;
    outFile.close();
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file> [output-file]" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : (filename + ".c");

    std::cout << "Attempting to compile: " << filename << std::endl;
    std::cout << "Output will be written to: " << outputFile << std::endl;

    // Read and potentially convert the file based on its encoding
    std::string sourceCode = readAndConvertFile(filename);
    if (sourceCode.empty()) {
        std::cerr << "Failed to read or convert file" << std::endl;
        return 1;
    }

    std::cout << "File size: " << sourceCode.size() << " bytes" << std::endl;
    printHexDump(sourceCode);

    // STEP 1: Lexical Analysis
    std::cout << "\n=== STEP 1: LEXICAL ANALYSIS ===\n" << std::endl;

    // Create lexer with the source code
    Lexer lexer(sourceCode, filename);

    // Tokenize the entire file
    std::vector<Token> tokens;
    Token token;

    std::cout << "Lexing file..." << std::endl;

    do {
        token = lexer.nextToken();
        tokens.push_back(token);
        std::cout << token << std::endl;
    } while (token.getType() != TokenType::END_OF_FILE);

    // Count tokens by type
    std::map<TokenType, int> tokenCounts;
    for (const auto& t : tokens) {
        tokenCounts[t.getType()]++;
    }

    std::cout << "\nToken statistics:" << std::endl;
    for (const auto& pair : tokenCounts) {
        std::cout << tokenTypeToString(pair.first) << ": " << pair.second << std::endl;
    }

    // Check for lexical errors
    if (lexer.hasErrors()) {
        std::cout << "\nLexical errors found:" << std::endl;

        // Print diagnostic messages
        for (const auto& diag : lexer.getDiagnosticReporter().getDiagnostics()) {
            if (diag.getSeverity() == DiagnosticSeverity::ERROR) {
                std::cout << "[ERROR] " << diag.getLocation().filename << ":"
                          << diag.getLocation().line << ":" << diag.getLocation().column << ": "
                          << diag.getMessage() << std::endl;
            }
        }
        return 1;
    } else {
        std::cout << "\nLexical analysis completed successfully." << std::endl;
    }

    // STEP 2: Syntax Analysis (Parsing)
    std::cout << "\n=== STEP 2: SYNTAX ANALYSIS (PARSING) ===\n" << std::endl;

    // Reset lexer for parsing
    lexer = Lexer(sourceCode, filename);

    // Create parser
    Parser parser(lexer);

    // Parse the tokens into an AST
    std::cout << "Parsing tokens into AST..." << std::endl;
    std::unique_ptr<AstNode> ast = parser.parse();

    // Check for syntax errors
    if (parser.hasErrors() || !ast) {
        std::cout << "\nSyntax errors found:" << std::endl;

        // Print diagnostic messages
        const auto& diagnostics = parser.getDiagnosticReporter().getDiagnostics();
        for (const auto& diag : diagnostics) {
            if (diag.getSeverity() == DiagnosticSeverity::ERROR) {
                std::cout << "[ERROR] " << diag.getLocation().filename << ":"
                          << diag.getLocation().line << ":" << diag.getLocation().column << ": "
                          << diag.getMessage() << std::endl;
            }
        }
        return 1;
    } else {
        std::cout << "\nSyntax analysis completed successfully." << std::endl;
        std::cout << "AST root created." << std::endl;
    }

    // STEP 3: Semantic Analysis
    std::cout << "\n=== STEP 3: SEMANTIC ANALYSIS ===\n" << std::endl;

    // Convert to ProgramNode
    ProgramNode* programNode = dynamic_cast<ProgramNode*>(ast.get());
    if (!programNode) {
        std::cerr << "Internal error: AST root is not a ProgramNode" << std::endl;
        return 1;
    }

    // Create semantic analyzer
    DiagnosticReporter semanticDiagnostics;
    SimpleSemanticAnalyzer semanticAnalyzer(semanticDiagnostics);

    // Perform semantic analysis
    std::cout << "Performing semantic analysis..." << std::endl;
    bool semanticsValid = semanticAnalyzer.analyze(programNode);

    // Check for semantic errors
    if (!semanticsValid) {
        std::cout << "\nSemantic errors found:" << std::endl;

        // Print diagnostic messages
        const auto& diagnostics = semanticDiagnostics.getDiagnostics();
        for (const auto& diag : diagnostics) {
            if (diag.getSeverity() == DiagnosticSeverity::ERROR) {
                std::cout << "[ERROR] " << diag.getLocation().filename << ":"
                          << diag.getLocation().line << ":" << diag.getLocation().column << ": "
                          << diag.getMessage() << std::endl;
            }
        }
        return 1;
    } else {
        std::cout << "\nSemantic analysis completed successfully." << std::endl;
    }

    // STEP 4: Code Generation
    std::cout << "\n=== STEP 4: CODE GENERATION ===\n" << std::endl;

    // Create code generator
    SimpleCodeGenerator codeGenerator;

    // Generate code
    std::cout << "Generating code..." << std::endl;
    std::string generatedCode = codeGenerator.generate(programNode);

    // Save the generated code to the output file
    if (saveToFile(generatedCode, outputFile)) {
        std::cout << "\nCode generation completed successfully." << std::endl;
        std::cout << "Generated code saved to: " << outputFile << std::endl;
    } else {
        std::cerr << "Failed to save generated code" << std::endl;
        return 1;
    }

    std::cout << "\nCompilation completed successfully!" << std::endl;
    return 0;
}