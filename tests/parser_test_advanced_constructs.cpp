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

// Helper function to get int value from LiteralValue
int64_t getIntValue(const LiteralExprNode::LiteralValue& value) { return std::get<int64_t>(value); }

// Helper function to verify a block statement has the expected number of children
void verifyBlockStmtSize(const BlockStmtNode* blockStmt, size_t expectedSize) {
    ASSERT_NE(blockStmt, nullptr);
    ASSERT_EQ(blockStmt->getStatements().size(), expectedSize);
}

// Test BREAK_CHRONON statement
TEST(ParserAdvancedTest, BreakChrononBasic) {
    std::string source = R"(
        FOR_CHRONON(t : 1 to 10) {
            BREAK_CHRONON;
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto forStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(forStmt, nullptr);

    const auto& body = forStmt->getBody();
    ASSERT_EQ(body.getStatements().size(), 1);

    auto breakStmt = dynamic_cast<BreakChrononStmtNode*>(body.getStatements()[0].get());
    ASSERT_NE(breakStmt, nullptr);
}

// Test BREAK_CHRONON in nested loops
TEST(ParserAdvancedTest, BreakChrononNested) {
    std::string source = R"(
        FOR_CHRONON(outer : 1 to 5) {
            FOR_CHRONON(inner : 1 to 5) {
                IF(inner > 3) {
                    BREAK_CHRONON;
                }
            }
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto outerForStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(outerForStmt, nullptr);

    const auto& outerBody = outerForStmt->getBody();
    ASSERT_EQ(outerBody.getStatements().size(), 1);

    auto innerForStmt = dynamic_cast<ForChrononStmtNode*>(outerBody.getStatements()[0].get());
    ASSERT_NE(innerForStmt, nullptr);

    const auto& innerBody = innerForStmt->getBody();
    ASSERT_EQ(innerBody.getStatements().size(), 1);

    auto ifStmt = dynamic_cast<IfStmtNode*>(innerBody.getStatements()[0].get());
    ASSERT_NE(ifStmt, nullptr);

    auto thenBranch = dynamic_cast<BlockStmtNode*>(&ifStmt->getThenBranch());
    ASSERT_NE(thenBranch, nullptr);
    ASSERT_EQ(thenBranch->getStatements().size(), 1);

    auto breakStmt = dynamic_cast<BreakChrononStmtNode*>(thenBranch->getStatements()[0].get());
    ASSERT_NE(breakStmt, nullptr);
}

// Test CONTINUE_WEAVE statement
TEST(ParserAdvancedTest, ContinueWeaveBasic) {
    std::string source = R"(
        FOR_CHRONON(t : 1 to 10) {
            CONTINUE_WEAVE;
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto forStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(forStmt, nullptr);

    const auto& body = forStmt->getBody();
    ASSERT_EQ(body.getStatements().size(), 1);

    auto continueStmt = dynamic_cast<ContinueWeaveStmtNode*>(body.getStatements()[0].get());
    ASSERT_NE(continueStmt, nullptr);
}

// Test CONTINUE_WEAVE in nested loops
TEST(ParserAdvancedTest, ContinueWeaveNested) {
    std::string source = R"(
        WHILE_EVENT(x < 10) {
            IF(x % 2 == 0) {
                CONTINUE_WEAVE;
            }
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto whileStmt = dynamic_cast<WhileEventStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(whileStmt, nullptr);

    const auto& body = whileStmt->getBody();
    ASSERT_EQ(body.getStatements().size(), 1);

    auto ifStmt = dynamic_cast<IfStmtNode*>(body.getStatements()[0].get());
    ASSERT_NE(ifStmt, nullptr);

    auto thenBranch = dynamic_cast<BlockStmtNode*>(&ifStmt->getThenBranch());
    ASSERT_NE(thenBranch, nullptr);
    ASSERT_EQ(thenBranch->getStatements().size(), 1);

    auto continueStmt = dynamic_cast<ContinueWeaveStmtNode*>(thenBranch->getStatements()[0].get());
    ASSERT_NE(continueStmt, nullptr);
}

// Test REVERSE_FLOW statement
TEST(ParserAdvancedTest, ReverseFlowBasic) {
    std::string source = R"(
        FOR_CHRONON(t : 1 to 10) {
            REVERSE_FLOW;
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto forStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(forStmt, nullptr);

    const auto& body = forStmt->getBody();
    ASSERT_EQ(body.getStatements().size(), 1);

    auto reverseStmt = dynamic_cast<ReverseFlowStmtNode*>(body.getStatements()[0].get());
    ASSERT_NE(reverseStmt, nullptr);
}

// Test REVERSE_FLOW in complex scenarios
TEST(ParserAdvancedTest, ReverseFlowComplex) {
    std::string source = R"(
        FOR_CHRONON(t : 1 to 10) {
            IF(t > 5) {
                REVERSE_FLOW;
            } ELSE IF(t < 3) {
                CONTINUE_WEAVE;
            }
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto forStmt = dynamic_cast<ForChrononStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(forStmt, nullptr);

    const auto& body = forStmt->getBody();
    ASSERT_EQ(body.getStatements().size(), 1);

    auto ifStmt = dynamic_cast<IfStmtNode*>(body.getStatements()[0].get());
    ASSERT_NE(ifStmt, nullptr);

    auto thenBranch = dynamic_cast<BlockStmtNode*>(&ifStmt->getThenBranch());
    ASSERT_NE(thenBranch, nullptr);
    ASSERT_EQ(thenBranch->getStatements().size(), 1);

    auto reverseStmt = dynamic_cast<ReverseFlowStmtNode*>(thenBranch->getStatements()[0].get());
    ASSERT_NE(reverseStmt, nullptr);

    ASSERT_TRUE(ifStmt->hasElseBranch());
    auto elseIfStmt = dynamic_cast<IfStmtNode*>(&ifStmt->getElseBranch());
    ASSERT_NE(elseIfStmt, nullptr);
}

// Test EXPEND_RESOURCES with Aethel only
TEST(ParserAdvancedTest, ExpendResourcesAethelOnly) {
    std::string source = R"(
        EXPEND_RESOURCES(AETHEL: 50) {
            // Main block
        } ON_INSUFFICIENT_RESOURCES {
            // Handler block
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto expendStmt = dynamic_cast<ExpendResourcesStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(expendStmt, nullptr);

    ASSERT_TRUE(expendStmt->hasAethelCost());
    ASSERT_FALSE(expendStmt->hasChrononCost());

    auto aethelCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getAethelCostExpr());
    ASSERT_NE(aethelCost, nullptr);
    EXPECT_EQ(getIntValue(aethelCost->getValue()), 50);
}

// Test EXPEND_RESOURCES with Chronons only
TEST(ParserAdvancedTest, ExpendResourcesChrononsOnly) {
    std::string source = R"(
        EXPEND_RESOURCES(CHRONONS: 100) {
            // Main block
        } ON_INSUFFICIENT_RESOURCES {
            // Handler block
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto expendStmt = dynamic_cast<ExpendResourcesStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(expendStmt, nullptr);

    ASSERT_FALSE(expendStmt->hasAethelCost());
    ASSERT_TRUE(expendStmt->hasChrononCost());

    auto chrononCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getChrononCostExpr());
    ASSERT_NE(chrononCost, nullptr);
    EXPECT_EQ(getIntValue(chrononCost->getValue()), 100);
}

// Test EXPEND_RESOURCES with both resource types
TEST(ParserAdvancedTest, ExpendResourcesBoth) {
    std::string source = R"(
        EXPEND_RESOURCES(AETHEL: 50, CHRONONS: 100) {
            // Main block
        } ON_INSUFFICIENT_RESOURCES {
            // Handler block
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto expendStmt = dynamic_cast<ExpendResourcesStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(expendStmt, nullptr);

    ASSERT_TRUE(expendStmt->hasAethelCost());
    ASSERT_TRUE(expendStmt->hasChrononCost());

    auto aethelCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getAethelCostExpr());
    ASSERT_NE(aethelCost, nullptr);
    EXPECT_EQ(getIntValue(aethelCost->getValue()), 50);

    auto chrononCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getChrononCostExpr());
    ASSERT_NE(chrononCost, nullptr);
    EXPECT_EQ(getIntValue(chrononCost->getValue()), 100);
}

// Test EXPEND_RESOURCES without insufficient handler
TEST(ParserAdvancedTest, ExpendResourcesWithoutHandler) {
    std::string source = R"(
        EXPEND_RESOURCES(AETHEL: 50, CHRONONS: 100) {
            // Main block
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto expendStmt = dynamic_cast<ExpendResourcesStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(expendStmt, nullptr);

    ASSERT_TRUE(expendStmt->hasAethelCost());
    ASSERT_TRUE(expendStmt->hasChrononCost());

    auto aethelCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getAethelCostExpr());
    ASSERT_NE(aethelCost, nullptr);
    EXPECT_EQ(getIntValue(aethelCost->getValue()), 50);

    auto chrononCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getChrononCostExpr());
    ASSERT_NE(chrononCost, nullptr);
    EXPECT_EQ(getIntValue(chrononCost->getValue()), 100);

    ASSERT_FALSE(expendStmt->hasInsufficientHandler());
}

// Test complex resource management
TEST(ParserAdvancedTest, ComplexResourceManagement) {
    std::string source = R"(
        EXPEND_RESOURCES(AETHEL: 50, CHRONONS: 100) {
            FOR_CHRONON(t : 1 to 10) {
                IF(t > 5) {
                    BREAK_CHRONON;
                }
            }
        } ON_INSUFFICIENT_RESOURCES {
            // Handler block
        }
    )";

    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->getStatements().size(), 1);

    auto expendStmt = dynamic_cast<ExpendResourcesStmtNode*>(program->getStatements()[0].get());
    ASSERT_NE(expendStmt, nullptr);

    ASSERT_TRUE(expendStmt->hasAethelCost());
    ASSERT_TRUE(expendStmt->hasChrononCost());

    auto aethelCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getAethelCostExpr());
    ASSERT_NE(aethelCost, nullptr);
    EXPECT_EQ(getIntValue(aethelCost->getValue()), 50);

    auto chrononCost = dynamic_cast<const LiteralExprNode*>(&expendStmt->getChrononCostExpr());
    ASSERT_NE(chrononCost, nullptr);
    EXPECT_EQ(getIntValue(chrononCost->getValue()), 100);

    const auto& mainBlock = expendStmt->getMainBlock();
    ASSERT_EQ(mainBlock.getStatements().size(), 1);

    auto forStmt = dynamic_cast<ForChrononStmtNode*>(mainBlock.getStatements()[0].get());
    ASSERT_NE(forStmt, nullptr);

    ASSERT_TRUE(expendStmt->hasInsufficientHandler());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}