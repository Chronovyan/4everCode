#include <functional>
#include <iostream>
#include <sstream>

#include "interpreter/BytecodeInterpreter.h"

namespace chronovyan {
namespace interpreter {

// Value implementation
std::string Value::toString() const {
  switch (type) {
  case Type::NULL_VALUE:
    return "null";
  case Type::BOOLEAN:
    return boolValue ? "true" : "false";
  case Type::INTEGER:
    return std::to_string(intValue);
  case Type::FLOAT:
    return std::to_string(floatValue);
  case Type::STRING:
    return "\"" + stringValue + "\"";
  case Type::RESOURCE:
    return "[resource]";
  case Type::TIMELINE:
    return "[timeline]";
  default:
    return "[unknown]";
  }
}

// BytecodeInterpreter implementation
BytecodeInterpreter::BytecodeInterpreter()
    : currentTimeline(0), aethelResource(1000), chrononResource(1000),
      paradoxAlertSet(false) {
  // Initialize paradox management
  setupParadoxAlerts();
}

BytecodeInterpreter::~BytecodeInterpreter() {
  // Clean up resources if needed
}

void BytecodeInterpreter::loadModule(
    const compiler::bytecode::BytecodeModule &mod) {
  module = mod;

  // Clear state
  globalVariables.clear();
  while (!valueStack.empty())
    valueStack.pop();
  while (!callStack.empty())
    callStack.pop();

  // Reset resources
  aethelResource = 1000;
  chrononResource = 1000;

  // Reset timeline
  currentTimeline = 0;
}

Value BytecodeInterpreter::execute(const std::vector<Value> &args) {
  // Look for a main function
  auto mainFunction = module.getFunction("main");
  if (!mainFunction) {
    throw RuntimeException("No 'main' function found in module");
  }

  return executeFunction("main", args);
}

Value BytecodeInterpreter::executeFunction(const std::string &functionName,
                                           const std::vector<Value> &args) {
  // Find the function
  auto function = module.getFunction(functionName);
  if (!function) {
    throw RuntimeException("Function '" + functionName + "' not found");
  }

  // Check parameter count
  if (args.size() != function->getParameterCount()) {
    throw RuntimeException("Function '" + functionName + "' expects " +
                           std::to_string(function->getParameterCount()) +
                           " arguments, but " + std::to_string(args.size()) +
                           " were provided");
  }

  // Create execution context
  size_t returnAddress = 0;
  if (!callStack.empty()) {
    returnAddress = callStack.top().getInstructionPointer() + 1;
  }

  ExecutionContext context(functionName, returnAddress);

  // Set up parameters as local variables
  for (size_t i = 0; i < args.size(); i++) {
    context.setLocalVariable(i, args[i]);
  }

  // Push context onto call stack
  callStack.push(context);

  // Execute instructions
  const auto &instructions = function->getInstructions();
  while (context.getInstructionPointer() < instructions.size()) {
    size_t ip = context.getInstructionPointer();
    const auto &instruction = instructions[ip];

    if (!executeInstruction(instruction, context)) {
      break;
    }

    // Update context
    context = callStack.top();
  }

  // If we have a return value on the stack, return it
  if (!valueStack.empty()) {
    Value result = pop();
    return result;
  }

  // Otherwise return null
  return Value();
}

bool BytecodeInterpreter::executeInstruction(
    const compiler::bytecode::BytecodeInstruction &instruction,
    ExecutionContext &context) {
  using namespace compiler::bytecode;

  const auto opcode = instruction.getOpcode();
  const auto &operands = instruction.getOperands();

  // Default is to increment the instruction pointer for the next instruction
  context.incrementInstructionPointer();

  switch (opcode) {
  case OpCode::NOP:
    // Do nothing
    break;

  case OpCode::PUSH_CONST: {
    // Push a constant onto the stack
    if (operands.empty()) {
      throw RuntimeException("PUSH_CONST requires an operand");
    }

    size_t constantIndex = operands[0];
    if (constantIndex >= module.getConstantPoolSize()) {
      throw RuntimeException("Constant index out of range: " +
                             std::to_string(constantIndex));
    }

    const auto &constant = module.getConstantPool()[constantIndex];
    push(convertConstantToValue(constant));
    break;
  }

  case OpCode::PUSH_VAR: {
    // Push a variable value onto the stack
    if (operands.empty()) {
      throw RuntimeException("PUSH_VAR requires an operand");
    }

    size_t varIndex = operands[0];
    push(context.getLocalVariable(varIndex));
    break;
  }

  case OpCode::POP: {
    // Pop a value from the stack
    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }
    pop();
    break;
  }

  case OpCode::DUP: {
    // Duplicate the top value on the stack
    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }

    Value value = pop();
    push(value);
    push(value);
    break;
  }

