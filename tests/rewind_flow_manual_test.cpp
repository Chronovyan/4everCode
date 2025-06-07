#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

// Simplified mock implementation for testing enhanced REWIND_FLOW control capabilities

class SourceLocation {
public:
    int line = 0;
    int column = 0;
    std::string filename = "";
};

// Forward declarations
class StmtNode;
class ExprNode;
class BlockStmtNode;

// Simple AST Visitor interface
class TestVisitor {
public:
    virtual ~TestVisitor() = default;
    virtual void visitBlock(const BlockStmtNode& block) = 0;
    virtual void visitRewindFlow(const StmtNode& stmt) = 0;
    virtual void visitHaltRewind(const StmtNode& stmt) = 0;
    virtual void visitStabilizeTimeline(const StmtNode& stmt) = 0;
    virtual void visitPreventModification(const StmtNode& stmt) = 0;
};

// Base Node class
class Node {
public:
    explicit Node(const SourceLocation& loc) : location(loc) {}
    virtual ~Node() = default;
    SourceLocation location;
};

// Statement Node
class StmtNode : public Node {
public:
    explicit StmtNode(const SourceLocation& loc) : Node(loc) {}
    virtual void accept(TestVisitor& visitor) const = 0;
};

// Block Node containing multiple statements
class BlockStmtNode : public StmtNode {
public:
    explicit BlockStmtNode(const SourceLocation& loc, std::vector<std::unique_ptr<StmtNode>> stmts)
        : StmtNode(loc), statements(std::move(stmts)) {}

    void accept(TestVisitor& visitor) const override { visitor.visitBlock(*this); }

    const std::vector<std::unique_ptr<StmtNode>>& getStatements() const { return statements; }

private:
    std::vector<std::unique_ptr<StmtNode>> statements;
};

// HALT_REWIND statement
class HaltRewindStmtNode : public StmtNode {
public:
    explicit HaltRewindStmtNode(const SourceLocation& loc) : StmtNode(loc) {}

    void accept(TestVisitor& visitor) const override { visitor.visitHaltRewind(*this); }
};

// STABILIZE_TIMELINE statement
class StabilizeTimelineStmtNode : public StmtNode {
public:
    StabilizeTimelineStmtNode(const SourceLocation& loc, double intensity)
        : StmtNode(loc), intensity_value(intensity) {}

    void accept(TestVisitor& visitor) const override { visitor.visitStabilizeTimeline(*this); }

    double getIntensity() const { return intensity_value; }

private:
    double intensity_value;
};

// PREVENT_MODIFICATION statement
class PreventModificationStmtNode : public StmtNode {
public:
    PreventModificationStmtNode(const SourceLocation& loc, std::string scope = "local")
        : StmtNode(loc), scope_value(scope) {}

    void accept(TestVisitor& visitor) const override { visitor.visitPreventModification(*this); }

    const std::string& getScope() const { return scope_value; }

private:
    std::string scope_value;
};

// REWIND_FLOW statement
class RewindFlowStmtNode : public StmtNode {
public:
    RewindFlowStmtNode(const SourceLocation& loc, std::unique_ptr<BlockStmtNode> body,
                       int max_rewinds = -1)
        : StmtNode(loc), body_stmt(std::move(body)), max_rewinds_value(max_rewinds) {}

    void accept(TestVisitor& visitor) const override { visitor.visitRewindFlow(*this); }

    const BlockStmtNode& getBody() const { return *body_stmt; }

    bool hasMaxRewinds() const { return max_rewinds_value >= 0; }

    int getMaxRewinds() const { return max_rewinds_value; }

private:
    std::unique_ptr<BlockStmtNode> body_stmt;
    int max_rewinds_value;
};

// Simple interpreter to execute the AST
class Interpreter : public TestVisitor {
public:
    Interpreter()
        : m_haltRewind_flag(false)
        , m_readOnlyMode(false)
        , m_stabilizationFactor(0.0)
        , m_temporalResources(100.0)
        , m_debugLogging(true) {}

