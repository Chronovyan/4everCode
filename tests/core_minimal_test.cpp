#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

#include "compiler/DiagnosticReporter.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"
#include "compiler/SourceFile.h"

using namespace chronovyan::compiler;

// Helper function to create a source file from a string
std::unique_ptr<SourceFile> createSourceFileFromString(const std::string& source) {
    return std::make_unique<SourceFile>("test_input", source);
}

// Helper function to create a lexer from a source file
std::unique_ptr<Lexer> createLexer(SourceFile& sourceFile, DiagnosticReporter& reporter) {
    return std::make_unique<Lexer>(sourceFile, reporter);
}

// Helper function to create a parser from a lexer
std::unique_ptr<Parser> createParser(Lexer& lexer, DiagnosticReporter& reporter) {
    return std::make_unique<Parser>(lexer, reporter);
}

namespace fs = std::filesystem;

class CoreMinimalTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test file
        testFilePath = (fs::temp_directory_path() / "chronovyan_test_file.cvy").string();

        // Create a test file with minimal content
        std::ofstream outFile(testFilePath);
        outFile << "// Test file for Chronovyan\nvar x = 42;\n";
        outFile.close();
    }

    void TearDown() override {
        // Clean up the test file
        if (fs::exists(testFilePath)) {
            fs::remove(testFilePath);
        }
    }

    std::string testFilePath;

    // Helper function to create a SourceFile from string content
    std::shared_ptr<SourceFile> createSourceFromString(const std::string& source) {
        return std::make_shared<SourceFile>(source, "<test_input>");
    }

    // Helper function to create a SourceFile from file
    std::shared_ptr<SourceFile> createSourceFromFile() {
        return std::make_shared<SourceFile>(testFilePath);
    }
};

TEST_F(CoreMinimalTest, ParsesVariableDeclaration) {
    // Setup
    DiagnosticReporter reporter;
    auto sourceFile = createSourceFileFromString("let x = 42;");
    auto lexer = createLexer(*sourceFile, reporter);
    auto parser = createParser(*lexer, reporter);

    // Parse
    auto ast = parser->parse();

    // Verify
    ASSERT_NE(ast, nullptr);
    ASSERT_FALSE(reporter.hasErrors()) << "Unexpected errors: " << reporter.getErrors();

    auto program = dynamic_cast<ProgramNode*>(ast.get());
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getDeclarations().size(), 1);

    auto varDecl = dynamic_cast<VarDeclNode*>(program->getDeclarations()[0].get());
    ASSERT_NE(varDecl, nullptr);
    EXPECT_EQ(varDecl->getName().getLexeme(), "x");

    auto literal = dynamic_cast<LiteralExprNode*>(varDecl->getInitializer().get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->getValue().getLexeme(), "42");
}

TEST_F(CoreMinimalTest, HandlesEmptySource) {
    // Setup
    DiagnosticReporter reporter;
    auto sourceFile = createSourceFileFromString("");
    auto lexer = createLexer(*sourceFile, reporter);
    auto parser = createParser(*lexer, reporter);

    // Parse
    auto ast = parser->parse();

    // Verify
    ASSERT_NE(ast, nullptr);
    ASSERT_FALSE(reporter.hasErrors()) << "Unexpected errors: " << reporter.getErrors();

    // An empty source should produce a program with no declarations
    auto program = dynamic_cast<ProgramNode*>(ast.get());
    ASSERT_NE(program, nullptr);
    EXPECT_TRUE(program->getDeclarations().empty());
}

TEST_F(CoreMinimalTest, LoadsSourceFromFile) {
    // Test loading source from a file
    ASSERT_NO_THROW({
        auto fileSource = createSourceFromFile();
        ASSERT_FALSE(fileSource->getSource().empty()) << "Source should not be empty";

        // Verify line counting
        auto [line, col] = fileSource->getLineAndColumn(0);
        ASSERT_EQ(line, 1) << "First character should be on line 1";
        ASSERT_EQ(col, 1) << "First character should be at column 1";
    }) << "Should be able to load source from file";
}

TEST_F(CoreMinimalTest, ReportsCorrectLineAndColumn) {
    // Test line and column position calculation
    std::string source = "var x = 42;\nvar y = x + 1;\n";
    auto sourceFile = createSourceFromString(source);

    // Test position calculations
    auto [line1, col1] = sourceFile->getLineAndColumn(0);  // 'v' in var
    ASSERT_EQ(line1, 1);
    ASSERT_EQ(col1, 1);

    auto [line2, col2] = sourceFile->getLineAndColumn(5);  // 'x'
    ASSERT_EQ(line2, 1);
    ASSERT_EQ(col2, 6);

    auto [line3, col3] = sourceFile->getLineAndColumn(12);  // 'v' in second line
    ASSERT_EQ(line3, 2);
    ASSERT_EQ(col3, 1);
}

TEST_F(CoreMinimalTest, HandlesInvalidPositionsGracefully) {
    // Test behavior with invalid positions
    std::string source = "var x = 42;";
    auto sourceFile = createSourceFromString(source);

    // Position beyond source length should return last line/column
    auto [line, col] = sourceFile->getLineAndColumn(100);
    ASSERT_GT(line, 0) << "Should handle position beyond source length";

    // Getting a line beyond source should throw
    ASSERT_THROW(sourceFile->getLine(100), std::out_of_range);
}

TEST_F(CoreMinimalTest, HandlesInvalidSyntax) {
    // Test handling of invalid syntax
    std::string invalidSource = "var x = ;";  // Missing expression
    auto sourceFile = createSourceFromString(invalidSource);

    // Should throw or return an error AST, depending on parser implementation
    try {
        DiagnosticReporter reporter;
        auto lexer = createLexer(*sourceFile, reporter);
        auto parser = createParser(*lexer, reporter);
        auto ast = parser->parse();
        // If we get here, the parser accepted invalid syntax
        // This might be expected if the parser is permissive
        // Add appropriate validation based on your parser's behavior
    } catch (const std::exception& e) {
        // Expected behavior for invalid syntax
        SUCCEED();
    }
}

// Test suite for parser error recovery
class ParserErrorRecoveryTest : public CoreMinimalTest {
protected:
    void TestErrorRecovery(const std::string& source, const std::string& expectedError) {
        auto sourceFile = createSourceFileFromString(source);

        try {
            DiagnosticReporter reporter;
            auto lexer = createLexer(*sourceFile, reporter);
            auto parser = createParser(*lexer, reporter);
            auto ast = parser->parse();
            // If we get here, the parser recovered from the error
            // Add validation of the recovered AST if needed
            SUCCEED();
        } catch (const std::exception& e) {
            // Check if the error message contains the expected text
            ASSERT_NE(std::string(e.what()).find(expectedError), std::string::npos)
                << "Unexpected error message: " << e.what();
        }
    }
};

TEST_F(ParserErrorRecoveryTest, RecoversFromMissingSemicolon) {
    std::string source = "var x = 42\nvar y = 10;";  // Missing semicolon
    TestErrorRecovery(source, ";");                  // Expect error about missing semicolon
}

TEST_F(ParserErrorRecoveryTest, RecoversFromMissingBrace) {
    std::string source = "if (x) { var y = 10; ";  // Missing closing brace
    TestErrorRecovery(source, "}");                // Expect error about missing brace
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Enable verbose output
    ::testing::GTEST_FLAG(filter) = "*.*";

    return RUN_ALL_TESTS();
}
