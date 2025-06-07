#include "AstNode.h"
#include "AstVisitor.h"

namespace chronovyan {
namespace compiler {

void ParallelExecutionStmtNode::accept(AstVisitor& visitor) {
    visitor.visitParallelExecutionStmtNode(this);
}

void BranchTimelineStmtNode::accept(AstVisitor& visitor) {
    visitor.visitBranchTimelineStmtNode(this);
}

}  // namespace compiler
}  // namespace chronovyan