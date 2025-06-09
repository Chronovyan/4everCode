#include <stdexcept>

#include "compiler/debugger/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {
namespace debugger {

// StateHistory implementation
StateHistory::StateHistory() {
  // Initialize with an empty vector of states
}

StateHistory::~StateHistory() = default;

void StateHistory::addState(const DebuggerState &state) {
  states.push_back(state);
}

const DebuggerState &StateHistory::getState(size_t index) const {
  if (index >= states.size()) {
    throw std::out_of_range("State history index out of range");
  }
  return states[index];
}

size_t StateHistory::getStateCount() const { return states.size(); }

void StateHistory::clear() { states.clear(); }

} // namespace debugger
} // namespace compiler
} // namespace chronovyan