  case OpCode::SWAP: {
    // Swap the top two values on the stack
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val1 = pop();
    Value val2 = pop();
    push(val1);
    push(val2);
    break;
  }

  case OpCode::LOAD: {
    // Load a variable onto the stack
    if (operands.empty()) {
      throw RuntimeException("LOAD requires an operand");
    }

    size_t varIndex = operands[0];
    push(context.getLocalVariable(varIndex));
    break;
  }

  case OpCode::STORE: {
    // Store a value to a variable
    if (operands.empty()) {
      throw RuntimeException("STORE requires an operand");
    }

    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }

    size_t varIndex = operands[0];
    Value value = pop();
    context.setLocalVariable(varIndex, value);
    break;
  }

  case OpCode::ADD: {
    // Add two values
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      push(Value(val1.getIntValue() + val2.getIntValue()));
    } else if (val1.getType() == Value::Type::FLOAT ||
               val2.getType() == Value::Type::FLOAT) {
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 + f2));
    } else if (val1.getType() == Value::Type::STRING ||
               val2.getType() == Value::Type::STRING) {
      // String concatenation
      push(Value(val1.toString() + val2.toString()));
    } else {
      throw RuntimeException("Unsupported operand types for ADD");
    }
    break;
  }

  case OpCode::SUB: {
    // Subtract two values
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      push(Value(val1.getIntValue() - val2.getIntValue()));
    } else if (val1.getType() == Value::Type::FLOAT ||
               val2.getType() == Value::Type::FLOAT) {
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 - f2));
    } else {
      throw RuntimeException("Unsupported operand types for SUB");
    }
    break;
  }

  case OpCode::MUL: {
    // Multiply two values
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      push(Value(val1.getIntValue() * val2.getIntValue()));
    } else if (val1.getType() == Value::Type::FLOAT ||
               val2.getType() == Value::Type::FLOAT) {
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 * f2));
    } else {
      throw RuntimeException("Unsupported operand types for MUL");
    }
    break;
  }

  case OpCode::DIV: {
    // Divide two values
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      if (val2.getIntValue() == 0) {
        throw RuntimeException("Division by zero");
      }
      push(Value(val1.getIntValue() / val2.getIntValue()));
    } else if (val1.getType() == Value::Type::FLOAT ||
               val2.getType() == Value::Type::FLOAT) {
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();

      if (f2 == 0.0) {
        throw RuntimeException("Division by zero");
      }

      push(Value(f1 / f2));
    } else {
      throw RuntimeException("Unsupported operand types for DIV");
    }
    break;
  }

  case OpCode::MOD: {
    // Modulo two values
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      if (val2.getIntValue() == 0) {
        throw RuntimeException("Modulo by zero");
      }
      push(Value(val1.getIntValue() % val2.getIntValue()));
    } else {
      throw RuntimeException("Unsupported operand types for MOD");
    }
    break;
  }

  case OpCode::NEG: {
    // Negate a value
    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }

    Value val = pop();

    if (val.getType() == Value::Type::INTEGER) {
      push(Value(-val.getIntValue()));
    } else if (val.getType() == Value::Type::FLOAT) {
      push(Value(-val.getFloatValue()));
    } else {
      throw RuntimeException("Unsupported operand type for NEG");
    }
    break;
  }

  case OpCode::AND: {
    // Logical AND
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::BOOLEAN &&
        val2.getType() == Value::Type::BOOLEAN) {
      push(Value(val1.getBoolValue() && val2.getBoolValue()));
    } else {
      throw RuntimeException("Unsupported operand types for AND");
    }
    break;
  }

  case OpCode::OR: {
    // Logical OR
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::BOOLEAN &&
        val2.getType() == Value::Type::BOOLEAN) {
      push(Value(val1.getBoolValue() || val2.getBoolValue()));
    } else {
      throw RuntimeException("Unsupported operand types for OR");
    }
    break;
  }

  case OpCode::NOT: {
    // Logical NOT
    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }

    Value val = pop();

    if (val.getType() == Value::Type::BOOLEAN) {
      push(Value(!val.getBoolValue()));
    } else {
      throw RuntimeException("Unsupported operand type for NOT");
    }
    break;
  }

  case OpCode::EQ: {
    // Equals
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == val2.getType()) {
      bool result = false;

      switch (val1.getType()) {
      case Value::Type::NULL_VALUE:
        result = true; // null == null
        break;
      case Value::Type::BOOLEAN:
        result = val1.getBoolValue() == val2.getBoolValue();
        break;
      case Value::Type::INTEGER:
        result = val1.getIntValue() == val2.getIntValue();
        break;
      case Value::Type::FLOAT:
        result = val1.getFloatValue() == val2.getFloatValue();
        break;
      case Value::Type::STRING:
        result = val1.getStringValue() == val2.getStringValue();
        break;
      default:
        result = false; // Object identity not supported yet
        break;
      }

      push(Value(result));
    } else if ((val1.getType() == Value::Type::INTEGER &&
                val2.getType() == Value::Type::FLOAT) ||
               (val1.getType() == Value::Type::FLOAT &&
                val2.getType() == Value::Type::INTEGER)) {
      // Compare numeric types
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 == f2));
    } else {
      // Different types are not equal
      push(Value(false));
    }
    break;
  }

  case OpCode::NE: {
    // Not equals
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == val2.getType()) {
      bool result = false;

      switch (val1.getType()) {
      case Value::Type::NULL_VALUE:
        result = false; // null == null
        break;
      case Value::Type::BOOLEAN:
        result = val1.getBoolValue() != val2.getBoolValue();
        break;
      case Value::Type::INTEGER:
        result = val1.getIntValue() != val2.getIntValue();
        break;
      case Value::Type::FLOAT:
        result = val1.getFloatValue() != val2.getFloatValue();
        break;
      case Value::Type::STRING:
        result = val1.getStringValue() != val2.getStringValue();
        break;
      default:
        result = true; // Object identity not supported yet
        break;
      }

      push(Value(result));
    } else if ((val1.getType() == Value::Type::INTEGER &&
                val2.getType() == Value::Type::FLOAT) ||
               (val1.getType() == Value::Type::FLOAT &&
                val2.getType() == Value::Type::INTEGER)) {
      // Compare numeric types
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 != f2));
    } else {
      // Different types are not equal
      push(Value(true));
    }
    break;
  }

  case OpCode::LT: {
    // Less than
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      push(Value(val1.getIntValue() < val2.getIntValue()));
    } else if ((val1.getType() == Value::Type::INTEGER ||
                val1.getType() == Value::Type::FLOAT) &&
               (val2.getType() == Value::Type::INTEGER ||
                val2.getType() == Value::Type::FLOAT)) {
      // Compare numeric types
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 < f2));
    } else if (val1.getType() == Value::Type::STRING &&
               val2.getType() == Value::Type::STRING) {
      // String comparison
      push(Value(val1.getStringValue() < val2.getStringValue()));
    } else {
      throw RuntimeException("Unsupported operand types for LT");
    }
    break;
  }

  case OpCode::LE: {
    // Less than or equal
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      push(Value(val1.getIntValue() <= val2.getIntValue()));
    } else if ((val1.getType() == Value::Type::INTEGER ||
                val1.getType() == Value::Type::FLOAT) &&
               (val2.getType() == Value::Type::INTEGER ||
                val2.getType() == Value::Type::FLOAT)) {
      // Compare numeric types
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 <= f2));
    } else if (val1.getType() == Value::Type::STRING &&
               val2.getType() == Value::Type::STRING) {
      // String comparison
      push(Value(val1.getStringValue() <= val2.getStringValue()));
    } else {
      throw RuntimeException("Unsupported operand types for LE");
    }
    break;
  }

  case OpCode::GT: {
    // Greater than
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      push(Value(val1.getIntValue() > val2.getIntValue()));
    } else if ((val1.getType() == Value::Type::INTEGER ||
                val1.getType() == Value::Type::FLOAT) &&
               (val2.getType() == Value::Type::INTEGER ||
                val2.getType() == Value::Type::FLOAT)) {
      // Compare numeric types
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 > f2));
    } else if (val1.getType() == Value::Type::STRING &&
               val2.getType() == Value::Type::STRING) {
      // String comparison
      push(Value(val1.getStringValue() > val2.getStringValue()));
    } else {
      throw RuntimeException("Unsupported operand types for GT");
    }
    break;
  }

  case OpCode::GE: {
    // Greater than or equal
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow");
    }

    Value val2 = pop();
    Value val1 = pop();

    if (val1.getType() == Value::Type::INTEGER &&
        val2.getType() == Value::Type::INTEGER) {
      push(Value(val1.getIntValue() >= val2.getIntValue()));
    } else if ((val1.getType() == Value::Type::INTEGER ||
                val1.getType() == Value::Type::FLOAT) &&
               (val2.getType() == Value::Type::INTEGER ||
                val2.getType() == Value::Type::FLOAT)) {
      // Compare numeric types
      double f1 = (val1.getType() == Value::Type::FLOAT) ? val1.getFloatValue()
                                                         : val1.getIntValue();
      double f2 = (val2.getType() == Value::Type::FLOAT) ? val2.getFloatValue()
                                                         : val2.getIntValue();
      push(Value(f1 >= f2));
    } else if (val1.getType() == Value::Type::STRING &&
               val2.getType() == Value::Type::STRING) {
      // String comparison
      push(Value(val1.getStringValue() >= val2.getStringValue()));
    } else {
      throw RuntimeException("Unsupported operand types for GE");
    }
    break;
  }

  case OpCode::JUMP: {
    // Unconditional jump
    if (operands.empty()) {
      throw RuntimeException("JUMP requires an operand");
    }

    size_t jumpOffset = operands[0];
    context.setInstructionPointer(jumpOffset);
    return true;
  }

  case OpCode::JUMP_IF_TRUE: {
    // Jump if condition is true
    if (operands.empty()) {
      throw RuntimeException("JUMP_IF_TRUE requires an operand");
    }

    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }

    Value condition = pop();
    if (condition.getType() != Value::Type::BOOLEAN) {
      throw RuntimeException("Condition must be a boolean");
    }

    if (condition.getBoolValue()) {
      size_t jumpOffset = operands[0];
      context.setInstructionPointer(jumpOffset);
      return true;
    }
    break;
  }

  case OpCode::JUMP_IF_FALSE: {
    // Jump if condition is false
    if (operands.empty()) {
      throw RuntimeException("JUMP_IF_FALSE requires an operand");
    }

    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }

    Value condition = pop();
    if (condition.getType() != Value::Type::BOOLEAN) {
      throw RuntimeException("Condition must be a boolean");
    }

    if (!condition.getBoolValue()) {
      size_t jumpOffset = operands[0];
      context.setInstructionPointer(jumpOffset);
      return true;
    }
    break;
  }

  case OpCode::CALL: {
    // Call a function
    if (operands.size() < 2) {
      throw RuntimeException("CALL requires at least 2 operands");
    }

    size_t funcIndex = operands[0];
    size_t argCount = operands[1];

    if (!checkStackSize(argCount)) {
      throw RuntimeException("Stack underflow");
    }

    // Pop arguments in reverse order
    std::vector<Value> args;
    for (size_t i = 0; i < argCount; i++) {
      args.insert(args.begin(), pop());
    }

    // Get function name from constant pool
    if (funcIndex >= module.getConstantPoolSize()) {
      throw RuntimeException("Function index out of range: " +
                             std::to_string(funcIndex));
    }

    const auto &constant = module.getConstantPool()[funcIndex];
    if (constant.getType() !=
        compiler::bytecode::BytecodeConstant::Type::STRING) {
      throw RuntimeException("Function name must be a string constant");
    }

    std::string functionName = constant.getStringValue();

    // Execute function
    Value result = executeFunction(functionName, args);

    // Push result onto stack
    push(result);

    // Context has been updated during function execution
    context = callStack.top();
    break;
  }

  case OpCode::RETURN: {
    // Return from function
    Value result;
    if (!valueStack.empty()) {
      result = pop();
    }

    // Pop the current context
    callStack.pop();

    // If there's no calling context, we're done
    if (callStack.empty()) {
      // Push result for final return value
      push(result);
      return false;
    }

    // Restore the calling context
    context = callStack.top();
    context.setInstructionPointer(context.getReturnAddress());

    // Push result onto stack
    push(result);
    return true;
  }

  // Temporal operations
  case OpCode::REWIND:
  case OpCode::BRANCH:
  case OpCode::MERGE:
  case OpCode::STABILIZE:
    return executeTemporalOperation(opcode, operands, context);

  case OpCode::CONSUME_RESOURCE: {
    // Consume resources
    if (operands.size() < 2) {
      throw RuntimeException("CONSUME_RESOURCE requires 2 operands");
    }

    int aethel = operands[0];
    int chronon = operands[1];

    if (!checkResources(aethel, chronon)) {
      throw RuntimeException("Not enough resources");
    }

    consumeResources(aethel, chronon);
    break;
  }

  case OpCode::CHECK_RESOURCE: {
    // Check if enough resources are available
    if (operands.size() < 2) {
      throw RuntimeException("CHECK_RESOURCE requires 2 operands");
    }

    int aethel = operands[0];
    int chronon = operands[1];

    push(Value(checkResources(aethel, chronon)));
    break;
  }

  case OpCode::DEBUG_PRINT: {
    // Print debug information
    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow");
    }

    Value value = pop();
    std::cout << "[DEBUG] " << value.toString() << std::endl;
    break;
  }

  case OpCode::HALT: {
    // Halt execution
    return false;
  }

  case OpCode::CHECK_PARADOX_LEVEL: {
    // Get the current paradox level and push it onto the stack
    stdlib::PARADOX_LEVEL level = stdlib::get_paradox_level();
    push(Value(static_cast<double>(level)));
    std::cout << "[PARADOX] Current level: " << level << std::endl;
    break;
  }

  case OpCode::REDUCE_PARADOX_LEVEL: {
    // Pop the amount of Aethel to use for reduction
    if (!checkStackSize(1)) {
      throw RuntimeException("Stack underflow in REDUCE_PARADOX_LEVEL");
    }

    Value amountValue = pop();
    if (amountValue.getType() != Value::Type::INTEGER) {
      throw RuntimeException("REDUCE_PARADOX_LEVEL requires an integer amount");
    }

    int amount = amountValue.getIntValue();

    // Check if we have enough Aethel
    if (!checkResources(amount, 0)) {
      throw RuntimeException("Not enough Aethel for REDUCE_PARADOX_LEVEL");
    }

    // Use stabilize_timeline to reduce paradox level
    stdlib::AETHEL_RESERVE reserve = aethelResource;
    stdlib::PARADOX_LEVEL newLevel =
        stdlib::stabilize_timeline(reserve, amount);
    aethelResource = reserve;

    // Push the new paradox level onto the stack
    push(Value(static_cast<double>(newLevel)));
    break;
  }

  case OpCode::CREATE_CONTAINMENT_FIELD: {
    // Pop the size and name of the containment field
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow in CREATE_CONTAINMENT_FIELD");
    }

    Value sizeValue = pop();
    Value nameValue = pop();

    if (sizeValue.getType() != Value::Type::INTEGER ||
        nameValue.getType() != Value::Type::STRING) {
      throw RuntimeException(
          "CREATE_CONTAINMENT_FIELD requires a string name and integer size");
    }

    int size = sizeValue.getIntValue();
    std::string name = nameValue.getStringValue();

    // Create the containment field
    stdlib::AETHEL_RESERVE reserve = aethelResource;
    bool success = paradoxManager.create_containment_field(name, reserve, size);
    aethelResource = reserve;

    // Push the result onto the stack
    push(Value(success));
    break;
  }

  case OpCode::REGISTER_DEPENDENCY: {
    // Pop the strength, event_b, and event_a values
    if (!checkStackSize(3)) {
      throw RuntimeException("Stack underflow in REGISTER_DEPENDENCY");
    }

    Value strengthValue = pop();
    Value eventBValue = pop();
    Value eventAValue = pop();

    if (strengthValue.getType() != Value::Type::FLOAT ||
        eventBValue.getType() != Value::Type::STRING ||
        eventAValue.getType() != Value::Type::STRING) {
      throw RuntimeException("REGISTER_DEPENDENCY requires two string events "
                             "and a float strength");
    }

    double strength = strengthValue.getFloatValue();
    std::string eventB = eventBValue.getStringValue();
    std::string eventA = eventAValue.getStringValue();

    // Create a vector of dependencies
    std::vector<std::string> dependencies = {eventA, eventB};

    // Check for paradox
    bool noConflict = stdlib::paradox_check(dependencies);

    // Push the result onto the stack
    push(Value(noConflict));
    break;
  }

  case OpCode::CHECK_DEPENDENCY_CONFLICT: {
    // Pop the event_b and event_a values
    if (!checkStackSize(2)) {
      throw RuntimeException("Stack underflow in CHECK_DEPENDENCY_CONFLICT");
    }

    Value eventBValue = pop();
    Value eventAValue = pop();

    if (eventBValue.getType() != Value::Type::STRING ||
        eventAValue.getType() != Value::Type::STRING) {
      throw RuntimeException(
          "CHECK_DEPENDENCY_CONFLICT requires two string events");
    }

    std::string eventB = eventBValue.getStringValue();
    std::string eventA = eventAValue.getStringValue();

    // Check for conflict using the dependency vector
    std::vector<std::string> dependencies = {eventA, eventB};
    bool noConflict = stdlib::paradox_check(dependencies);

    // Push the result onto the stack
    push(Value(noConflict));
    break;
  }

  default:
    throw RuntimeException("Unknown opcode: " +
                           std::to_string(static_cast<int>(opcode)));
  }

  return true;
}

