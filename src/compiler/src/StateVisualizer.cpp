#include <iomanip>
#include <sstream>

#include "compiler/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {

StateVisualizer::StateVisualizer() = default;

StateVisualizer::~StateVisualizer() = default;

std::string StateVisualizer::visualizeState(const DebuggerState &state,
                                            const std::string &format) const {
  if (format == "json") {
    return generateJsonVisualization(state);
  } else if (format == "html") {
    return generateHtmlVisualization(state);
  } else {
    // Default to text format
    return generateTextVisualization(state);
  }
}

std::string StateVisualizer::visualizeHistory(const StateHistory &history,
                                              const std::string &format) const {
  if (format == "json") {
    return generateJsonHistoryVisualization(history);
  } else if (format == "html") {
    return generateHtmlHistoryVisualization(history);
  } else {
    // Default to text format
    return generateTextHistoryVisualization(history);
  }
}

std::string
StateVisualizer::visualizeTemporalFlow(const StateHistory &history,
                                       const std::string &format) const {
  if (format == "json") {
    return generateJsonTemporalFlowVisualization(history);
  } else if (format == "html") {
    return generateHtmlTemporalFlowVisualization(history);
  } else {
    // Default to text format
    return generateTextTemporalFlowVisualization(history);
  }
}

std::string
StateVisualizer::generateTextVisualization(const DebuggerState &state) const {
  std::ostringstream oss;

  // Source location
  auto [file, line] = state.getSourceLocation();
  oss << "===== Debugger State =====\n";
  oss << "Source: " << file << ":" << line << "\n";

  // Instruction pointer
  oss << "Instruction Pointer: " << state.getInstructionPointer() << "\n";

  // Call stack
  oss << "Call Stack:\n";
  const auto &callStack = state.getCallStack();
  if (callStack.empty()) {
    oss << "  <empty>\n";
  } else {
    for (size_t i = 0; i < callStack.size(); ++i) {
      oss << "  " << i << ": " << callStack[i] << "\n";
    }
  }

  // Timeline and thread info
  oss << "Timeline ID: " << state.getTimelineId() << "\n";
  oss << "Thread ID: " << state.getThreadId() << "\n";

  // Local variables
  oss << "Local Variables:\n";
  const auto &localVars = state.getLocalVariables();
  if (localVars.empty()) {
    oss << "  <none>\n";
  } else {
    for (const auto &[name, value] : localVars) {
      oss << "  " << name << " = " << value << "\n";
    }
  }

  // Global variables
  oss << "Global Variables:\n";
  const auto &globalVars = state.getGlobalVariables();
  if (globalVars.empty()) {
    oss << "  <none>\n";
  } else {
    for (const auto &[name, value] : globalVars) {
      oss << "  " << name << " = " << value << "\n";
    }
  }

  // Resource usage
  oss << "Resource Usage:\n";
  const auto &resources = state.getResourceUsage();
  if (resources.empty()) {
    oss << "  <none>\n";
  } else {
    for (const auto &[name, value] : resources) {
      oss << "  " << name << " = " << value << "\n";
    }
  }

  // Paradox risk
  oss << "Potential Paradox: " << (state.isPotentialParadox() ? "YES" : "NO")
      << "\n";

  return oss.str();
}

std::string
StateVisualizer::generateJsonVisualization(const DebuggerState &state) const {
  std::ostringstream oss;
  auto [file, line] = state.getSourceLocation();

  oss << "{\n";
  oss << "  \"sourceLocation\": {\n";
  oss << "    \"file\": \"" << file << "\",\n";
  oss << "    \"line\": " << line << "\n";
  oss << "  },\n";
  oss << "  \"instructionPointer\": " << state.getInstructionPointer() << ",\n";

  // Call stack
  oss << "  \"callStack\": [\n";
  const auto &callStack = state.getCallStack();
  for (size_t i = 0; i < callStack.size(); ++i) {
    oss << "    \"" << callStack[i] << "\"";
    if (i < callStack.size() - 1) {
      oss << ",";
    }
    oss << "\n";
  }
  oss << "  ],\n";

  // Timeline and thread info
  oss << "  \"timelineId\": " << state.getTimelineId() << ",\n";
  oss << "  \"threadId\": " << state.getThreadId() << ",\n";

  // Local variables
  oss << "  \"localVariables\": {\n";
  const auto &localVars = state.getLocalVariables();
  size_t localVarCount = 0;
  for (const auto &[name, value] : localVars) {
    oss << "    \"" << name << "\": \"" << value << "\"";
    if (++localVarCount < localVars.size()) {
      oss << ",";
    }
    oss << "\n";
  }
  oss << "  },\n";

  // Global variables
  oss << "  \"globalVariables\": {\n";
  const auto &globalVars = state.getGlobalVariables();
  size_t globalVarCount = 0;
  for (const auto &[name, value] : globalVars) {
    oss << "    \"" << name << "\": \"" << value << "\"";
    if (++globalVarCount < globalVars.size()) {
      oss << ",";
    }
    oss << "\n";
  }
  oss << "  },\n";

  // Resource usage
  oss << "  \"resourceUsage\": {\n";
  const auto &resources = state.getResourceUsage();
  size_t resourceCount = 0;
  for (const auto &[name, value] : resources) {
    oss << "    \"" << name << "\": " << value;
    if (++resourceCount < resources.size()) {
      oss << ",";
    }
    oss << "\n";
  }
  oss << "  },\n";

  // Paradox risk
  oss << "  \"potentialParadox\": "
      << (state.isPotentialParadox() ? "true" : "false") << "\n";

  oss << "}\n";

  return oss.str();
}

