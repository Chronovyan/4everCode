#include <iomanip>
#include <sstream>

#include "compiler/debugger/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {
namespace debugger {

// StateVisualizer implementation
StateVisualizer::StateVisualizer() {}

StateVisualizer::~StateVisualizer() = default;

std::string StateVisualizer::visualizeState(const DebuggerState &state,
                                            const std::string &format) {
  return formatState(state, format);
}

std::string StateVisualizer::visualizeHistory(const StateHistory &history,
                                              const std::string &format) {
  std::stringstream ss;

  ss << "State History (" << history.getStateCount()
     << " states):" << std::endl;

  for (size_t i = 0; i < history.getStateCount(); ++i) {
    ss << "===== State " << i << " =====" << std::endl;
    ss << formatState(history.getState(i), format) << std::endl;
  }

  return ss.str();
}

std::string StateVisualizer::visualizeTemporalFlow(const StateHistory &history,
                                                   const std::string &format) {
  std::stringstream ss;

  ss << "Temporal Flow:" << std::endl;

  // Create a map to track timeline branches
  std::map<int, std::vector<size_t>> timelineStates;

  // Collect states by timeline ID
  for (size_t i = 0; i < history.getStateCount(); ++i) {
    const DebuggerState &state = history.getState(i);
    timelineStates[state.getTimelineId()].push_back(i);
  }

  // Output timeline branches
  for (const auto &[timelineId, states] : timelineStates) {
    ss << "Timeline " << timelineId << " (" << states.size()
       << " states):" << std::endl;

    for (size_t stateIndex : states) {
      const DebuggerState &state = history.getState(stateIndex);

      ss << "  State " << stateIndex << ": ";
      ss << state.getSourceLocation().file << ":"
         << state.getSourceLocation().line;

      if (state.isPotentialParadox()) {
        ss << " [POTENTIAL PARADOX]";
      }

      ss << std::endl;
    }
  }

  return ss.str();
}

std::string StateVisualizer::formatState(const DebuggerState &state,
                                         const std::string &format) {
  std::stringstream ss;

  if (format == "json") {
    // JSON format
    ss << "{" << std::endl;
    ss << "  \"sourceFile\": \"" << state.getSourceLocation().file << "\","
       << std::endl;
    ss << "  \"sourceLine\": " << state.getSourceLocation().line << ","
       << std::endl;
    ss << "  \"instructionPointer\": " << state.getInstructionPointer() << ","
       << std::endl;

    // Call stack
    ss << "  \"callStack\": [";
    const auto &callStack = state.getCallStack();
    for (size_t i = 0; i < callStack.size(); ++i) {
      if (i > 0)
        ss << ", ";
      ss << "\"" << callStack[i] << "\"";
    }
    ss << "]," << std::endl;

    // Local variables
    ss << "  \"localVariables\": {" << std::endl;
    const auto &localVars = state.getLocalVariables();
    size_t localVarCount = 0;
    for (const auto &[name, value] : localVars) {
      if (localVarCount > 0)
        ss << "," << std::endl;
      ss << "    \"" << name << "\": \"" << value << "\"";
      ++localVarCount;
    }
    ss << std::endl << "  }," << std::endl;

    // Global variables
    ss << "  \"globalVariables\": {" << std::endl;
    const auto &globalVars = state.getGlobalVariables();
    size_t globalVarCount = 0;
    for (const auto &[name, value] : globalVars) {
      if (globalVarCount > 0)
        ss << "," << std::endl;
      ss << "    \"" << name << "\": \"" << value << "\"";
      ++globalVarCount;
    }
    ss << std::endl << "  }," << std::endl;

    // Resource usage
    ss << "  \"resourceUsage\": {" << std::endl;
    const auto &resources = state.getResourceUsage();
    size_t resourceCount = 0;
    for (const auto &[name, value] : resources) {
      if (resourceCount > 0)
        ss << "," << std::endl;
      ss << "    \"" << name << "\": " << value;
      ++resourceCount;
    }
    ss << std::endl << "  }," << std::endl;

    ss << "  \"timelineId\": " << state.getTimelineId() << "," << std::endl;
    ss << "  \"threadId\": " << state.getThreadId() << "," << std::endl;
    ss << "  \"isPotentialParadox\": "
       << (state.isPotentialParadox() ? "true" : "false") << std::endl;

    ss << "}";
  } else {
    // Default text format
    ss << "Source: " << state.getSourceLocation().file << ":"
       << state.getSourceLocation().line << std::endl;
    ss << "Instruction Pointer: " << state.getInstructionPointer() << std::endl;

    // Call stack
    ss << "Call Stack:" << std::endl;
    const auto &callStack = state.getCallStack();
    for (size_t i = 0; i < callStack.size(); ++i) {
      ss << "  " << i << ": " << callStack[i] << std::endl;
    }

    // Local variables
    ss << "Local Variables:" << std::endl;
    const auto &localVars = state.getLocalVariables();
    for (const auto &[name, value] : localVars) {
      ss << "  " << name << " = " << value << std::endl;
    }

    // Global variables
    ss << "Global Variables:" << std::endl;
    const auto &globalVars = state.getGlobalVariables();
    for (const auto &[name, value] : globalVars) {
      ss << "  " << name << " = " << value << std::endl;
    }

    // Resource usage
    ss << "Resource Usage:" << std::endl;
    const auto &resources = state.getResourceUsage();
    for (const auto &[name, value] : resources) {
      ss << "  " << name << " = " << std::fixed << std::setprecision(2) << value
         << std::endl;
    }

    ss << "Timeline ID: " << state.getTimelineId() << std::endl;
    ss << "Thread ID: " << state.getThreadId() << std::endl;

    if (state.isPotentialParadox()) {
      ss << "WARNING: Potential Paradox Detected!" << std::endl;
    }
  }

  return ss.str();
}

} // namespace debugger
} // namespace compiler
} // namespace chronovyan