bool BytecodeInterpreter::executeTemporalOperation(
    compiler::bytecode::OpCode opcode, const std::vector<uint16_t> &operands,
    ExecutionContext &context) {
  using namespace compiler::bytecode;

  switch (opcode) {
  case OpCode::REWIND: {
    // Rewind to a previous point in time
    if (operands.empty()) {
      throw RuntimeException("REWIND requires an operand");
    }

    int steps = operands[0];

    // Consume resources for temporal operation
    if (!checkResources(steps * 10, steps * 5)) {
      throw RuntimeException("Not enough resources for REWIND");
    }

    consumeResources(steps * 10, steps * 5);

    // Update paradox level for REWIND operation
    updateParadoxLevel(OpCode::REWIND, 0.05);

    // TODO: Implement actual rewinding
    std::cout << "[TEMPORAL] Rewinding " << steps << " steps" << std::endl;
    break;
  }

  case OpCode::BRANCH: {
    // Branch timeline
    if (operands.empty()) {
      throw RuntimeException("BRANCH requires an operand");
    }

    // Consume resources for temporal operation
    if (!checkResources(50, 20)) {
      throw RuntimeException("Not enough resources for BRANCH");
    }

    consumeResources(50, 20);

    // Update paradox level for BRANCH operation
    updateParadoxLevel(OpCode::BRANCH, 0.07);

    // TODO: Implement actual branching
    currentTimeline++;
    std::cout << "[TEMPORAL] Branching to timeline " << currentTimeline
              << std::endl;
    break;
  }

  case OpCode::MERGE: {
    // Merge timelines
    if (operands.empty()) {
      throw RuntimeException("MERGE requires an operand");
    }

    // Consume resources for temporal operation
    if (!checkResources(100, 50)) {
      throw RuntimeException("Not enough resources for MERGE");
    }

    consumeResources(100, 50);

    // Update paradox level for MERGE operation
    updateParadoxLevel(OpCode::MERGE, 0.10);

    // TODO: Implement actual merging
    std::cout << "[TEMPORAL] Merging timelines" << std::endl;
    break;
  }

  case OpCode::STABILIZE: {
    // Stabilize timeline
    // Consume resources for temporal operation
    if (!checkResources(20, 10)) {
      throw RuntimeException("Not enough resources for STABILIZE");
    }

    consumeResources(20, 10);

    // Use aethel to reduce paradox level
    stdlib::AETHEL_RESERVE reserve = aethelResource;
    stdlib::stabilize_timeline(reserve, 50);
    aethelResource = reserve;

    std::cout << "[TEMPORAL] Stabilizing timeline" << std::endl;
    break;
  }

  default:
    throw RuntimeException("Unknown temporal operation: " +
                           std::to_string(static_cast<int>(opcode)));
  }

  return true;
}