std::string
StateVisualizer::generateHtmlVisualization(const DebuggerState &state) const {
  // For brevity, we'll implement a simplified HTML visualization
  std::ostringstream oss;
  auto [file, line] = state.getSourceLocation();

  oss << "<!DOCTYPE html>\n";
  oss << "<html>\n";
  oss << "<head>\n";
  oss << "  <title>Chronovyan Debugger State</title>\n";
  oss << "  <style>\n";
  oss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
  oss << "    h1 { color: #336699; }\n";
  oss << "    .section { margin-bottom: 20px; }\n";
  oss << "    .section-title { font-weight: bold; margin-bottom: 5px; }\n";
  oss << "    .variable { margin-left: 20px; }\n";
  oss << "    .call-stack { margin-left: 20px; }\n";
  oss << "    .resource { margin-left: 20px; }\n";
  oss << "    .paradox { color: red; font-weight: bold; }\n";
  oss << "  </style>\n";
  oss << "</head>\n";
  oss << "<body>\n";

  oss << "  <h1>Chronovyan Debugger State</h1>\n";

  // Source location
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Source Location:</div>\n";
  oss << "    " << file << ":" << line << "\n";
  oss << "  </div>\n";

  // Instruction pointer
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Instruction Pointer:</div>\n";
  oss << "    " << state.getInstructionPointer() << "\n";
  oss << "  </div>\n";

  // Call stack
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Call Stack:</div>\n";
  const auto &callStack = state.getCallStack();
  if (callStack.empty()) {
    oss << "    <div class=\"call-stack\">&lt;empty&gt;</div>\n";
  } else {
    for (size_t i = 0; i < callStack.size(); ++i) {
      oss << "    <div class=\"call-stack\">" << i << ": " << callStack[i]
          << "</div>\n";
    }
  }
  oss << "  </div>\n";

  // Timeline and thread info
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Timeline ID:</div>\n";
  oss << "    " << state.getTimelineId() << "\n";
  oss << "  </div>\n";

  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Thread ID:</div>\n";
  oss << "    " << state.getThreadId() << "\n";
  oss << "  </div>\n";

  // Local variables
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Local Variables:</div>\n";
  const auto &localVars = state.getLocalVariables();
  if (localVars.empty()) {
    oss << "    <div class=\"variable\">&lt;none&gt;</div>\n";
  } else {
    for (const auto &[name, value] : localVars) {
      oss << "    <div class=\"variable\">" << name << " = " << value
          << "</div>\n";
    }
  }
  oss << "  </div>\n";

  // Global variables
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Global Variables:</div>\n";
  const auto &globalVars = state.getGlobalVariables();
  if (globalVars.empty()) {
    oss << "    <div class=\"variable\">&lt;none&gt;</div>\n";
  } else {
    for (const auto &[name, value] : globalVars) {
      oss << "    <div class=\"variable\">" << name << " = " << value
          << "</div>\n";
    }
  }
  oss << "  </div>\n";

  // Resource usage
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Resource Usage:</div>\n";
  const auto &resources = state.getResourceUsage();
  if (resources.empty()) {
    oss << "    <div class=\"resource\">&lt;none&gt;</div>\n";
  } else {
    for (const auto &[name, value] : resources) {
      oss << "    <div class=\"resource\">" << name << " = " << value
          << "</div>\n";
    }
  }
  oss << "  </div>\n";

  // Paradox risk
  oss << "  <div class=\"section\">\n";
  oss << "    <div class=\"section-title\">Potential Paradox:</div>\n";
  if (state.isPotentialParadox()) {
    oss << "    <div class=\"paradox\">YES</div>\n";
  } else {
    oss << "    NO\n";
  }
  oss << "  </div>\n";

  oss << "</body>\n";
  oss << "</html>\n";

  return oss.str();
}

