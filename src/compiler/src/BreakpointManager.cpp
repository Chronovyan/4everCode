#include <algorithm>

#include "compiler/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {

BreakpointManager::BreakpointManager()
    : nextBreakpointId(1), hitBreakpointId(-1) {
  // Initialize with empty values
}

BreakpointManager::~BreakpointManager() = default;

int BreakpointManager::setBreakpoint(const std::string &sourceFile, int line,
                                     const BreakpointCondition &condition) {
  // Check if a breakpoint already exists at this location
  for (const auto &[id, bp] : breakpoints) {
    if (bp.sourceFile == sourceFile && bp.line == line) {
      // Update the condition and return the existing ID
      breakpoints[id].condition = condition;
      return id;
    }
  }

  // Create a new breakpoint
  int id = nextBreakpointId++;
  breakpoints[id] = {sourceFile, line, condition};
  return id;
}

bool BreakpointManager::removeBreakpoint(int breakpointId) {
  return breakpoints.erase(breakpointId) > 0;
}

bool BreakpointManager::isBreakpointHit(const DebuggerState &state) const {
  hitBreakpointId = -1;

  // Get the current source location
  auto [file, line] = state.getSourceLocation();

  // Check if any breakpoint is hit
  for (const auto &[id, bp] : breakpoints) {
    if (isSpecificBreakpointHit(id, bp, state)) {
      hitBreakpointId = id;
      return true;
    }
  }

  return false;
}

int BreakpointManager::getHitBreakpointId() const { return hitBreakpointId; }

std::map<int, std::pair<std::string, int>>
BreakpointManager::getAllBreakpoints() const {
  std::map<int, std::pair<std::string, int>> result;
  for (const auto &[id, bp] : breakpoints) {
    result[id] = {bp.sourceFile, bp.line};
  }
  return result;
}

bool BreakpointManager::isSpecificBreakpointHit(
    int breakpointId, const Breakpoint &bp, const DebuggerState &state) const {
  // Get the current source location
  auto [file, line] = state.getSourceLocation();

  // Check if the location matches
  if (bp.sourceFile == file && bp.line == line) {
    // If there's no condition, the breakpoint is hit
    if (bp.condition.type == BreakpointType::LOCATION) {
      return true;
    }

    // Otherwise, evaluate the condition
    return evaluateCondition(bp.condition, state);
  }

  // Check for non-location breakpoints
  if (bp.condition.type != BreakpointType::LOCATION) {
    return evaluateCondition(bp.condition, state);
  }

  return false;
}

bool BreakpointManager::evaluateCondition(const BreakpointCondition &condition,
                                          const DebuggerState &state) const {
  switch (condition.type) {
  case BreakpointType::LOCATION:
    // Already checked in isSpecificBreakpointHit
    return true;

  case BreakpointType::VARIABLE_CHANGE: {
    // Check if the variable has changed
    // We need the variable name from the parameters
    auto it = condition.parameters.find("variable");
    if (it == condition.parameters.end()) {
      return false;
    }

    const std::string &varName = it->second;

    // Get the variable value
    const auto &localVars = state.getLocalVariables();
    auto localIt = localVars.find(varName);
    if (localIt != localVars.end()) {
      // If there's a value parameter, check if the value matches
      auto valueIt = condition.parameters.find("value");
      if (valueIt != condition.parameters.end()) {
        return localIt->second == valueIt->second;
      }

      // Otherwise, just check if the variable exists
      return true;
    }

    // Check global variables
    const auto &globalVars = state.getGlobalVariables();
    auto globalIt = globalVars.find(varName);
    if (globalIt != globalVars.end()) {
      // If there's a value parameter, check if the value matches
      auto valueIt = condition.parameters.find("value");
      if (valueIt != condition.parameters.end()) {
        return globalIt->second == valueIt->second;
      }

      // Otherwise, just check if the variable exists
      return true;
    }

    return false;
  }

  case BreakpointType::RESOURCE_THRESHOLD: {
    // Check if a resource has reached a threshold
    // We need the resource name and threshold from the parameters
    auto nameIt = condition.parameters.find("resource");
    auto thresholdIt = condition.parameters.find("threshold");
    if (nameIt == condition.parameters.end() ||
        thresholdIt == condition.parameters.end()) {
      return false;
    }

    const std::string &resourceName = nameIt->second;
    double threshold = std::stod(thresholdIt->second);

    // Get the resource usage
    const auto &resources = state.getResourceUsage();
    auto resourceIt = resources.find(resourceName);
    if (resourceIt != resources.end()) {
      // Check if the usage is above the threshold
      return resourceIt->second >= threshold;
    }

    return false;
  }

  case BreakpointType::TIMELINE_BRANCH:
    // Break when a timeline branches
    // This would typically be triggered by specific bytecode instructions
    // For demonstration, we'll just check if the timeline ID is non-zero
    return state.getTimelineId() != 0;

  case BreakpointType::TIMELINE_MERGE:
    // Break when timelines merge
    // This would typically be triggered by specific bytecode instructions
    // For demonstration, we'll just check if we're in a specific function
    return std::find(state.getCallStack().begin(), state.getCallStack().end(),
                     "mergeTimelines") != state.getCallStack().end();

  case BreakpointType::PARADOX_RISK:
    // Break when a potential paradox is detected
    return state.isPotentialParadox();

  case BreakpointType::TEMPORAL_OPERATION: {
    // Break on specific temporal operations
    // We need the operation name from the parameters
    auto opIt = condition.parameters.find("operation");
    if (opIt == condition.parameters.end()) {
      return false;
    }

    const std::string &operation = opIt->second;

    // For demonstration, we'll just check if we're in a function with a
    // matching name
    for (const auto &funcName : state.getCallStack()) {
      if (funcName.find(operation) != std::string::npos) {
        return true;
      }
    }

    return false;
  }

  default:
    return false;
  }
}

} // namespace compiler
} // namespace chronovyan