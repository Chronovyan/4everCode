#include <algorithm>
#include <iostream>
#include <sstream>

#include "compiler/debugger/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {
namespace debugger {

// DejaVuDebugger implementation
DejaVuDebugger::DejaVuDebugger(DiagnosticReporter &diag)
    : diagnostics(diag), isRunning(false), nextCallbackHandle(0) {
  currentState = std::make_unique<DebuggerState>();
  stateHistory = std::make_unique<StateHistory>();
  breakpointMgr = std::make_unique<BreakpointManager>();
  stateVisualizer = std::make_unique<StateVisualizer>();
}

DejaVuDebugger::~DejaVuDebugger() = default;

bool DejaVuDebugger::initialize(const bytecode::BytecodeModule &module) {
  bytecodeModule = module;

  // Reset the state
  currentState = std::make_unique<DebuggerState>();
  stateHistory = std::make_unique<StateHistory>();

  // Set initial instruction pointer
  currentState->setInstructionPointer(0);

  // Find the main function if it exists
  const auto &functions = bytecodeModule.getFunctions();
  for (const auto &function : functions) {
    if (function.getName() == "main") {
      currentState->pushCallStack("main");
      break;
    }
  }

  // Set the initial source location based on debug information if available
  if (!bytecodeModule.getDebugInfo().empty()) {
    const auto &debugInfo = bytecodeModule.getDebugInfo().front();
    currentState->setSourceLocation(debugInfo.sourceFile, debugInfo.line);
  }

  return true;
}

bool DejaVuDebugger::start() {
  if (isRunning) {
    diagnostics.addWarning(SourceLocation(), "Debugger is already running");
    return false;
  }

  isRunning = true;

  // Reset state history
  stateHistory->clear();

  // Add initial state to history
  updateStateHistory();

  return true;
}

void DejaVuDebugger::stop() { isRunning = false; }

bool DejaVuDebugger::stepInto() {
  if (!isRunning) {
    diagnostics.addWarning(SourceLocation(), "Debugger is not running");
    return false;
  }

  bool success = executeInstruction();
  if (success) {
    updateStateHistory();
  }

  return success;
}

bool DejaVuDebugger::stepOver() {
  if (!isRunning) {
    diagnostics.addWarning(SourceLocation(), "Debugger is not running");
    return false;
  }

  // Get the current call stack depth
  size_t currentDepth = currentState->getCallStack().size();

  // Execute instructions until we return to the same or lower call stack depth
  // or hit a breakpoint
  while (isRunning) {
    bool success = executeInstruction();
    if (!success) {
      return false;
    }

    updateStateHistory();

    // Check if we've returned to the same or lower call stack depth
    if (currentState->getCallStack().size() <= currentDepth) {
      return true;
    }

    // Check if we hit a breakpoint
    if (checkBreakpoints()) {
      return true;
    }
  }

  return false;
}

bool DejaVuDebugger::stepOut() {
  if (!isRunning) {
    diagnostics.addWarning(SourceLocation(), "Debugger is not running");
    return false;
  }

  // Get the current call stack depth
  size_t currentDepth = currentState->getCallStack().size();
  if (currentDepth <= 1) {
    // Already at the top level, just execute one instruction
    return stepInto();
  }

  // Execute instructions until we return to a lower call stack depth
  // or hit a breakpoint
  while (isRunning) {
    bool success = executeInstruction();
    if (!success) {
      return false;
    }

    updateStateHistory();

    // Check if we've returned to a lower call stack depth
    if (currentState->getCallStack().size() < currentDepth) {
      return true;
    }

    // Check if we hit a breakpoint
    if (checkBreakpoints()) {
      return true;
    }
  }

  return false;
}

bool DejaVuDebugger::continueExecution() {
  if (!isRunning) {
    diagnostics.addWarning(SourceLocation(), "Debugger is not running");
    return false;
  }

  // Execute instructions until we hit a breakpoint or the program ends
  while (isRunning) {
    bool success = executeInstruction();
    if (!success) {
      return false;
    }

    updateStateHistory();

    // Check if we hit a breakpoint
    if (checkBreakpoints()) {
      notifyBreakpointCallbacks();
      return true;
    }
  }

  return false;
}

int DejaVuDebugger::setBreakpoint(const std::string &sourceFile, int line,
                                  const BreakpointCondition &condition) {
  return breakpointMgr->setBreakpoint(sourceFile, line, condition);
}

bool DejaVuDebugger::removeBreakpoint(int breakpointId) {
  return breakpointMgr->removeBreakpoint(breakpointId);
}

const DebuggerState &DejaVuDebugger::getCurrentState() const {
  return *currentState;
}

const StateHistory &DejaVuDebugger::getStateHistory() const {
  return *stateHistory;
}

std::string
DejaVuDebugger::getVariableValue(const std::string &variableName) const {
  // Check local variables first
  const auto &localVars = currentState->getLocalVariables();
  auto localIt = localVars.find(variableName);
  if (localIt != localVars.end()) {
    return localIt->second;
  }

  // Check global variables
  const auto &globalVars = currentState->getGlobalVariables();
  auto globalIt = globalVars.find(variableName);
  if (globalIt != globalVars.end()) {
    return globalIt->second;
  }

  return "";
}

std::string DejaVuDebugger::getVariableValueAt(const std::string &variableName,
                                               int historyIndex) const {
  if (historyIndex < 0 ||
      historyIndex >= static_cast<int>(stateHistory->getStateCount())) {
    return "";
  }

  const DebuggerState &state = stateHistory->getState(historyIndex);

  // Check local variables first
  const auto &localVars = state.getLocalVariables();
  auto localIt = localVars.find(variableName);
  if (localIt != localVars.end()) {
    return localIt->second;
  }

  // Check global variables
  const auto &globalVars = state.getGlobalVariables();
  auto globalIt = globalVars.find(variableName);
  if (globalIt != globalVars.end()) {
    return globalIt->second;
  }

  return "";
}

int DejaVuDebugger::getCurrentTimelineId() const {
  return currentState->getTimelineId();
}

std::map<std::string, double> DejaVuDebugger::getCurrentResourceUsage() const {
  return currentState->getResourceUsage();
}

int DejaVuDebugger::registerBreakpointCallback(
    std::function<void(const DebuggerState &)> callback) {
  int handle = nextCallbackHandle++;
  breakpointCallbacks[handle] = callback;
  return handle;
}

bool DejaVuDebugger::unregisterBreakpointCallback(int handle) {
  return breakpointCallbacks.erase(handle) > 0;
}

std::string
DejaVuDebugger::visualizeCurrentState(const std::string &format) const {
  return stateVisualizer->visualizeState(*currentState, format);
}

std::string
DejaVuDebugger::visualizeStateHistory(const std::string &format) const {
  return stateVisualizer->visualizeHistory(*stateHistory, format);
}

std::string
DejaVuDebugger::visualizeTemporalFlow(const std::string &format) const {
  return stateVisualizer->visualizeTemporalFlow(*stateHistory, format);
}

bool DejaVuDebugger::checkBreakpoints() {
  return breakpointMgr->isBreakpointHit(*currentState);
}

bool DejaVuDebugger::executeInstruction() {
  if (!isRunning) {
    return false;
  }

  // Get the current instruction pointer
  size_t ip = currentState->getInstructionPointer();

  // Get the current function
  const auto &callStack = currentState->getCallStack();
  if (callStack.empty()) {
    diagnostics.addError(SourceLocation(), "Call stack is empty");
    stop();
    return false;
  }

  const std::string &currentFunction = callStack.back();

  // Find the function in the bytecode module
  const auto &functions = bytecodeModule.getFunctions();
  auto functionIt =
      std::find_if(functions.begin(), functions.end(),
                   [&currentFunction](const bytecode::Function &func) {
                     return func.getName() == currentFunction;
                   });

  if (functionIt == functions.end()) {
    diagnostics.addError(SourceLocation(),
                         "Function not found: " + currentFunction);
    stop();
    return false;
  }

  // Get the function's instructions
  const auto &instructions = functionIt->getInstructions();

  // Check if we've reached the end of the function
  if (ip >= instructions.size()) {
    // If we're in the main function, end the program
    if (currentFunction == "main") {
      stop();
      return false;
    }

    // Otherwise, return from the function
    currentState->popCallStack();
    if (!currentState->getCallStack().empty()) {
      // Resume execution in the caller function
      // Note: In a real implementation, we would need to get the caller's
      // instruction pointer and restore it here. For simplicity, we just
      // increment it.
      currentState->setInstructionPointer(
          currentState->getInstructionPointer() + 1);
    } else {
      // No more functions to execute
      stop();
      return false;
    }

    return true;
  }

  // Get the current instruction
  const bytecode::Instruction &instruction = instructions[ip];

  // Update source location based on debug information if available
  if (instruction.hasDebugInfo()) {
    const auto &debugInfo = instruction.getDebugInfo();
    currentState->setSourceLocation(debugInfo.sourceFile, debugInfo.line);
  }

  // Execute the instruction based on its opcode
  switch (instruction.getOpcode()) {
  case bytecode::OpCode::NOP:
    // Do nothing
    break;

  case bytecode::OpCode::LOAD_CONST:
    // Load a constant value
    // In a real implementation, we would push the constant onto the stack
    break;

  case bytecode::OpCode::LOAD_VAR:
    // Load a variable
    // In a real implementation, we would push the variable value onto the stack
    break;

  case bytecode::OpCode::STORE_VAR: {
    // Store a value in a variable
    // In a real implementation, we would pop a value from the stack and store
    // it in the variable

    // For demonstration purposes, assume the instruction has a string operand
    // that is the variable name
    if (instruction.getOperands().empty() ||
        instruction.getOperands()[0].getType() !=
            bytecode::OperandType::STRING) {
      diagnostics.addError(SourceLocation(), "Invalid STORE_VAR instruction");
      stop();
      return false;
    }

    std::string varName = instruction.getOperands()[0].getStringValue();

    // For demonstration, just store a dummy value
    currentState->setLocalVariable(varName, "<value>");
    break;
  }

  case bytecode::OpCode::CALL: {
    // Call a function
    // In a real implementation, we would set up the call frame, push arguments,
    // etc.

    // For demonstration purposes, assume the instruction has a string operand
    // that is the function name
    if (instruction.getOperands().empty() ||
        instruction.getOperands()[0].getType() !=
            bytecode::OperandType::STRING) {
      diagnostics.addError(SourceLocation(), "Invalid CALL instruction");
      stop();
      return false;
    }

    std::string funcName = instruction.getOperands()[0].getStringValue();

    // Push the function onto the call stack
    currentState->pushCallStack(funcName);

    // Set the instruction pointer to 0 for the new function
    currentState->setInstructionPointer(0);

    return true;
  }

  case bytecode::OpCode::RETURN:
    // Return from a function
    currentState->popCallStack();

    if (currentState->getCallStack().empty()) {
      // No more functions to execute
      stop();
      return false;
    }

    // Resume execution in the caller function
    // Note: In a real implementation, we would need to get the caller's
    // instruction pointer and restore it here. For simplicity, we just
    // increment it.
    currentState->setInstructionPointer(currentState->getInstructionPointer() +
                                        1);

    return true;

  case bytecode::OpCode::JUMP: {
    // Jump to a different instruction
    // For demonstration purposes, assume the instruction has an integer operand
    // that is the jump offset
    if (instruction.getOperands().empty() ||
        instruction.getOperands()[0].getType() !=
            bytecode::OperandType::INTEGER) {
      diagnostics.addError(SourceLocation(), "Invalid JUMP instruction");
      stop();
      return false;
    }

    int offset = instruction.getOperands()[0].getIntValue();

    // Update the instruction pointer
    currentState->setInstructionPointer(ip + offset);

    return true;
  }

  case bytecode::OpCode::JUMP_IF_TRUE: {
    // Jump if the top of the stack is true
    // For demonstration purposes, assume the instruction has an integer operand
    // that is the jump offset
    if (instruction.getOperands().empty() ||
        instruction.getOperands()[0].getType() !=
            bytecode::OperandType::INTEGER) {
      diagnostics.addError(SourceLocation(),
                           "Invalid JUMP_IF_TRUE instruction");
      stop();
      return false;
    }

    int offset = instruction.getOperands()[0].getIntValue();

    // In a real implementation, we would check the top of the stack
    // For demonstration, just use a random value
    bool condition = (rand() % 2) == 0;

    if (condition) {
      // Update the instruction pointer
      currentState->setInstructionPointer(ip + offset);
    } else {
      // Move to the next instruction
      currentState->setInstructionPointer(ip + 1);
    }

    return true;
  }

  case bytecode::OpCode::TIMELINE_BRANCH_BEGIN: {
    // Begin a timeline branch
    // For demonstration purposes, assume the instruction has an integer operand
    // that is the branch count
    if (instruction.getOperands().empty() ||
        instruction.getOperands()[0].getType() !=
            bytecode::OperandType::INTEGER) {
      diagnostics.addError(SourceLocation(),
                           "Invalid TIMELINE_BRANCH_BEGIN instruction");
      stop();
      return false;
    }

    int branchCount = instruction.getOperands()[0].getIntValue();

    // For demonstration, just set the timeline ID to a random value
    currentState->setTimelineId(rand() % branchCount);

    // Move to the next instruction
    currentState->setInstructionPointer(ip + 1);

    return true;
  }

  case bytecode::OpCode::TIMELINE_BRANCH_END:
    // End a timeline branch

    // Reset the timeline ID
    currentState->setTimelineId(0);

    // Move to the next instruction
    currentState->setInstructionPointer(ip + 1);

    return true;

  case bytecode::OpCode::PARALLEL_BEGIN: {
    // Begin parallel execution
    // For demonstration purposes, assume the instruction has an integer operand
    // that is the thread count
    if (instruction.getOperands().empty() ||
        instruction.getOperands()[0].getType() !=
            bytecode::OperandType::INTEGER) {
      diagnostics.addError(SourceLocation(),
                           "Invalid PARALLEL_BEGIN instruction");
      stop();
      return false;
    }

    int threadCount = instruction.getOperands()[0].getIntValue();

    // For demonstration, just set the thread ID to a random value
    currentState->setThreadId(rand() % threadCount);

    // Move to the next instruction
    currentState->setInstructionPointer(ip + 1);

    return true;
  }

  case bytecode::OpCode::PARALLEL_END:
    // End parallel execution

    // Reset the thread ID
    currentState->setThreadId(0);

    // Move to the next instruction
    currentState->setInstructionPointer(ip + 1);

    return true;

  case bytecode::OpCode::RESOURCE_BEGIN: {
    // Begin a resource block
    // For demonstration purposes, assume the instruction has a float operand
    // that is the resource amount
    if (instruction.getOperands().empty() ||
        instruction.getOperands()[0].getType() !=
            bytecode::OperandType::FLOAT) {
      diagnostics.addError(SourceLocation(),
                           "Invalid RESOURCE_BEGIN instruction");
      stop();
      return false;
    }

    double amount = instruction.getOperands()[0].getFloatValue();

    // Update the resource usage
    currentState->setResourceUsage("chronons", amount);

    // Move to the next instruction
    currentState->setInstructionPointer(ip + 1);

    return true;
  }

  case bytecode::OpCode::RESOURCE_END:
    // End a resource block

    // Reset the resource usage
    currentState->setResourceUsage("chronons", 0.0);

    // Move to the next instruction
    currentState->setInstructionPointer(ip + 1);

    return true;

  default:
    // Unknown opcode
    diagnostics.addError(SourceLocation(),
                         "Unknown opcode: " + std::to_string(static_cast<int>(
                                                  instruction.getOpcode())));
    stop();
    return false;
  }

  // Move to the next instruction
  currentState->setInstructionPointer(ip + 1);

  return true;
}

void DejaVuDebugger::updateStateHistory() {
  stateHistory->addState(*currentState);
}

void DejaVuDebugger::notifyBreakpointCallbacks() {
  for (const auto &[handle, callback] : breakpointCallbacks) {
    callback(*currentState);
  }
}

} // namespace debugger
} // namespace compiler
} // namespace chronovyan