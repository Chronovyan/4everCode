#include "compiler/debugger/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {
namespace debugger {

// BreakpointManager implementation
BreakpointManager::BreakpointManager() : nextBreakpointId(1) {}

BreakpointManager::~BreakpointManager() = default;

int BreakpointManager::setBreakpoint(const std::string &sourceFile, int line,
                                     const BreakpointCondition &condition) {
  int id = nextBreakpointId++;

  Breakpoint bp;
  bp.id = id;
  bp.sourceFile = sourceFile;
  bp.line = line;
  bp.type = BreakpointType::LINE;
  bp.condition = condition;

  breakpoints[id] = bp;

  return id;
}

bool BreakpointManager::removeBreakpoint(int id) {
  return breakpoints.erase(id) > 0;
}

bool BreakpointManager::isBreakpointHit(const DebuggerState &state) {
  const SourceLocation &location = state.getSourceLocation();

  for (const auto &[id, bp] : breakpoints) {
    // Skip disabled breakpoints
    if (!bp.condition.enabled) {
      continue;
    }

    // Check if the location matches
    if (bp.type == BreakpointType::LINE && bp.sourceFile == location.file &&
        bp.line == location.line) {
      // If there's a condition, evaluate it
      if (!bp.condition.expression.empty()) {
        // In a real implementation, we would evaluate the condition here
        // For now, just assume all conditions are true
        return true;
      } else {
        return true;
      }
    }
  }

  return false;
}

} // namespace debugger
} // namespace compiler
} // namespace chronovyan