    void visitBlock(const BlockStmtNode& block) override {
        std::cout << "Executing block with " << block.getStatements().size() << " statements"
                  << std::endl;

        for (const auto& stmt : block.getStatements()) {
            stmt->accept(*this);

            // Check for early termination via HALT_REWIND
            if (m_haltRewind_flag) {
                std::cout << "Block execution halted by HALT_REWIND" << std::endl;
                break;
            }
        }
    }

    void visitRewindFlow(const StmtNode& stmt) override {
        const auto& rewindStmt = static_cast<const RewindFlowStmtNode&>(stmt);

        std::cout << "Executing REWIND_FLOW" << std::endl;

        // Save old states
        bool oldHaltRewind = m_haltRewind_flag;
        bool oldReadOnlyMode = m_readOnlyMode;
        double oldStabilizationFactor = m_stabilizationFactor;

        // Reset flags for this execution
        m_haltRewind_flag = false;

        // Determine max rewinds
        int maxRewinds = rewindStmt.hasMaxRewinds() ? rewindStmt.getMaxRewinds() : -1;
        int rewindCount = 0;

        // Execute the rewind loop
        while (true) {
            // Check if we've reached the maximum rewinds
            if (maxRewinds >= 0 && rewindCount >= maxRewinds) {
                std::cout << "Reached maximum rewinds: " << maxRewinds << std::endl;
                break;
            }

            // For demonstration, we'll use a simple counter as the condition
            // In a real implementation, this would evaluate an expression
            bool condition = (rewindCount < 5);

            if (!condition) {
                std::cout << "REWIND_FLOW condition is false, exiting loop" << std::endl;
                break;
            }

            // Consume resources
            double resourceCost = 10.0;  // Base cost for REWIND_FLOW
            consumeResources(resourceCost);

            std::cout << "REWIND_FLOW iteration " << rewindCount + 1 << std::endl;

            // Execute the body
            rewindStmt.getBody().accept(*this);

            // Check for HALT_REWIND
            if (m_haltRewind_flag) {
                std::cout << "REWIND_FLOW terminated by HALT_REWIND" << std::endl;
                break;
            }

            rewindCount++;
        }

        // Restore states
        m_haltRewind_flag = oldHaltRewind;
        m_readOnlyMode = oldReadOnlyMode;
        m_stabilizationFactor = oldStabilizationFactor;

        std::cout << "REWIND_FLOW completed with " << rewindCount << " iterations" << std::endl;
    }

    void visitHaltRewind(const StmtNode& stmt) override {
        std::cout << "Executing HALT_REWIND" << std::endl;
        m_haltRewind_flag = true;
    }

    void visitStabilizeTimeline(const StmtNode& stmt) override {
        const auto& stabilizeStmt = static_cast<const StabilizeTimelineStmtNode&>(stmt);
        double intensity = stabilizeStmt.getIntensity();

        // Clamp intensity to [0.0, 1.0]
        if (intensity < 0.0)
            intensity = 0.0;
        if (intensity > 1.0)
            intensity = 1.0;

        std::cout << "Executing STABILIZE_TIMELINE with intensity " << intensity << std::endl;

        // Apply stabilization
        m_stabilizationFactor = intensity;

        // Calculate resource cost
        double resourceCost = 5.0 * intensity;  // Base cost * intensity
        consumeResources(resourceCost);
    }

    void visitPreventModification(const StmtNode& stmt) override {
        const auto& preventStmt = static_cast<const PreventModificationStmtNode&>(stmt);
        std::string scope = preventStmt.getScope();

        std::cout << "Executing PREVENT_MODIFICATION with scope '" << scope << "'" << std::endl;

        // Apply modification prevention
        if (scope == "local" || scope == "global") {
            m_readOnlyMode = true;
            std::cout << "Read-only mode enabled" << std::endl;
        }

        // Calculate resource cost
        double scopeFactor = 1.0;
        if (scope == "global")
            scopeFactor = 2.0;
        if (scope == "timeline")
            scopeFactor = 1.5;

        double resourceCost = 7.5 * scopeFactor;  // Base cost * scope factor
        consumeResources(resourceCost);
    }

