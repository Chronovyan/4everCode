#ifndef CHRONOVYAN_INTERPRETER_BYTECODE_INTERPRETER_H
#define CHRONOVYAN_INTERPRETER_BYTECODE_INTERPRETER_H

#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "compiler/bytecode/BytecodeModule.h"
#include "stdlib/paradox_management.h"

namespace chronovyan {
namespace interpreter {

/**
 * @brief Runtime value representation
 */
class Value {
public:
  enum class Type {
    NULL_VALUE,
    BOOLEAN,
    INTEGER,
    FLOAT,
    STRING,
    RESOURCE,
    TIMELINE
  };

  Value() : type(Type::NULL_VALUE), intValue(0) {}
  explicit Value(bool value) : type(Type::BOOLEAN), boolValue(value) {}
  explicit Value(int value) : type(Type::INTEGER), intValue(value) {}
  explicit Value(double value) : type(Type::FLOAT), floatValue(value) {}
  explicit Value(const std::string &value)
      : type(Type::STRING), stringValue(value) {}

  Type getType() const { return type; }
  bool getBoolValue() const { return boolValue; }
  int getIntValue() const { return intValue; }
  double getFloatValue() const { return floatValue; }
  const std::string &getStringValue() const { return stringValue; }

  std::string toString() const;

private:
  Type type;
  union {
    bool boolValue;
    int intValue;
    double floatValue;
  };
  std::string stringValue; // Outside union because it has a constructor
};

/**
 * @brief Exception thrown during bytecode execution
 */
class RuntimeException : public std::runtime_error {
public:
  explicit RuntimeException(const std::string &message)
      : std::runtime_error(message) {}
  virtual ~RuntimeException() = default;
};

/**
 * @brief Execution context for a function
 */
class ExecutionContext {
public:
  ExecutionContext(const std::string &functionName, size_t returnAddress = 0)
      : functionName(functionName), returnAddress(returnAddress),
        instructionPointer(0) {}

  const std::string &getFunctionName() const { return functionName; }
  size_t getReturnAddress() const { return returnAddress; }
  size_t getInstructionPointer() const { return instructionPointer; }
  void setInstructionPointer(size_t ip) { instructionPointer = ip; }
  void incrementInstructionPointer() { instructionPointer++; }

  const std::vector<Value> &getLocalVariables() const { return localVariables; }
  void setLocalVariable(size_t index, const Value &value) {
    if (index >= localVariables.size()) {
      localVariables.resize(index + 1);
    }
    localVariables[index] = value;
  }
  Value getLocalVariable(size_t index) const {
    if (index < localVariables.size()) {
      return localVariables[index];
    }
    return Value(); // Return null value for out-of-bounds access
  }

private:
  std::string functionName;
  size_t returnAddress;
  size_t instructionPointer;
  std::vector<Value> localVariables;
};

/**
 * @brief Bytecode interpreter for executing Chronovyan bytecode
 */
class BytecodeInterpreter {
public:
  BytecodeInterpreter();
  ~BytecodeInterpreter();

  /**
   * @brief Load a bytecode module
   * @param module The bytecode module to load
   */
  void loadModule(const compiler::bytecode::BytecodeModule &module);

  /**
   * @brief Execute the main function of the loaded module
   * @param args Arguments to pass to the main function
   * @return The result of execution
   */
  Value execute(const std::vector<Value> &args = {});

  /**
   * @brief Execute a specific function of the loaded module
   * @param functionName The name of the function to execute
   * @param args Arguments to pass to the function
   * @return The result of execution
   */
  Value executeFunction(const std::string &functionName,
                        const std::vector<Value> &args = {});

  /**
   * @brief Set a global variable
   * @param name The name of the variable
   * @param value The value to set
   */
  void setGlobalVariable(const std::string &name, const Value &value);

  /**
   * @brief Get a global variable
   * @param name The name of the variable
   * @return The value of the variable
   */
  Value getGlobalVariable(const std::string &name) const;

private:
  // The bytecode module being executed
  compiler::bytecode::BytecodeModule module;

  // Global variables
  std::unordered_map<std::string, Value> globalVariables;

  // Execution stack for values
  std::stack<Value> valueStack;

  // Call stack for function calls
  std::stack<ExecutionContext> callStack;

  // Current timeline state
  size_t currentTimeline;

  // Resource tracking
  int aethelResource;
  int chrononResource;

  // Paradox management
  stdlib::ParadoxManager paradoxManager;
  bool paradoxAlertSet;

  /**
   * @brief Execute a single instruction
   * @param instruction The instruction to execute
   * @param context The current execution context
   * @return True if execution should continue, false if execution should stop
   */
  bool
  executeInstruction(const compiler::bytecode::BytecodeInstruction &instruction,
                     ExecutionContext &context);

  /**
   * @brief Execute a temporal operation
   * @param opcode The operation code
   * @param operands The operands for the operation
   * @param context The current execution context
   * @return True if execution should continue, false if execution should stop
   */
  bool executeTemporalOperation(compiler::bytecode::OpCode opcode,
                                const std::vector<uint16_t> &operands,
                                ExecutionContext &context);

  /**
   * @brief Convert a bytecode constant to a runtime value
   * @param constant The bytecode constant
   * @return The runtime value
   */
  Value
  convertConstantToValue(const compiler::bytecode::BytecodeConstant &constant);

  /**
   * @brief Push a value onto the stack
   * @param value The value to push
   */
  void push(const Value &value);

  /**
   * @brief Pop a value from the stack
   * @return The popped value
   */
  Value pop();

  /**
   * @brief Check if the stack has enough values
   * @param count The number of values needed
   * @return True if the stack has enough values, false otherwise
   */
  bool checkStackSize(size_t count);

  /**
   * @brief Check if enough resources are available
   * @param aethel The amount of aethel needed
   * @param chronon The amount of chronon needed
   * @return True if enough resources are available, false otherwise
   */
  bool checkResources(int aethel, int chronon);

  /**
   * @brief Consume resources
   * @param aethel The amount of aethel to consume
   * @param chronon The amount of chronon to consume
   */
  void consumeResources(int aethel, int chronon);

  /**
   * @brief Update paradox level based on an operation
   * @param opcode The operation performed
   * @param paradoxImpact The base impact on the paradox level
   */
  void updateParadoxLevel(compiler::bytecode::OpCode opcode,
                          double paradoxImpact);

  /**
   * @brief Set up paradox alerts
   */
  void setupParadoxAlerts();

  /**
   * @brief Handle paradox alert callback
   * @param level The current paradox level
   */
  void handleParadoxAlert(stdlib::PARADOX_LEVEL level);
};

} // namespace interpreter
} // namespace chronovyan

#endif // CHRONOVYAN_INTERPRETER_BYTECODE_INTERPRETER_H