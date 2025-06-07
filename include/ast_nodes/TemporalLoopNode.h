#ifndef CHRONOVYAN_TEMPORAL_LOOP_NODE_H
#define CHRONOVYAN_TEMPORAL_LOOP_NODE_H

#include <memory>

#include "ast_nodes/ExprNode.h"
#include "ast_nodes/StmtNode.h"

namespace chronovyan {

class TemporalLoopNode : public StmtNode {
public:
    TemporalLoopNode(SourceLocation loc, std::unique_ptr<ExprNode> duration,
                     std::unique_ptr<ExprNode> iterations, std::unique_ptr<StmtNode> body)
        : StmtNode(std::move(loc))
        , m_duration(std::move(duration))
        , m_iterations(std::move(iterations))
        , m_body(std::move(body)) {}

    // Getters
    const ExprNode* getDuration() const { return m_duration.get(); }
    const ExprNode* getIterations() const { return m_iterations.get(); }
    const StmtNode* getBody() const { return m_body.get(); }

    // Visitor pattern support
    void accept(Visitor& visitor) const override { visitor.visit(*this); }

private:
    std::unique_ptr<ExprNode> m_duration;    // Duration in milliseconds
    std::unique_ptr<ExprNode> m_iterations;  // Number of iterations
    std::unique_ptr<StmtNode> m_body;        // Loop body
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_TEMPORAL_LOOP_NODE_H
