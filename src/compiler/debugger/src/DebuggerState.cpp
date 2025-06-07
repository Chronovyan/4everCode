#include <algorithm>

#include "compiler/debugger/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {
namespace debugger {

// DebuggerState implementation
DebuggerState::DebuggerState()
    : instructionPointer(0), timelineId(0), threadId(0), potentialParadox(false) {}

DebuggerState::DebuggerState(const DebuggerState& other)
    : instructionPointer(other.instructionPointer)
    , callStack(other.callStack)
    , sourceLocation(other.sourceLocation)
    , localVariables(other.localVariables)
    , globalVariables(other.globalVariables)
    , resourceUsage(other.resourceUsage)
    , timelineId(other.timelineId)
    , threadId(other.threadId)
    , potentialParadox(other.potentialParadox) {}

DebuggerState& DebuggerState::operator=(const DebuggerState& other) {
    if (this != &other) {
        instructionPointer = other.instructionPointer;
        callStack = other.callStack;
        sourceLocation = other.sourceLocation;
        localVariables = other.localVariables;
        globalVariables = other.globalVariables;
        resourceUsage = other.resourceUsage;
        timelineId = other.timelineId;
        threadId = other.threadId;
        potentialParadox = other.potentialParadox;
    }
    return *this;
}

void DebuggerState::setInstructionPointer(size_t ip) { instructionPointer = ip; }

size_t DebuggerState::getInstructionPointer() const { return instructionPointer; }

void DebuggerState::pushCallStack(const std::string& functionName) {
    callStack.push_back(functionName);
}

void DebuggerState::popCallStack() {
    if (!callStack.empty()) {
        callStack.pop_back();
    }
}

const std::vector<std::string>& DebuggerState::getCallStack() const { return callStack; }

void DebuggerState::setSourceLocation(const std::string& file, int line) {
    sourceLocation.file = file;
    sourceLocation.line = line;
}

const SourceLocation& DebuggerState::getSourceLocation() const { return sourceLocation; }

void DebuggerState::setLocalVariable(const std::string& name, const std::string& value) {
    localVariables[name] = value;
}

void DebuggerState::clearLocalVariables() { localVariables.clear(); }

const std::map<std::string, std::string>& DebuggerState::getLocalVariables() const {
    return localVariables;
}

void DebuggerState::setGlobalVariable(const std::string& name, const std::string& value) {
    globalVariables[name] = value;
}

const std::map<std::string, std::string>& DebuggerState::getGlobalVariables() const {
    return globalVariables;
}

void DebuggerState::setResourceUsage(const std::string& resource, double amount) {
    resourceUsage[resource] = amount;
}

std::map<std::string, double> DebuggerState::getResourceUsage() const { return resourceUsage; }

void DebuggerState::setTimelineId(int id) { timelineId = id; }

int DebuggerState::getTimelineId() const { return timelineId; }

void DebuggerState::setThreadId(int id) { threadId = id; }

int DebuggerState::getThreadId() const { return threadId; }

bool DebuggerState::isPotentialParadox() const { return potentialParadox; }

void DebuggerState::setPotentialParadox(bool value) { potentialParadox = value; }

}  // namespace debugger
}  // namespace compiler
}  // namespace chronovyan