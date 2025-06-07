#include <gtest/gtest.h>

#include <memory>
#include <variant>

#include "../include/ast_nodes.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/source_file.h"

using namespace chronovyan;

// Helper function to parse a string and return the AST
std::unique_ptr<ProgramNode> parseString(const std::string& source) {
    auto sourceFile = std::make_shared<SourceFile>(std::string(source), "test.chron");
    auto lexer = std::make_shared<Lexer>(sourceFile);
    Parser parser(lexer);
    return parser.parse();
}

// Helper function to verify a BlockStmtNode has the expected number of statements
void verifyBlockStmtSize(const BlockStmtNode* block, size_t expectedSize) {
    ASSERT_NE(block, nullptr);
    ASSERT_EQ(block->getStatements().size(), expectedSize);
}

// Helper function to get int value from LiteralValue
int64_t getIntValue(const LiteralExprNode::LiteralValue& value) { return std::get<int64_t>(value); }

// Test FOR_CHRONON statement
TEST(ParserLegacyTest, ForChrononBasic) {
    std::string source = R"(
        FOR_CHRONON(t : 1 to 10) {
            // Loop body
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto forStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(forStmt, nullptr);

    // Test iterator name
    EXPECT_EQ(forStmt->getIterator(), "t");

    // Test range start
    auto startExpr = dynamic_cast<const LiteralExprNode*>(&forStmt->getRangeStart());
    ASSERT_NE(startExpr, nullptr);
    EXPECT_EQ(getIntValue(startExpr->getValue()), 1);

    // Test range end
    auto endExpr = dynamic_cast<const LiteralExprNode*>(&forStmt->getRangeEnd());
    ASSERT_NE(endExpr, nullptr);
    EXPECT_EQ(getIntValue(endExpr->getValue()), 10);

    // Test no step
    EXPECT_FALSE(forStmt->hasStep());

    // Test body
    const auto& body = forStmt->getBody();
    ASSERT_EQ(body.getStatements().size(), 0);
}

// Test FOR_CHRONON with step
TEST(ParserLegacyTest, ForChrononWithStep) {
    std::string source = R"(
        FOR_CHRONON(t : 1 to 10 step 2) {
            // Loop body
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto forStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(forStmt, nullptr);

    // Test iterator name
    EXPECT_EQ(forStmt->getIterator(), "t");

    // Test range start
    auto startExpr = dynamic_cast<const LiteralExprNode*>(&forStmt->getRangeStart());
    ASSERT_NE(startExpr, nullptr);
    EXPECT_EQ(getIntValue(startExpr->getValue()), 1);

    // Test range end
    auto endExpr = dynamic_cast<const LiteralExprNode*>(&forStmt->getRangeEnd());
    ASSERT_NE(endExpr, nullptr);
    EXPECT_EQ(getIntValue(endExpr->getValue()), 10);

    // Test step
    ASSERT_TRUE(forStmt->hasStep());
    auto stepExpr = dynamic_cast<const LiteralExprNode*>(&forStmt->getStepExpr());
    ASSERT_NE(stepExpr, nullptr);
    EXPECT_EQ(getIntValue(stepExpr->getValue()), 2);
}

// Test nested FOR_CHRONON statements
TEST(ParserLegacyTest, NestedForChronon) {
    std::string source = R"(
        FOR_CHRONON(outer : 1 to 3) {
            FOR_CHRONON(inner : 1 to 5) {
                // Nested loop body
            }
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto outerForStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(outerForStmt, nullptr);
    EXPECT_EQ(outerForStmt->getIterator(), "outer");

    const auto& outerBody = outerForStmt->getBody();
    ASSERT_EQ(outerBody.getStatements().size(), 1);

    auto innerForStmt = dynamic_cast<ForChrononStmtNode*>(outerBody.getStatements()[0].get());
    ASSERT_NE(innerForStmt, nullptr);
    EXPECT_EQ(innerForStmt->getIterator(), "inner");

    const auto& innerBody = innerForStmt->getBody();
    ASSERT_EQ(innerBody.getStatements().size(), 0);
}

// Test WHILE_EVENT statement
TEST(ParserLegacyTest, WhileEventBasic) {
    std::string source = R"(
        WHILE_EVENT(x < 10) {
            // Loop body
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto whileStmt = dynamic_cast<WhileEventStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(whileStmt, nullptr);

    // Test condition
    auto condition = dynamic_cast<const BinaryExprNode*>(&whileStmt->getCondition());
    ASSERT_NE(condition, nullptr);

    // Test body
    const auto& whileBody = whileStmt->getBody();
    ASSERT_EQ(whileBody.getStatements().size(), 0);
}

// Test REWIND_FLOW statement
TEST(ParserLegacyTest, RewindFlowBasic) {
    std::string source = R"(
        REWIND_FLOW(condition) {
            // Loop body
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto rewindStmt = dynamic_cast<RewindFlowStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(rewindStmt, nullptr);

    // Test condition
    auto condition = dynamic_cast<const VariableExprNode*>(&rewindStmt->getCondition());
    ASSERT_NE(condition, nullptr);

    // Test that there's no max rewinds
    EXPECT_FALSE(rewindStmt->hasMaxRewinds());

    // Test body
    const auto& rewindBody = rewindStmt->getBody();
    ASSERT_EQ(rewindBody.getStatements().size(), 0);
}

// Test ATTEMPT_WEAVE statement
TEST(ParserLegacyTest, AttemptWeaveBasic) {
    std::string source = R"(
        ATTEMPT_WEAVE {
            // Try block
        } ON_DISSONANCE(TimeParadoxError, err) {
            // Error handler
        } ENSURE_HARMONY {
            // Finally block
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto attemptWeaveStmt = dynamic_cast<AttemptWeaveStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(attemptWeaveStmt, nullptr);

    // Test try block
    const auto& tryBlock = attemptWeaveStmt->getTryBlock();
    ASSERT_EQ(tryBlock.getStatements().size(), 0);

    // Test handlers
    const auto& handlers = attemptWeaveStmt->getHandlers();
    ASSERT_EQ(handlers.size(), 1);

    // Test first handler
    auto firstHandler = handlers[0].get();
    ASSERT_NE(firstHandler, nullptr);

    // Test dissonance type
    ASSERT_TRUE(firstHandler->hasDissonanceType());
    EXPECT_EQ(firstHandler->getDissonanceType().getNameToken().lexeme, "TimeParadoxError");

    // Test error variable name
    EXPECT_EQ(firstHandler->getErrorVar(), "err");

    // Test handler body
    const auto& handlerBody = firstHandler->getBody();
    ASSERT_EQ(handlerBody.getStatements().size(), 0);

    // Test finally block
    ASSERT_TRUE(attemptWeaveStmt->hasFinallyBlock());
    const auto& finallyBlock = attemptWeaveStmt->getFinallyBlock();
    ASSERT_EQ(finallyBlock.getStatements().size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}