    void execute(const StmtNode& stmt) { stmt.accept(*this); }

private:
    bool m_haltRewind_flag;
    bool m_readOnlyMode;
    double m_stabilizationFactor;
    double m_temporalResources;
    bool m_debugLogging;

    void consumeResources(double amount) {
        if (m_temporalResources < amount) {
            std::cout << "RESOURCE ERROR: Insufficient resources - needed " << amount
                      << ", available " << m_temporalResources << std::endl;
            return;
        }

        m_temporalResources -= amount;

        if (m_debugLogging) {
            std::cout << "Consumed " << amount << " resources. Remaining: " << m_temporalResources
                      << std::endl;
        }
    }
};

// Test function to create and run a sample program
void testEnhancedRewindFlow() {
    std::cout << "=== Testing Enhanced REWIND_FLOW Controls ===" << std::endl;

    // Create statements for the test
    auto loc = SourceLocation{1, 1, "test.cvy"};

    // Create inner block for REWIND_FLOW
    std::vector<std::unique_ptr<StmtNode>> innerStatements;

    // Add STABILIZE_TIMELINE with 0.7 intensity
    innerStatements.push_back(
        std::make_unique<StabilizeTimelineStmtNode>(SourceLocation{2, 3, "test.cvy"}, 0.7));

    // Add PREVENT_MODIFICATION with "local" scope
    innerStatements.push_back(
        std::make_unique<PreventModificationStmtNode>(SourceLocation{3, 3, "test.cvy"}, "local"));

    // Add HALT_REWIND to terminate early
    innerStatements.push_back(
        std::make_unique<HaltRewindStmtNode>(SourceLocation{4, 3, "test.cvy"}));

    // Create the block
    auto innerBlock = std::make_unique<BlockStmtNode>(SourceLocation{2, 1, "test.cvy"},
                                                      std::move(innerStatements));

    // Create REWIND_FLOW with max 3 rewinds
    auto rewindFlow = std::make_unique<RewindFlowStmtNode>(loc, std::move(innerBlock), 3);

    // Create interpreter and execute
    Interpreter interpreter;
    interpreter.execute(*rewindFlow);

    std::cout << "=== Test Completed ===" << std::endl;
}

// Test function for HALT_REWIND functionality
void testHaltRewind() {
    std::cout << "\n=== Testing HALT_REWIND Control ===" << std::endl;

    // Create inner block for REWIND_FLOW
    std::vector<std::unique_ptr<StmtNode>> innerStatements;

    // Add some statements, then HALT_REWIND
    innerStatements.push_back(
        std::make_unique<PreventModificationStmtNode>(SourceLocation{2, 3, "test.cvy"}, "local"));

    innerStatements.push_back(
        std::make_unique<HaltRewindStmtNode>(SourceLocation{3, 3, "test.cvy"}));

    innerStatements.push_back(
        std::make_unique<StabilizeTimelineStmtNode>(SourceLocation{4, 3, "test.cvy"}, 0.5));

    // Create the block
    auto innerBlock = std::make_unique<BlockStmtNode>(SourceLocation{2, 1, "test.cvy"},
                                                      std::move(innerStatements));

    // Create REWIND_FLOW with max 10 rewinds (should stop early due to HALT_REWIND)
    auto rewindFlow = std::make_unique<RewindFlowStmtNode>(SourceLocation{1, 1, "test.cvy"},
                                                           std::move(innerBlock), 10);

    // Create interpreter and execute
    Interpreter interpreter;
    interpreter.execute(*rewindFlow);

    std::cout << "=== Test Completed ===" << std::endl;
}

