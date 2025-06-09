#include <sstream>

#include "compiler/IRGenerator.h"

// Helper for std::visit with overloaded lambdas
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace chronovyan {
namespace compiler {

IRGenerator::IRGenerator(DiagnosticReporter &diagnostics)
    : diagnostics(diagnostics), currentFunction(nullptr), currentBlock(nullptr),
      labelCounter(0), hasError(false) {}

IRProgram IRGenerator::generate(AstNode *node) {
  // Reset state
  program = IRProgram();
  hasError = false;
  localVariables.clear();
  while (!loopStack.empty())
    loopStack.pop();

  // Visit the AST to generate IR
  if (node) {
    node->accept(*this);
  }

  return program;
}

void IRGenerator::visitProgramNode(ProgramNode *node) {
  // Visit all declarations in the program
  for (const auto &decl : node->getDeclarations()) {
    if (decl) {
      decl->accept(*this);
    }
  }
}

void IRGenerator::visitBasicTypeNode(BasicTypeNode *node) {
  // Nothing to do for type nodes
}

void IRGenerator::visitLiteralExprNode(LiteralExprNode *node) {
  // Generate IR for a literal expression
  // Simply load the constant onto the stack

  IRValue value;

  const auto &tokenValue = node->getValue();

  // Use std::visit to handle the variant type
  value = std::visit(
      overloaded{[](int i) -> IRValue { return IRValue::createInteger(i); },
                 [](double d) -> IRValue { return IRValue::createFloat(d); },
                 [](const std::string &s) -> IRValue {
                   return IRValue::createString(s);
                 },
                 [](bool b) -> IRValue { return IRValue::createBoolean(b); },
                 [](std::monostate) -> IRValue {
                   // Handle nil value
                   return IRValue::createVoid();
                 }},
      tokenValue);

  // Add the constant to the current function's constant pool
  addInstruction(IROpCode::LOAD_CONST, value, node->getLocation());
}

void IRGenerator::visitVariableExprNode(VariableExprNode *node) {
  // Generate IR for a variable expression
  // Load the variable onto the stack

  // Get the variable name from the token's lexeme
  const std::string &varName = node->getName().getLexeme();
  IRValue varRef = IRValue::createReference(varName);
  addInstruction(IROpCode::LOAD_VAR, varRef, node->getLocation());
}

void IRGenerator::visitUnaryExprNode(UnaryExprNode *node) {
  // Generate IR for a unary expression

  // First, generate code for the operand
  node->getRight().accept(*this);

  // Then, apply the unary operator
  TokenType opType = node->getOperator().getType();
  switch (opType) {
  case TokenType::MINUS:
    addInstruction(IROpCode::NEG, node->getLocation());
    break;
  case TokenType::BANG:
    addInstruction(IROpCode::NOT, node->getLocation());
    break;
  default:
    // Unknown unary operator
    diagnostics.addError(node->getLocation(), "Unknown unary operator");
    hasError = true;
    break;
  }
}

void IRGenerator::visitBinaryExprNode(BinaryExprNode *node) {
  // Generate IR for a binary expression

  // First, generate code for the left operand
  node->getLeft().accept(*this);

  // Then, generate code for the right operand
  node->getRight().accept(*this);

  // Finally, apply the binary operator
  TokenType opType = node->getOperator().getType();
  switch (opType) {
  case TokenType::PLUS:
    addInstruction(IROpCode::ADD, node->getLocation());
    break;
  case TokenType::MINUS:
    addInstruction(IROpCode::SUB, node->getLocation());
    break;
  case TokenType::STAR:
    addInstruction(IROpCode::MUL, node->getLocation());
    break;
  case TokenType::SLASH:
    addInstruction(IROpCode::DIV, node->getLocation());
    break;
  case TokenType::PERCENT:
    addInstruction(IROpCode::MOD, node->getLocation());
    break;
  case TokenType::EQUAL_EQUAL:
    addInstruction(IROpCode::EQ, node->getLocation());
    break;
  case TokenType::BANG_EQUAL:
    addInstruction(IROpCode::NE, node->getLocation());
    break;
  case TokenType::LESS:
    addInstruction(IROpCode::LT, node->getLocation());
    break;
  case TokenType::LESS_EQUAL:
    addInstruction(IROpCode::LE, node->getLocation());
    break;
  case TokenType::GREATER:
    addInstruction(IROpCode::GT, node->getLocation());
    break;
  case TokenType::GREATER_EQUAL:
    addInstruction(IROpCode::GE, node->getLocation());
    break;
  case TokenType::AND:
    addInstruction(IROpCode::AND, node->getLocation());
    break;
  case TokenType::OR:
    addInstruction(IROpCode::OR, node->getLocation());
    break;
  default:
    // Unknown binary operator
    diagnostics.addError(node->getLocation(), "Unknown binary operator");
    hasError = true;
    break;
  }
}

void IRGenerator::visitCallExprNode(CallExprNode *node) {
  // Generate IR for a call expression

  // First, generate code for the callee
  node->getCallee().accept(*this);

  // Then, generate code for the arguments
  for (const auto &arg : node->getArguments()) {
    if (arg) {
      arg->accept(*this);
    }
  }

  // Get the callee name from the callee expression
  std::string calleeName;
  if (auto varExpr = dynamic_cast<VariableExprNode *>(&node->getCallee())) {
    calleeName = varExpr->getName().getLexeme();
  } else {
    // Handle other types of callees (e.g., member access, function pointer,
    // etc.)
    diagnostics.addError(node->getLocation(),
                         "Unsupported callee type in function call");
    hasError = true;
    return;
  }

  // Create a function reference and generate the call instruction
  IRValue calleeValue = IRValue::createFunction(calleeName);
  addInstruction(IROpCode::CALL, calleeValue, node->getLocation());
}

void IRGenerator::visitAssignExprNode(AssignExprNode *node) {
  // Generate IR for an assignment expression

  // First, generate code for the value
  node->getValue().accept(*this);

  // Then, store the value to the variable
  IRValue varRef = IRValue::createReference(node->getName().getLexeme());
  addInstruction(IROpCode::STORE_VAR, varRef, node->getLocation());

  // The assignment expression leaves the value on the stack
  addInstruction(IROpCode::DUP, node->getLocation());
}

void IRGenerator::visitVarDeclNode(VarDeclNode *node) {
  // Generate IR for a variable declaration

  // If there's an initializer, generate code for it
  if (node->getInitializer()) {
    node->getInitializer()->accept(*this);
  } else {
    // No initializer, load nil
    addInstruction(IROpCode::LOAD_CONST, IRValue::createVoid(),
                   node->getLocation());
  }

  // Store the initializer value to the variable
  const std::string &varName = node->getName().getLexeme();
  IRValue varRef = IRValue::createReference(varName);
  addInstruction(IROpCode::STORE_VAR, varRef, node->getLocation());
}

void IRGenerator::visitExprStmtNode(ExprStmtNode *node) {
  // Generate IR for an expression statement

  // Generate code for the expression
  node->getExpression().accept(*this);

  // Pop the result as it's not used
  addInstruction(IROpCode::POP, node->getLocation());
}

void IRGenerator::visitBlockStmtNode(BlockStmtNode *node) {
  // Generate IR for a block statement

  // Generate code for each statement in the block
  for (const auto &stmt : node->getStatements()) {
    if (stmt) {
      stmt->accept(*this);
    }
  }
}

void IRGenerator::visitIfStmtNode(IfStmtNode *node) {
  // Generate IR for an if statement

  // Generate code for the condition
  node->getCondition().accept(*this);

  // Create labels for the else branch and end of the if statement
  std::string elseLabel = generateLabel("else");
  std::string endLabel = generateLabel("endif");

  // Jump to the else branch if the condition is false
  addInstruction(IROpCode::JUMP_IF_FALSE, IRValue::createString(elseLabel),
                 node->getLocation());

  // Generate code for the then branch
  node->getThenBranch().accept(*this);

  // Jump to the end of the if statement
  addInstruction(IROpCode::JUMP, IRValue::createString(endLabel),
                 node->getLocation());

  // Mark the start of the else branch
  IRBasicBlock &elseBlock = createBasicBlock(elseLabel);
  currentBlock = &elseBlock;

  // Generate code for the else branch if present
  if (node->getElseBranch()) {
    node->getElseBranch()->accept(*this);
  }

  // Mark the end of the if statement
  IRBasicBlock &endBlock = createBasicBlock(endLabel);
  currentBlock = &endBlock;
}

void IRGenerator::visitWhileStmtNode(WhileStmtNode *node) {
  // Generate IR for a while statement

  // Create labels for the loop condition, body, and end
  std::string condLabel = generateLabel("while_cond");
  std::string bodyLabel = generateLabel("while_body");
  std::string endLabel = generateLabel("while_end");

  // Push the loop labels onto the stack
  loopStack.push({endLabel, condLabel});

  // Mark the start of the loop condition
  IRBasicBlock &condBlock = createBasicBlock(condLabel);
  currentBlock = &condBlock;

  // Generate code for the condition
  node->getCondition().accept(*this);

  // Jump to the loop body if the condition is true
  addInstruction(IROpCode::JUMP_IF_TRUE, IRValue::createString(bodyLabel),
                 node->getLocation());

  // Jump to the end of the loop if the condition is false
  addInstruction(IROpCode::JUMP, IRValue::createString(endLabel),
                 node->getLocation());

  // Mark the start of the loop body
  IRBasicBlock &bodyBlock = createBasicBlock(bodyLabel);
  currentBlock = &bodyBlock;

  // Generate code for the loop body
  node->getBody().accept(*this);

  // Jump back to the loop condition
  addInstruction(IROpCode::JUMP, IRValue::createString(condLabel),
                 node->getLocation());

  // Mark the end of the loop
  IRBasicBlock &endBlock = createBasicBlock(endLabel);
  currentBlock = &endBlock;

  // Pop the loop labels from the stack
  loopStack.pop();
}

void IRGenerator::visitResourceStmtNode(ResourceStmtNode *node) {
  // Generate IR for a resource statement

  // Generate code for the chronons expression
  if (node->getChronons()) {
    node->getChronons()->accept(*this);
  } else {
    // Default to 0 if no chronons specified
    addInstruction(IROpCode::LOAD_CONST, IRValue::createFloat(0.0),
                   node->getLocation());
  }

  // Generate code for the aethel expression
  if (node->getAethel()) {
    node->getAethel()->accept(*this);
  } else {
    // Default to 0 if no aethel specified
    addInstruction(IROpCode::LOAD_CONST, IRValue::createFloat(0.0),
                   node->getLocation());
  }

  // Check if there are enough resources based on operation type
  switch (node->getOperation()) {
  case ResourceStmtNode::ResourceOperation::ALLOCATE:
    addInstruction(IROpCode::CONSUME_AETHEL, node->getLocation());
    addInstruction(IROpCode::CONSUME_CHRONON, node->getLocation());
    break;
  case ResourceStmtNode::ResourceOperation::EXPEND:
    addInstruction(IROpCode::CONSUME_AETHEL, node->getLocation());
    break;
  case ResourceStmtNode::ResourceOperation::BORROW:
    addInstruction(IROpCode::CONSUME_CHRONON, node->getLocation());
    break;
  case ResourceStmtNode::ResourceOperation::REPAY:
    // Repay operations don't consume resources
    break;
  }

  // Generate code for the body
  node->getBody().accept(*this);
}

void IRGenerator::visitParallelExecutionStmtNode(
    ParallelExecutionStmtNode *node) {
  // Generate IR for a parallel execution statement

  // Generate code for the thread count expression
  node->getThreadCount().accept(*this);

  // Begin parallel execution
  addInstruction(IROpCode::PARALLEL_BEGIN, node->getLocation());

  // Generate code for the body
  node->getBody().accept(*this);

  // End parallel execution
  addInstruction(IROpCode::PARALLEL_END, node->getLocation());
}

void IRGenerator::visitBranchTimelineStmtNode(BranchTimelineStmtNode *node) {
  // Generate IR for a branch timeline statement

  // Generate code for the branch count expression
  node->getBranchCount().accept(*this);

  // Begin timeline branching
  addInstruction(IROpCode::TIMELINE_BRANCH_BEGIN, node->getLocation());

  // Generate code for the body
  node->getBody().accept(*this);

  // End timeline branching
  addInstruction(IROpCode::TIMELINE_BRANCH_END, node->getLocation());
}

void IRGenerator::visitStmtDeclNode(StmtDeclNode *node) {
  // Generate IR for a statement declaration
  // This is a wrapper around a statement that needs to be treated as a
  // declaration

  // Simply visit the contained statement
  node->getStmt().accept(*this);
}

IRBasicBlock &IRGenerator::createBasicBlock(const std::string &label) {
  // Create a new basic block with the given label
  if (!currentFunction) {
    // This should not happen
    diagnostics.addError(SourceLocation(),
                         "Cannot create basic block outside of a function");
    hasError = true;

    // Create a dummy function
    startFunction("__dummy");
  }

  // Create the block
  currentFunction->addBasicBlock(IRBasicBlock(label));
  return currentFunction->getBasicBlock(
      currentFunction->getBasicBlocks().size() - 1);
}

std::string IRGenerator::generateLabel(const std::string &prefix) {
  // Generate a unique label
  std::ostringstream oss;
  oss << prefix << "_" << labelCounter++;
  return oss.str();
}

void IRGenerator::addInstruction(IROpCode opcode,
                                 const SourceLocation &location) {
  // Add an instruction to the current basic block
  if (!currentBlock) {
    // This should not happen
    diagnostics.addError(location,
                         "Cannot add instruction outside of a basic block");
    hasError = true;
    return;
  }

  // Create the instruction
  IRInstruction instruction(opcode, location);

  // Add the instruction to the current block
  currentBlock->addInstruction(instruction);
}

void IRGenerator::addInstruction(IROpCode opcode, const IRValue &operand1,
                                 const SourceLocation &location) {
  // Add an instruction with one operand to the current basic block
  if (!currentBlock) {
    // This should not happen
    diagnostics.addError(location,
                         "Cannot add instruction outside of a basic block");
    hasError = true;
    return;
  }

  // Create the instruction
  IRInstruction instruction(opcode, operand1, location);

  // Add the instruction to the current block
  currentBlock->addInstruction(instruction);
}

void IRGenerator::addInstruction(IROpCode opcode, const IRValue &operand1,
                                 const IRValue &operand2,
                                 const SourceLocation &location) {
  // Add an instruction with two operands to the current basic block
  if (!currentBlock) {
    // This should not happen
    diagnostics.addError(location,
                         "Cannot add instruction outside of a basic block");
    hasError = true;
    return;
  }

  // Create the instruction
  IRInstruction instruction(opcode, operand1, operand2, location);

  // Add the instruction to the current block
  currentBlock->addInstruction(instruction);
}

void IRGenerator::addInstruction(IROpCode opcode, const IRValue &operand1,
                                 const IRValue &operand2,
                                 const IRValue &operand3,
                                 const SourceLocation &location) {
  // Add an instruction with three operands to the current basic block
  if (!currentBlock) {
    // This should not happen
    diagnostics.addError(location,
                         "Cannot add instruction outside of a basic block");
    hasError = true;
    return;
  }

  // Create the instruction
  IRInstruction instruction(opcode, operand1, operand2, operand3, location);

  // Add the instruction to the current block
  currentBlock->addInstruction(instruction);
}

void IRGenerator::startFunction(const std::string &name,
                                const std::vector<std::string> &parameters) {
  // Start a new function
  program.addFunction(IRFunction(name, parameters));
  currentFunction = program.getFunctionByName(name);

  // Create the entry block
  IRBasicBlock &entryBlock = createBasicBlock("entry");
  currentBlock = &entryBlock;

  // Reset the local variables
  localVariables.clear();

  // Add parameters to the local variables
  for (const auto &param : parameters) {
    localVariables[param] = localVariables.size();
  }
}

void IRGenerator::endFunction() {
  // End the current function
  currentFunction = nullptr;
  currentBlock = nullptr;
}

void IRGenerator::generateBinaryOp(BinaryExprNode *node, IROpCode opcode) {
  // Generate code for a binary operation

  // First, generate code for the left operand
  node->getLeft().accept(*this);

  // Then, generate code for the right operand
  node->getRight().accept(*this);

  // Finally, apply the binary operator
  addInstruction(opcode, node->getLocation());
}

void IRGenerator::generateComparisonOp(BinaryExprNode *node, IROpCode opcode) {
  // Generate code for a comparison operation

  // First, generate code for the left operand
  node->getLeft().accept(*this);

  // Then, generate code for the right operand
  node->getRight().accept(*this);

  // Finally, apply the comparison operator
  addInstruction(opcode, node->getLocation());
}

} // namespace compiler
} // namespace chronovyan