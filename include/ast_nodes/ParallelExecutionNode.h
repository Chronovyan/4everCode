#ifndef CHRONOVYAN_PARALLEL_EXECUTION_NODE_H
#define CHRONOVYAN_PARALLEL_EXECUTION_NODE_H

#include <memory>

#include "ast_nodes/ExprNode.h"
#include "ast_nodes/StmtNode.h"

namespace chronovyan {

class ParallelExecutionNode : public StmtNode {
public:
    ParallelExecutionNode(SourceLocation loc, int threadCount, std::unique_ptr<StmtNode> body)
        : StmtNode(std::move(loc)), m_threadCount(threadCount), m_body(std::move(body)) {}

    // Getters
    int getThreadCount() const { return m_threadCount; }
    const StmtNode* getBody() const { return m_body.get(); }

    // Visitor pattern support
    void accept(Visitor& visitor) const override { visitor.visit(*this); }

private:
    int m_threadCount;                 // Number of threads to use
    std::unique_ptr<StmtNode> m_body;  // Code to execute in parallel
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_PARALLEL_EXECUTION_NODE_H