// Test function for PREVENT_MODIFICATION functionality
void testPreventModification() {
    std::cout << "\n=== Testing PREVENT_MODIFICATION Control ===" << std::endl;

    // Create inner block for REWIND_FLOW
    std::vector<std::unique_ptr<StmtNode>> innerStatements;

    // Add statements to demonstrate PREVENT_MODIFICATION
    // First, a modification that will work
    innerStatements.push_back(
        std::make_unique<PreventModificationStmtNode>(SourceLocation{2, 3, "test.cvy"}, "local"));

    // Create the block
    auto innerBlock = std::make_unique<BlockStmtNode>(SourceLocation{2, 1, "test.cvy"},
                                                      std::move(innerStatements));

    // Create REWIND_FLOW
    auto rewindFlow = std::make_unique<RewindFlowStmtNode>(SourceLocation{1, 1, "test.cvy"},
                                                           std::move(innerBlock));

    // Create interpreter and execute
    Interpreter interpreter;
    interpreter.execute(*rewindFlow);

    // Test modifying a variable after PREVENT_MODIFICATION
    std::cout << "Attempting to modify variables after PREVENT_MODIFICATION..." << std::endl;
    std::cout << "This would fail in the full implementation!" << std::endl;

    std::cout << "=== Test Completed ===" << std::endl;
}

// Test function for STABILIZE_TIMELINE functionality with different intensities
void testStabilizeTimeline() {
    std::cout << "\n=== Testing STABILIZE_TIMELINE Control with Different Intensities ==="
              << std::endl;

    // Test with low intensity (0.2)
    {
        std::vector<std::unique_ptr<StmtNode>> innerStatements;
        innerStatements.push_back(
            std::make_unique<StabilizeTimelineStmtNode>(SourceLocation{2, 3, "test.cvy"}, 0.2));

        auto innerBlock = std::make_unique<BlockStmtNode>(SourceLocation{2, 1, "test.cvy"},
                                                          std::move(innerStatements));

        auto rewindFlow = std::make_unique<RewindFlowStmtNode>(SourceLocation{1, 1, "test.cvy"},
                                                               std::move(innerBlock),
                                                               1);  // Just one iteration

        std::cout << "Low Intensity (0.2) Test:" << std::endl;
        Interpreter interpreter;
        interpreter.execute(*rewindFlow);
    }

    // Test with medium intensity (0.5)
    {
        std::vector<std::unique_ptr<StmtNode>> innerStatements;
        innerStatements.push_back(
            std::make_unique<StabilizeTimelineStmtNode>(SourceLocation{2, 3, "test.cvy"}, 0.5));

        auto innerBlock = std::make_unique<BlockStmtNode>(SourceLocation{2, 1, "test.cvy"},
                                                          std::move(innerStatements));

        auto rewindFlow = std::make_unique<RewindFlowStmtNode>(SourceLocation{1, 1, "test.cvy"},
                                                               std::move(innerBlock),
                                                               1);  // Just one iteration

        std::cout << "\nMedium Intensity (0.5) Test:" << std::endl;
        Interpreter interpreter;
        interpreter.execute(*rewindFlow);
    }

    // Test with high intensity (0.9)
    {
        std::vector<std::unique_ptr<StmtNode>> innerStatements;
        innerStatements.push_back(
            std::make_unique<StabilizeTimelineStmtNode>(SourceLocation{2, 3, "test.cvy"}, 0.9));

        auto innerBlock = std::make_unique<BlockStmtNode>(SourceLocation{2, 1, "test.cvy"},
                                                          std::move(innerStatements));

        auto rewindFlow = std::make_unique<RewindFlowStmtNode>(SourceLocation{1, 1, "test.cvy"},
                                                               std::move(innerBlock),
                                                               1);  // Just one iteration

        std::cout << "\nHigh Intensity (0.9) Test:" << std::endl;
        Interpreter interpreter;
        interpreter.execute(*rewindFlow);
    }

    std::cout << "=== Test Completed ===" << std::endl;
}

// Main function
int main() {
    std::cout << "Enhanced REWIND_FLOW Manual Test" << std::endl;
    std::cout << "================================" << std::endl;

    testEnhancedRewindFlow();
    testHaltRewind();
    testPreventModification();
    testStabilizeTimeline();

    return 0;
}