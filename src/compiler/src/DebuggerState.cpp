#include "compiler/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {

DebuggerState::DebuggerState()
    : sourceLine(0), instructionPointer(0), timelineId(0), threadId(0), potentialParadox(false) {
    // Initialize with empty values
}

DebuggerState::~DebuggerState() = default;

std::pair<std::string, int> DebuggerState::getSourceLocation() const {
    return {sourceFile, sourceLine};
}

size_t DebuggerState::getInstructionPointer() const { return instructionPointer; }

std::vector<std::string> DebuggerState::getCallStack() const { return callStack; }

std::map<std::string, std::string> DebuggerState::getLocalVariables() const { return localVars; }

std::map<std::string, std::string> DebuggerState::getGlobalVariables() const { return globalVars; }

int DebuggerState::getTimelineId() const { return timelineId; }

int DebuggerState::getThreadId() const { return threadId; }

std::map<std::string, double> DebuggerState::getResourceUsage() const { return resourceUsage; }

bool DebuggerState::isPotentialParadox() const { return potentialParadox; }

void DebuggerState::setSourceLocation(const std::string& file, int line) {
    sourceFile = file;
    sourceLine = line;
}

void DebuggerState::setInstructionPointer(size_t ip) { instructionPointer = ip; }

void DebuggerState::pushCallStack(const std::string& functionName) {
    callStack.push_back(functionName);
}

void DebuggerState::popCallStack() {
    if (!callStack.empty()) {
        callStack.pop_back();
    }
}

void DebuggerState::setLocalVariable(const std::string& name, const std::string& value) {
    localVars[name] = value;
}

void DebuggerState::setGlobalVariable(const std::string& name, const std::string& value) {
    globalVars[name] = value;
}

void DebuggerState::setTimelineId(int id) { timelineId = id; }

void DebuggerState::setThreadId(int id) { threadId = id; }

void DebuggerState::setResourceUsage(const std::string& resource, double value) {
    resourceUsage[resource] = value;
}

void DebuggerState::setPotentialParadox(bool value) { potentialParadox = value; }

}  // namespace compiler
}  // namespace chronovyan