// Simple text visualization of the state history
std::string StateVisualizer::generateTextHistoryVisualization(
    const StateHistory &history) const {
  std::ostringstream oss;

  oss << "===== State History =====\n";
  oss << "Total States: " << history.getStateCount() << "\n\n";

  // Show a summary of each state
  for (size_t i = 0; i < history.getStateCount(); ++i) {
    const DebuggerState &state = history.getState(i);
    auto [file, line] = state.getSourceLocation();

    oss << "State " << i << ":\n";
    oss << "  Source: " << file << ":" << line << "\n";
    oss << "  Call Stack: ";

    const auto &callStack = state.getCallStack();
    if (callStack.empty()) {
      oss << "<empty>";
    } else {
      oss << callStack.back();
    }
    oss << "\n";

    oss << "  Timeline ID: " << state.getTimelineId() << "\n";
    oss << "  Thread ID: " << state.getThreadId() << "\n";

    // Show resource usage
    const auto &resources = state.getResourceUsage();
    if (!resources.empty()) {
      oss << "  Resource Usage:\n";
      for (const auto &[name, value] : resources) {
        oss << "    " << name << " = " << value << "\n";
      }
    }

    // Show if there's a potential paradox
    if (state.isPotentialParadox()) {
      oss << "  *** POTENTIAL PARADOX ***\n";
    }

    oss << "\n";
  }

  return oss.str();
}

// Simple JSON visualization of the state history
std::string StateVisualizer::generateJsonHistoryVisualization(
    const StateHistory &history) const {
  std::ostringstream oss;

  oss << "{\n";
  oss << "  \"stateCount\": " << history.getStateCount() << ",\n";
  oss << "  \"states\": [\n";

  // Show each state
  for (size_t i = 0; i < history.getStateCount(); ++i) {
    const DebuggerState &state = history.getState(i);
    auto [file, line] = state.getSourceLocation();

    oss << "    {\n";
    oss << "      \"index\": " << i << ",\n";
    oss << "      \"sourceLocation\": {\n";
    oss << "        \"file\": \"" << file << "\",\n";
    oss << "        \"line\": " << line << "\n";
    oss << "      },\n";

    // Call stack
    oss << "      \"callStack\": [";
    const auto &callStack = state.getCallStack();
    for (size_t j = 0; j < callStack.size(); ++j) {
      oss << "\"" << callStack[j] << "\"";
      if (j < callStack.size() - 1) {
        oss << ", ";
      }
    }
    oss << "],\n";

    oss << "      \"timelineId\": " << state.getTimelineId() << ",\n";
    oss << "      \"threadId\": " << state.getThreadId() << ",\n";

    // Resource usage
    oss << "      \"resourceUsage\": {\n";
    const auto &resources = state.getResourceUsage();
    size_t resourceCount = 0;
    for (const auto &[name, value] : resources) {
      oss << "        \"" << name << "\": " << value;
      if (++resourceCount < resources.size()) {
        oss << ",";
      }
      oss << "\n";
    }
    oss << "      },\n";

    oss << "      \"potentialParadox\": "
        << (state.isPotentialParadox() ? "true" : "false") << "\n";

    oss << "    }";
    if (i < history.getStateCount() - 1) {
      oss << ",";
    }
    oss << "\n";
  }

  oss << "  ]\n";
  oss << "}\n";

  return oss.str();
}

