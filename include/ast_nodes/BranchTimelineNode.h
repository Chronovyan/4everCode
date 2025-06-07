#ifndef CHRONOVYAN_BRANCH_TIMELINE_NODE_H
#define CHRONOVYAN_BRANCH_TIMELINE_NODE_H

#include <memory>
#include <string>

#include "ast_nodes/ExprNode.h"
#include "ast_nodes/StmtNode.h"

namespace chronovyan {

class BranchTimelineNode : public StmtNode {
public:
    BranchTimelineNode(SourceLocation loc, std::unique_ptr<ExprNode> branchCount,
                       std::unique_ptr<StmtNode> body, std::string branchId = "")
        : StmtNode(std::move(loc))
        , m_branchCount(std::move(branchCount))
        , m_body(std::move(body))
        , m_branchId(std::move(branchId)) {}

    // Getters
    const ExprNode* getBranchCount() const { return m_branchCount.get(); }
    const StmtNode* getBody() const { return m_body.get(); }
    const std::string& getBranchId() const { return m_branchId; }

    // Visitor pattern support
    void accept(Visitor& visitor) const override { visitor.visit(*this); }

private:
    std::unique_ptr<ExprNode> m_branchCount;  // Number of branches to create
    std::unique_ptr<StmtNode> m_body;         // Code to execute in each branch
    std::string m_branchId;                   // Optional branch identifier
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_BRANCH_TIMELINE_NODE_H
