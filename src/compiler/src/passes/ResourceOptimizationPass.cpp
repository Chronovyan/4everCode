#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "compiler/passes/ResourceOptimizationPass.h"

namespace chronovyan {
namespace compiler {

ResourceOptimizationPass::ResourceOptimizationPass(
    DiagnosticReporter &diagnostics)
    : diagnostics(diagnostics) {}

std::string ResourceOptimizationPass::getName() const {
  return "ResourceOptimization";
}

std::string ResourceOptimizationPass::getDescription() const {
  return "Optimizes resource usage in temporal operations by coalescing, "
         "reordering, and "
         "eliminating redundant operations.";
}

void ResourceOptimizationPass::run(ProgramNode *program) {
  // Reset internal state
  resourceUsageMap.clear();

  // Process the program
  if (program) {
    program->accept(*this);
  }

  // Analyze resource usage and apply optimizations
  optimizeResourceUsage(program);
}

void ResourceOptimizationPass::optimizeResourceUsage(ProgramNode *program) {
  // Identify functions that could benefit from temporal or parallel
  // optimizations
  for (auto &decl : program->getDeclarations()) {
    if (auto *funcDecl = dynamic_cast<FunctionDeclNode *>(decl.get())) {
      // Check if this function uses resources that could benefit from
      // optimization
      ResourceUsage funcUsage = getResourceUsage(funcDecl);

      // If the function uses temporal resources, check if we can optimize it
      if (funcUsage.usesTemporalOperations) {
        optimizeTemporalOperations(funcDecl);
      }

      // If the function uses parallel resources, check if we can optimize it
      if (funcUsage.usesParallelOperations) {
        optimizeParallelOperations(funcDecl);
      }
    }
  }
}

void ResourceOptimizationPass::optimizeTemporalOperations(DeclNode *node) {
  // Check if this is a function declaration
  if (auto *funcDecl = dynamic_cast<FunctionDeclNode *>(node)) {
    // Analyze the function body for temporal operations
    if (auto *stmtDecl = dynamic_cast<StmtDeclNode *>(funcDecl)) {
      if (stmtDecl->getBody()) {
        optimizeTemporalOperations(stmtDecl->getBody().get());
      }
    }
  }
  // Otherwise, check if this is a statement
  else if (auto *stmt = dynamic_cast<StmtNode *>(node)) {
    optimizeTemporalOperations(stmt);
  }
}

void ResourceOptimizationPass::optimizeTemporalOperations(StmtNode *node) {
  // Implement temporal operation optimizations
  // This would involve analyzing resource usage patterns and
  // restructuring code to optimize temporal operations
}

void ResourceOptimizationPass::optimizeParallelOperations(DeclNode *node) {
  // Similar to optimizeTemporalOperations, but focused on parallel
  // optimizations
}

ResourceUsage ResourceOptimizationPass::getResourceUsage(AstNode *node) {
  // Check if we've already computed resource usage for this node
  auto it = resourceUsageMap.find(node);
  if (it != resourceUsageMap.end()) {
    return it->second;
  }

  // Default resource usage
  return ResourceUsage();
}

void ResourceOptimizationPass::visitProgramNode(ProgramNode *node) {
  // Visit all declarations in the program
  for (auto &decl : node->getDeclarations()) {
    if (decl) {
      decl->accept(*this);
    }
  }
}

void ResourceOptimizationPass::visitBasicTypeNode(BasicTypeNode *node) {
  // Types don't use resources
  resourceUsageMap[node] = ResourceUsage();
}

void ResourceOptimizationPass::visitLiteralExprNode(LiteralExprNode *node) {
  // Literals don't use resources
  resourceUsageMap[node] = ResourceUsage();
}

void ResourceOptimizationPass::visitVariableExprNode(VariableExprNode *node) {
  // Variables might use resources indirectly, but we can't determine that here
  resourceUsageMap[node] = ResourceUsage();
}

void ResourceOptimizationPass::visitUnaryExprNode(UnaryExprNode *node) {
  // Visit the operand first
  node->getRight().accept(*this);

  // Get the operand's resource usage
  ResourceUsage operandUsage = resourceUsageMap[&node->getRight()];

  // Unary expressions inherit resource usage from their operand
  resourceUsageMap[node] = operandUsage;
}

void ResourceOptimizationPass::visitBinaryExprNode(BinaryExprNode *node) {
  // Visit the operands first
  node->getLeft().accept(*this);
  node->getRight().accept(*this);

  // Get the operands' resource usage
  ResourceUsage leftUsage = resourceUsageMap[&node->getLeft()];
  ResourceUsage rightUsage = resourceUsageMap[&node->getRight()];

  // Combine resource usage from both operands
  ResourceUsage combinedUsage;
  combinedUsage.usesTemporalOperations =
      leftUsage.usesTemporalOperations || rightUsage.usesTemporalOperations;
  combinedUsage.usesParallelOperations =
      leftUsage.usesParallelOperations || rightUsage.usesParallelOperations;

  resourceUsageMap[node] = combinedUsage;
}

void ResourceOptimizationPass::visitCallExprNode(CallExprNode *node) {
  // Visit the callee first
  node->getCallee().accept(*this);

  // Visit all arguments
  for (auto &arg : node->getArguments()) {
    if (arg) {
      arg->accept(*this);
    }
  }

  // Get the callee's resource usage
  ResourceUsage calleeUsage = resourceUsageMap[&node->getCallee()];

  // Combine with arguments' resource usage
  for (auto &arg : node->getArguments()) {
    if (arg) {
      ResourceUsage argUsage = resourceUsageMap[arg.get()];
      calleeUsage.usesTemporalOperations |= argUsage.usesTemporalOperations;
      calleeUsage.usesParallelOperations |= argUsage.usesParallelOperations;
    }
  }

  // Check if this is a call to a temporal or parallel function
  std::string funcName;
  if (auto *varExpr = dynamic_cast<VariableExprNode *>(&node->getCallee())) {
    funcName = varExpr->getName().getLexeme();
  }

  // Known temporal functions
  static const std::unordered_set<std::string> temporalFunctions = {
      "rewind", "branch", "merge", "snapshot"};

  // Known parallel functions
  static const std::unordered_set<std::string> parallelFunctions = {
      "parallel", "thread_id", "thread_count"};

  if (temporalFunctions.find(funcName) != temporalFunctions.end()) {
    calleeUsage.usesTemporalOperations = true;
  }

  if (parallelFunctions.find(funcName) != parallelFunctions.end()) {
    calleeUsage.usesParallelOperations = true;
  }

  resourceUsageMap[node] = calleeUsage;
}

void ResourceOptimizationPass::visitAssignExprNode(AssignExprNode *node) {
  // Visit the value first
  node->getValue().accept(*this);

  // Get the value's resource usage
  ResourceUsage valueUsage = resourceUsageMap[&node->getValue()];

  // Assignment expressions inherit resource usage from their value
  resourceUsageMap[node] = valueUsage;
}

void ResourceOptimizationPass::visitVarDeclNode(VarDeclNode *node) {
  // Visit the initializer if present
  if (node->getInitializer()) {
    node->getInitializer()->accept(*this);

    // Get the initializer's resource usage
    ResourceUsage initUsage = resourceUsageMap[node->getInitializer().get()];

    // Variable declarations inherit resource usage from their initializer
    resourceUsageMap[node] = initUsage;
  } else {
    // Variables without initializers don't use resources directly
    resourceUsageMap[node] = ResourceUsage();
  }
}

void ResourceOptimizationPass::visitExprStmtNode(ExprStmtNode *node) {
  // Visit the expression
  const auto &expr = node->getExpression();
  if (&expr) {
    const_cast<ExprNode &>(expr).accept(*this);

    // Expression statements inherit resource usage from their expression
    resourceUsageMap[node] = resourceUsageMap[&node->getExpression()];
  } else {
    // Empty expression statements don't use resources
    resourceUsageMap[node] = ResourceUsage();
  }
}

void ResourceOptimizationPass::visitBlockStmtNode(BlockStmtNode *node) {
  // Visit all statements in the block
  ResourceUsage blockUsage;

  for (auto &stmt : node->getStatements()) {
    if (stmt) {
      stmt->accept(*this);

      // Combine resource usage from all statements
      ResourceUsage stmtUsage = resourceUsageMap[stmt.get()];
      blockUsage.usesTemporalOperations |= stmtUsage.usesTemporalOperations;
      blockUsage.usesParallelOperations |= stmtUsage.usesParallelOperations;
    }
  }

  resourceUsageMap[node] = blockUsage;
}

void ResourceOptimizationPass::visitIfStmtNode(IfStmtNode *node) {
  // Visit the condition first
  node->getCondition().accept(*this);

  // Visit the then branch
  node->getThenBranch().accept(*this);

  // Visit the else branch if present
  ResourceUsage elseUsage;
  if (node->getElseBranch()) {
    node->getElseBranch()->accept(*this);
    elseUsage = resourceUsageMap[node->getElseBranch().get()];
  }

  // Combine resource usage from condition, then branch, and else branch
  ResourceUsage condUsage = resourceUsageMap[&node->getCondition()];
  ResourceUsage thenUsage = resourceUsageMap[&node->getThenBranch()];

  ResourceUsage combinedUsage;
  combinedUsage.usesTemporalOperations = condUsage.usesTemporalOperations ||
                                         thenUsage.usesTemporalOperations ||
                                         elseUsage.usesTemporalOperations;
  combinedUsage.usesParallelOperations = condUsage.usesParallelOperations ||
                                         thenUsage.usesParallelOperations ||
                                         elseUsage.usesParallelOperations;

  resourceUsageMap[node] = combinedUsage;
}

void ResourceOptimizationPass::visitWhileStmtNode(WhileStmtNode *node) {
  // Visit the condition first
  node->getCondition().accept(*this);

  // Visit the body
  node->getBody().accept(*this);

  // Combine resource usage from condition and body
  ResourceUsage condUsage = resourceUsageMap[&node->getCondition()];
  ResourceUsage bodyUsage = resourceUsageMap[&node->getBody()];

  ResourceUsage combinedUsage;
  combinedUsage.usesTemporalOperations =
      condUsage.usesTemporalOperations || bodyUsage.usesTemporalOperations;
  combinedUsage.usesParallelOperations =
      condUsage.usesParallelOperations || bodyUsage.usesParallelOperations;

  resourceUsageMap[node] = combinedUsage;
}

void ResourceOptimizationPass::visitResourceStmtNode(ResourceStmtNode *node) {
  // Resource statements explicitly use temporal resources
  ResourceUsage usage;
  usage.usesTemporalOperations = true;

  // Visit resource expressions
  if (node->getChronons()) {
    node->getChronons()->accept(*this);
    ResourceUsage chrononsUsage = resourceUsageMap[node->getChronons().get()];
    usage.usesParallelOperations |= chrononsUsage.usesParallelOperations;
  }

  if (node->getAethel()) {
    node->getAethel()->accept(*this);
    ResourceUsage aethelUsage = resourceUsageMap[node->getAethel().get()];
    usage.usesParallelOperations |= aethelUsage.usesParallelOperations;
  }

  // Visit the body
  node->getBody().accept(*this);
  ResourceUsage bodyUsage = resourceUsageMap[&node->getBody()];
  usage.usesParallelOperations |= bodyUsage.usesParallelOperations;

  resourceUsageMap[node] = usage;
}

void ResourceOptimizationPass::visitStmtDeclNode(StmtDeclNode *node) {
  // Visit the body
  if (node->getBody()) {
    node->getBody()->accept(*this);

    // Use the resource usage of the body
    resourceUsageMap[node] = resourceUsageMap[node->getBody()];
  } else {
    // Default usage for an empty function
    resourceUsageMap[node] = ResourceUsage(0.0, 0.0, true);
  }
}

ResourceOptimizationPass::ResourceUsage
ResourceOptimizationPass::calculateResourceUsage(AstNode *node) {
  // Check if we already have the resource usage for this node
  auto it = resourceUsageMap.find(node);
  if (it != resourceUsageMap.end()) {
    return it->second;
  }

  // Default usage
  return ResourceUsage(1.0, 1.0, false);
}

bool ResourceOptimizationPass::tryOptimizeResourceStmt(ResourceStmtNode *node) {
  // For now, just return false to indicate no optimization was applied
  return false;
}

bool ResourceOptimizationPass::optimizeTemporalOperations(BlockStmtNode *node) {
  // Check for temporal operations that can be optimized
  bool optimized = false;

  // For simplicity, just check for consecutive resource statements that can be
  // coalesced
  std::vector<AstNode *> temporalOps;

  // Collect all temporal operations in the block
  for (const auto &stmt : node->getStatements()) {
    if (stmt && isTemporalOperation(stmt.get())) {
      temporalOps.push_back(stmt.get());
    }
  }

  // Check for consecutive operations that can be coalesced
  for (size_t i = 0; i < temporalOps.size() - 1; ++i) {
    for (size_t j = i + 1; j < temporalOps.size(); ++j) {
      if (canCoalesceOperations(temporalOps[i], temporalOps[j])) {
        // In a real implementation, we would coalesce the operations here
        // and update the AST accordingly

        // Add a diagnostic
        if (diagnostics) {
          SourceLocation location = temporalOps[i]->getLocation();
          diagnostics->addInfo(
              location,
              "Coalesced temporal operations to reduce resource usage");
        }

        optimized = true;

        // For now, just break after the first optimization
        break;
      }
    }

    if (optimized) {
      break;
    }
  }

  return optimized;
}

bool ResourceOptimizationPass::isTemporalOperation(AstNode *node) {
  // Check if this is a temporal operation
  // For now, just check if it's a resource statement
  return dynamic_cast<ResourceStmtNode *>(node) != nullptr;
}

ResourceOptimizationPass::ResourceUsage
ResourceOptimizationPass::getTemporalOperationCost(AstNode *node) {
  // Get the cost of a temporal operation
  return calculateResourceUsage(node);
}

bool ResourceOptimizationPass::canCoalesceOperations(AstNode *first,
                                                     AstNode *second) {
  // Check if two temporal operations can be coalesced
  // For now, just return false to indicate no coalescing is possible
  return false;
}

AstNode *ResourceOptimizationPass::coalesceOperations(AstNode *first,
                                                      AstNode *second) {
  // Coalesce two temporal operations
  // For now, just return nullptr to indicate coalescing failed
  return nullptr;
}

void ResourceOptimizationPass::visitParallelExecutionStmtNode(
    ParallelExecutionStmtNode *node) {
  // Visit the thread count expression
  node->getThreadCount().accept(*this);

  // Resource usage for thread count
  ResourceUsage threadCountUsage = resourceUsageMap[&node->getThreadCount()];

  // Visit the body
  node->getBody().accept(*this);

  // Resource usage for body
  ResourceUsage bodyUsage = resourceUsageMap[&node->getBody()];

  // Calculate the total resource usage for this node
  ResourceUsage nodeUsage;
  nodeUsage.chronons = threadCountUsage.chronons + bodyUsage.chronons;
  nodeUsage.aethel = threadCountUsage.aethel + bodyUsage.aethel;

  // Store resource usage for this node
  resourceUsageMap[node] = nodeUsage;
}

void ResourceOptimizationPass::visitBranchTimelineStmtNode(
    BranchTimelineStmtNode *node) {
  // Visit the branch count expression
  node->getBranchCount().accept(*this);

  // Resource usage for branch count
  ResourceUsage branchCountUsage = resourceUsageMap[&node->getBranchCount()];

  // Visit the body
  node->getBody().accept(*this);

  // Resource usage for body
  ResourceUsage bodyUsage = resourceUsageMap[&node->getBody()];

  // Calculate the total resource usage for this node
  ResourceUsage nodeUsage;
  nodeUsage.chronons =
      branchCountUsage.chronons +
      bodyUsage.chronons * 2; // Timeline branches are expensive
  nodeUsage.aethel = branchCountUsage.aethel + bodyUsage.aethel * 2;

  // Store resource usage for this node
  resourceUsageMap[node] = nodeUsage;
}

} // namespace compiler
} // namespace chronovyan