Value BytecodeInterpreter::convertConstantToValue(
    const compiler::bytecode::BytecodeConstant &constant) {
  using namespace compiler::bytecode;

  switch (constant.getType()) {
  case BytecodeConstant::Type::NULL_VALUE:
    return Value();
  case BytecodeConstant::Type::BOOLEAN:
    return Value(constant.getBoolValue());
  case BytecodeConstant::Type::INTEGER:
    return Value(constant.getIntValue());
  case BytecodeConstant::Type::FLOAT:
    return Value(constant.getFloatValue());
  case BytecodeConstant::Type::STRING:
    return Value(constant.getStringValue());
  default:
    throw RuntimeException(
        "Unknown constant type: " +
        std::to_string(static_cast<int>(constant.getType())));
  }
}

void BytecodeInterpreter::push(const Value &value) { valueStack.push(value); }

Value BytecodeInterpreter::pop() {
  if (valueStack.empty()) {
    throw RuntimeException("Stack underflow");
  }

  Value value = valueStack.top();
  valueStack.pop();
  return value;
}

bool BytecodeInterpreter::checkStackSize(size_t count) {
  return valueStack.size() >= count;
}

bool BytecodeInterpreter::checkResources(int aethel, int chronon) {
  return aethelResource >= aethel && chrononResource >= chronon;
}

