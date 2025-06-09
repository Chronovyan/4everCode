#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "AstNode.h"
#include "SourceLocation.h"

namespace chronovyan {
namespace compiler {

/**
 * @brief Enumeration of IR operation codes
 */
enum class IROpCode {
  // Control flow
  NOP,           ///< No operation
  LABEL,         ///< Label definition
  JUMP,          ///< Unconditional jump
  JUMP_IF_TRUE,  ///< Jump if condition is true
  JUMP_IF_FALSE, ///< Jump if condition is false
  CALL,          ///< Function call
  RETURN,        ///< Return from function

  // Stack and memory operations
  LOAD_CONST, ///< Load constant onto stack
  LOAD_VAR,   ///< Load variable onto stack
  STORE_VAR,  ///< Store top of stack to variable
  PUSH,       ///< Push value onto stack
  POP,        ///< Pop value from stack
  DUP,        ///< Duplicate top stack value
  SWAP,       ///< Swap top two stack values

  // Arithmetic operations
  ADD, ///< Addition
  SUB, ///< Subtraction
  MUL, ///< Multiplication
  DIV, ///< Division
  MOD, ///< Modulo
  NEG, ///< Negation

  // Logical operations
  AND, ///< Logical AND
  OR,  ///< Logical OR
  NOT, ///< Logical NOT

  // Comparison operations
  EQ, ///< Equal to
  NE, ///< Not equal to
  LT, ///< Less than
  LE, ///< Less than or equal to
  GT, ///< Greater than
  GE, ///< Greater than or equal to

  // Temporal operations
  REWIND,   ///< Rewind timeline
  BRANCH,   ///< Branch timeline
  MERGE,    ///< Merge timelines
  SNAPSHOT, ///< Create timeline snapshot

  // Resource operations
  CONSUME_AETHEL,  ///< Consume aethel resource
  CONSUME_CHRONON, ///< Consume chronon resource
  CHECK_RESOURCES, ///< Check if enough resources are available

  // Parallel execution operations
  PARALLEL_BEGIN, ///< Begin parallel execution
  PARALLEL_END,   ///< End parallel execution
  THREAD_ID,      ///< Get current thread ID
  THREAD_COUNT,   ///< Get total thread count

  // Timeline branching operations
  TIMELINE_BRANCH_BEGIN, ///< Begin timeline branching
  TIMELINE_BRANCH_END,   ///< End timeline branching
  TIMELINE_ID,           ///< Get current timeline ID
  TIMELINE_COUNT,        ///< Get total timeline count

  // Debug operations
  DEBUG_PRINT, ///< Print debug information
};

/**
 * @brief Type of values in the IR
 */
enum class IRValueType {
  VOID,
  BOOLEAN,
  INTEGER,
  FLOAT,
  STRING,
  REFERENCE, ///< Reference to a variable
  FUNCTION,  ///< Function reference
  RESOURCE,  ///< Resource type
  TIMELINE,  ///< Timeline type
  THREAD,    ///< Thread type
  UNKNOWN
};

/**
 * @brief Represents a value in the IR
 */
class IRValue {
public:
  using ValueVariant =
      std::variant<std::monostate, // VOID
                   bool,           // BOOLEAN
                   int64_t,        // INTEGER
                   double,         // FLOAT
                   std::string,    // STRING or REFERENCE or FUNCTION
                   void *          // For future use
                   >;

  IRValue() : type(IRValueType::VOID), value(std::monostate{}) {}

  explicit IRValue(bool val) : type(IRValueType::BOOLEAN), value(val) {}
  explicit IRValue(int64_t val) : type(IRValueType::INTEGER), value(val) {}
  explicit IRValue(double val) : type(IRValueType::FLOAT), value(val) {}
  explicit IRValue(const std::string &val,
                   IRValueType strType = IRValueType::STRING)
      : type(strType), value(val) {}

  IRValueType getType() const { return type; }
  ValueVariant getValue() const { return value; }

  // Helper methods to get specific value types
  bool getBoolValue() const {
    assert(type == IRValueType::BOOLEAN);
    return std::get<bool>(value);
  }

  int64_t getIntValue() const {
    assert(type == IRValueType::INTEGER);
    return std::get<int64_t>(value);
  }

  double getFloatValue() const {
    assert(type == IRValueType::FLOAT);
    return std::get<double>(value);
  }