// Simplified HTML visualization of the state history
std::string StateVisualizer::generateHtmlHistoryVisualization(
    const StateHistory &history) const {
  std::ostringstream oss;

  oss << "<!DOCTYPE html>\n";
  oss << "<html>\n";
  oss << "<head>\n";
  oss << "  <title>Chronovyan Debugger State History</title>\n";
  oss << "  <style>\n";
  oss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
  oss << "    h1 { color: #336699; }\n";
  oss << "    .state { margin-bottom: 20px; border: 1px solid #ccc; padding: "
         "10px; }\n";
  oss << "    .state-header { background-color: #f0f0f0; padding: 5px; "
         "margin-bottom: 10px; }\n";
  oss << "    .state-body { margin-left: 20px; }\n";
  oss << "    .paradox { color: red; font-weight: bold; }\n";
  oss << "  </style>\n";
  oss << "</head>\n";
  oss << "<body>\n";

  oss << "  <h1>Chronovyan Debugger State History</h1>\n";
  oss << "  <p>Total States: " << history.getStateCount() << "</p>\n";

  // Show each state
  for (size_t i = 0; i < history.getStateCount(); ++i) {
    const DebuggerState &state = history.getState(i);
    auto [file, line] = state.getSourceLocation();

    oss << "  <div class=\"state\">\n";
    oss << "    <div class=\"state-header\">State " << i << "</div>\n";
    oss << "    <div class=\"state-body\">\n";
    oss << "      <p>Source: " << file << ":" << line << "</p>\n";

    // Call stack
    oss << "      <p>Call Stack: ";
    const auto &callStack = state.getCallStack();
    if (callStack.empty()) {
      oss << "&lt;empty&gt;";
    } else {
      for (size_t j = 0; j < callStack.size(); ++j) {
        oss << callStack[j];
        if (j < callStack.size() - 1) {
          oss << " &rarr; ";
        }
      }
    }
    oss << "</p>\n";

    oss << "      <p>Timeline ID: " << state.getTimelineId() << "</p>\n";
    oss << "      <p>Thread ID: " << state.getThreadId() << "</p>\n";

    // Resource usage
    const auto &resources = state.getResourceUsage();
    if (!resources.empty()) {
      oss << "      <p>Resource Usage:</p>\n";
      oss << "      <ul>\n";
      for (const auto &[name, value] : resources) {
        oss << "        <li>" << name << " = " << value << "</li>\n";
      }
      oss << "      </ul>\n";
    }

    // Show if there's a potential paradox
    if (state.isPotentialParadox()) {
      oss << "      <p class=\"paradox\">*** POTENTIAL PARADOX ***</p>\n";
    }

    oss << "    </div>\n";
    oss << "  </div>\n";
  }

  oss << "</body>\n";
  oss << "</html>\n";

  return oss.str();
}

// Simple text visualization of the temporal flow
std::string StateVisualizer::generateTextTemporalFlowVisualization(
    const StateHistory &history) const {
  std::ostringstream oss;

  oss << "===== Temporal Flow =====\n";

  // Get the timeline branching history
  const auto &timelineBranching = history.getTimelineBranchingHistory();

  // Show timeline branching
  oss << "Timeline Branching:\n";
  if (timelineBranching.empty()) {
    oss << "  <none>\n";
  } else {
    for (const auto &[timeline, parent] : timelineBranching) {
      oss << "  Timeline " << timeline << " branched from Timeline " << parent
          << "\n";
    }
  }

  return oss.str();
}

// Simple JSON visualization of the temporal flow
std::string StateVisualizer::generateJsonTemporalFlowVisualization(
    const StateHistory &history) const {
  std::ostringstream oss;

  // Get the timeline branching history
  const auto &timelineBranching = history.getTimelineBranchingHistory();

  oss << "{\n";
  oss << "  \"timelineBranching\": {\n";

  size_t branchCount = 0;
  for (const auto &[timeline, parent] : timelineBranching) {
    oss << "    \"" << timeline << "\": " << parent;
    if (++branchCount < timelineBranching.size()) {
      oss << ",";
    }
    oss << "\n";
  }

  oss << "  }\n";
  oss << "}\n";

  return oss.str();
}

// Simple HTML visualization of the temporal flow
std::string StateVisualizer::generateHtmlTemporalFlowVisualization(
    const StateHistory &history) const {
  std::ostringstream oss;

  oss << "<!DOCTYPE html>\n";
  oss << "<html>\n";
  oss << "<head>\n";
  oss << "  <title>Chronovyan Temporal Flow</title>\n";
  oss << "  <style>\n";
  oss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
  oss << "    h1 { color: #336699; }\n";
  oss << "    .timeline { margin-bottom: 5px; }\n";
  oss << "  </style>\n";
  oss << "</head>\n";
  oss << "<body>\n";

  oss << "  <h1>Chronovyan Temporal Flow</h1>\n";

  // Get the timeline branching history
  const auto &timelineBranching = history.getTimelineBranchingHistory();

  // Show timeline branching
  oss << "  <h2>Timeline Branching</h2>\n";
  if (timelineBranching.empty()) {
    oss << "  <p>&lt;none&gt;</p>\n";
  } else {
    oss << "  <ul>\n";
    for (const auto &[timeline, parent] : timelineBranching) {
      oss << "    <li class=\"timeline\">Timeline " << timeline
          << " branched from Timeline " << parent << "</li>\n";
    }
    oss << "  </ul>\n";
  }

  oss << "</body>\n";
  oss << "</html>\n";

  return oss.str();
}

} // namespace compiler
} // namespace chronovyan