void BytecodeInterpreter::consumeResources(int aethel, int chronon) {
  aethelResource -= aethel;
  chrononResource -= chronon;
}

void BytecodeInterpreter::setGlobalVariable(const std::string &name,
                                            const Value &value) {
  globalVariables[name] = value;
}

Value BytecodeInterpreter::getGlobalVariable(const std::string &name) const {
  auto it = globalVariables.find(name);
  if (it != globalVariables.end()) {
    return it->second;
  }
  return Value(); // Return null value for undefined variables
}

void BytecodeInterpreter::updateParadoxLevel(compiler::bytecode::OpCode opcode,
                                             double paradoxImpact) {
  // Register this operation in the dependency registry if needed
  std::string operationName =
      "Operation_" + std::to_string(static_cast<int>(opcode));

  // Get the current paradox level before the update
  stdlib::PARADOX_LEVEL currentLevel = stdlib::get_paradox_level();

  // For simplicity, we'll use trigger_paradox_level, but in a real
  // implementation we would integrate more deeply with the paradox_management
  // system
  stdlib::trigger_paradox_level(currentLevel + paradoxImpact);

  // Report the change
  std::cout << "[PARADOX] Level increased from " << currentLevel << " to "
            << stdlib::get_paradox_level() << " due to operation "
            << operationName << std::endl;

  // Check if we're approaching dangerous levels
  if (stdlib::get_paradox_level() >= 0.7 && !paradoxAlertSet) {
    std::cout << "[WARNING] Paradox level approaching critical threshold!"
              << std::endl;
    std::cout << "[WARNING] Consider using STABILIZE operations to reduce "
                 "paradox level."
              << std::endl;
  }
}