  std::string getStringValue() const {
    assert(type == IRValueType::STRING || type == IRValueType::REFERENCE ||
           type == IRValueType::FUNCTION);
    return std::get<std::string>(value);
  }

  // Static factory methods for creating IRValues
  static IRValue createVoid() { return IRValue(); }
  static IRValue createBoolean(bool val) { return IRValue(val); }
  static IRValue createInteger(int64_t val) { return IRValue(val); }
  static IRValue createFloat(double val) { return IRValue(val); }
  static IRValue createString(const std::string &val) {
    return IRValue(val, IRValueType::STRING);
  }
  static IRValue createReference(const std::string &name) {
    return IRValue(name, IRValueType::REFERENCE);
  }
  static IRValue createFunction(const std::string &name) {
    return IRValue(name, IRValueType::FUNCTION);
  }

  // For debugging
  std::string toString() const;

private:
  IRValueType type;
  ValueVariant value;
};

/**
 * @brief Represents a single instruction in the IR
 */
class IRInstruction {
public:
  IRInstruction(IROpCode opcode,
                const SourceLocation &location = SourceLocation())
      : opcode(opcode), location(location) {}

  IRInstruction(IROpCode opcode, const IRValue &operand1,
                const SourceLocation &location = SourceLocation())
      : opcode(opcode), operands({operand1}), location(location) {}

  IRInstruction(IROpCode opcode, const IRValue &operand1,
                const IRValue &operand2,
                const SourceLocation &location = SourceLocation())
      : opcode(opcode), operands({operand1, operand2}), location(location) {}

  IRInstruction(IROpCode opcode, const IRValue &operand1,
                const IRValue &operand2, const IRValue &operand3,
                const SourceLocation &location = SourceLocation())
      : opcode(opcode), operands({operand1, operand2, operand3}),
        location(location) {}

  IROpCode getOpcode() const { return opcode; }
  const std::vector<IRValue> &getOperands() const { return operands; }
  const SourceLocation &getLocation() const { return location; }

  // Helper to add an operand
  void addOperand(const IRValue &operand) { operands.push_back(operand); }

  // For debugging
  std::string toString() const;

private:
  IROpCode opcode;
  std::vector<IRValue> operands;
  SourceLocation location;
};

/**
 * @brief Represents a basic block in the IR
 */
class IRBasicBlock {
public:
  explicit IRBasicBlock(const std::string &label = "") : label(label) {}

  const std::string &getLabel() const { return label; }
  const std::vector<IRInstruction> &getInstructions() const {
    return instructions;
  }

  void setLabel(const std::string &newLabel) { label = newLabel; }

  void addInstruction(const IRInstruction &instruction) {
    instructions.push_back(instruction);
  }

  // For debugging
  std::string toString() const;

private:
  std::string label;
  std::vector<IRInstruction> instructions;
};

/**
 * @brief Represents a function in the IR
 */
class IRFunction {
public:
  IRFunction(const std::string &name,
             const std::vector<std::string> &parameters = {})
      : name(name), parameters(parameters) {}

  const std::string &getName() const { return name; }
  const std::vector<std::string> &getParameters() const { return parameters; }
  const std::vector<IRBasicBlock> &getBasicBlocks() const {
    return basicBlocks;
  }

  void addBasicBlock(const IRBasicBlock &block) {
    basicBlocks.push_back(block);
  }

  void addBasicBlock(IRBasicBlock &&block) {
    basicBlocks.push_back(std::move(block));
  }

  // Get a basic block by index
  IRBasicBlock &getBasicBlock(size_t index) {
    assert(index < basicBlocks.size());
    return basicBlocks[index];
  }

  // For debugging
  std::string toString() const;

private:
  std::string name;
  std::vector<std::string> parameters;
  std::vector<IRBasicBlock> basicBlocks;
};

/**
 * @brief Represents the entire program in IR form
 */
class IRProgram {
public:
  IRProgram() = default;

  const std::vector<IRFunction> &getFunctions() const { return functions; }

  void addFunction(const IRFunction &function) {
    functions.push_back(function);
  }

  void addFunction(IRFunction &&function) {
    functions.push_back(std::move(function));
  }

  // Get a function by name
  IRFunction *getFunctionByName(const std::string &name) {
    for (auto &function : functions) {
      if (function.getName() == name) {
        return &function;
      }
    }
    return nullptr;
  }

  // For debugging
  std::string toString() const;

private:
  std::vector<IRFunction> functions;
};

} // namespace compiler
} // namespace chronovyan