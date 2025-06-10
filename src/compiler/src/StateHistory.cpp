#include "compiler/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {

StateHistory::StateHistory() = default;

StateHistory::~StateHistory() = default;

void StateHistory::addState(const DebuggerState &state) {
  states.push_back(state);

  // Update the variable history
  updateVariableHistory(state);

  // Update the resource history
  updateResourceHistory(state);

  // Update the timeline branching history
  updateTimelineBranchingHistory(state);
}

const DebuggerState &StateHistory::getState(size_t index) const {
  if (index >= states.size()) {
    static DebuggerState emptyState;
    return emptyState;
  }

  return states[index];
}

size_t StateHistory::getStateCount() const { return states.size(); }

void StateHistory::clear() {
  states.clear();
  variableHistory.clear();
  resourceHistory.clear();
  timelineBranching.clear();
}

std::vector<std::string>
StateHistory::getVariableHistory(const std::string &variableName) const {
  auto it = variableHistory.find(variableName);
  if (it != variableHistory.end()) {
    return it->second;
  }

  return {};
}

std::vector<double>
StateHistory::getResourceHistory(const std::string &resourceName) const {
  auto it = resourceHistory.find(resourceName);
  if (it != resourceHistory.end()) {
    return it->second;
  }

  return {};
}

std::map<int, int> StateHistory::getTimelineBranchingHistory() const {
  return timelineBranching;
}

void StateHistory::updateVariableHistory(const DebuggerState &state) {
  // Update local variables
  const auto &localVars = state.getLocalVariables();
  for (const auto &[name, value] : localVars) {
    variableHistory[name].push_back(value);
  }

  // Update global variables
  const auto &globalVars = state.getGlobalVariables();
  for (const auto &[name, value] : globalVars) {
    variableHistory[name].push_back(value);
  }
}

void StateHistory::updateResourceHistory(const DebuggerState &state) {
  const auto &resources = state.getResourceUsage();
  for (const auto &[name, value] : resources) {
    resourceHistory[name].push_back(value);
  }
}

void StateHistory::updateTimelineBranchingHistory(const DebuggerState &state) {
  // If this is a new timeline ID, record it
  int timelineId = state.getTimelineId();
  if (timelineId > 0 &&
      timelineBranching.find(timelineId) == timelineBranching.end()) {
    // For simplicity, assume the parent is always timeline 0
    // In a real implementation, we would need to track the actual parent
    timelineBranching[timelineId] = 0;
  }
}

} // namespace compiler
} // namespace chronovyan