void BytecodeInterpreter::setupParadoxAlerts() {
  // Set up paradox alerts for various thresholds
  stdlib::set_paradox_alert(0.5, [this](stdlib::PARADOX_LEVEL level) {
    std::cout << "[ALERT] Paradox level at " << level << " - Caution advised"
              << std::endl;
  });

  stdlib::set_paradox_alert(0.7, [this](stdlib::PARADOX_LEVEL level) {
    std::cout << "[WARNING] Paradox level at " << level
              << " - Timeline instability detected" << std::endl;
  });

  stdlib::set_paradox_alert(
      0.9, [this](stdlib::PARADOX_LEVEL level) { handleParadoxAlert(level); });

  paradoxAlertSet = true;
}

void BytecodeInterpreter::handleParadoxAlert(stdlib::PARADOX_LEVEL level) {
  std::cout << "[CRITICAL] Paradox level at " << level
            << " - Timeline collapse imminent!" << std::endl;

  // Create a containment field to isolate the paradox
  stdlib::AETHEL_RESERVE reserve = aethelResource;
  if (paradoxManager.create_containment_field("Emergency_Containment", reserve,
                                              100)) {
    aethelResource = reserve;
    std::cout << "[RECOVERY] Emergency containment field deployed" << std::endl;
  } else {
    std::cout << "[CRITICAL] Failed to deploy containment field - insufficient "
                 "resources"
              << std::endl;
    throw RuntimeException("Critical paradox level - execution terminated to "
                           "prevent timeline collapse");
  }
}

} // namespace interpreter
} // namespace chronovyan