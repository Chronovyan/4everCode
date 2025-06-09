#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "bytecode/BytecodeModule.h"
#include "compiler/BytecodeGenerator.h"
#include "compiler/DiagnosticReporter.h"
#include "compiler/IRGenerator.h"

namespace chronovyan {
namespace compiler {

// Forward declarations
class DebuggerState;
class StateHistory;
class BreakpointManager;
class StateVisualizer;

/**
 * @enum BreakpointType
 * @brief Types of breakpoints supported by the debugger
 */
enum class BreakpointType {
  LOCATION,           // Break at a specific source location
  VARIABLE_CHANGE,    // Break when a variable changes
  RESOURCE_THRESHOLD, // Break when resource usage reaches a threshold
  TIMELINE_BRANCH,    // Break when a timeline branches
  TIMELINE_MERGE,     // Break when timelines merge
  PARADOX_RISK,       // Break when a potential paradox is detected
  TEMPORAL_OPERATION  // Break on specific temporal operations
};

/**
 * @struct BreakpointCondition
 * @brief Defines a condition for a breakpoint
 */
struct BreakpointCondition {
  BreakpointType type;
  std::string expression; // Optional expression for conditional breakpoints
  std::map<std::string, std::string>
      parameters; // Additional parameters for the breakpoint
};

/**
 * @class DejaVuDebugger
 * @brief Main debugger class for the Chronovyan language.
 *
 * The DejaVu Debugger provides specialized debugging capabilities for temporal
 * programs, including state history tracking, temporal breakpoints, and state
 * visualization.
 */
class DejaVuDebugger {
public:
  /**
   * @brief Constructor
   * @param diagnostics Diagnostic reporter for reporting errors
   */
  explicit DejaVuDebugger(DiagnosticReporter &diagnostics);

  /**
   * @brief Destructor
   */
  ~DejaVuDebugger();

  /**
   * @brief Initialize the debugger with a bytecode module
   * @param module The bytecode module to debug
   * @return True if initialization was successful, false otherwise
   */
  bool initialize(const bytecode::BytecodeModule &module);

  /**
   * @brief Start the debugging session
   * @return True if the session started successfully, false otherwise
   */
  bool start();

  /**
   * @brief Stop the debugging session
   */
  void stop();

  /**
   * @brief Step to the next instruction
   * @return True if the step was successful, false otherwise
   */
  bool stepInto();

  /**
   * @brief Step over the current function call
   * @return True if the step was successful, false otherwise
   */
  bool stepOver();

  /**
   * @brief Step out of the current function
   * @return True if the step was successful, false otherwise
   */
  bool stepOut();

  /**
   * @brief Continue execution until the next breakpoint
   * @return True if continuation was successful, false otherwise
   */
  bool continueExecution();

  /**
   * @brief Set a breakpoint
   * @param sourceFile The source file where the breakpoint is set
   * @param line The line number where the breakpoint is set
   * @param condition Optional condition for the breakpoint
   * @return The ID of the created breakpoint, or -1 if creation failed
   */
  int setBreakpoint(const std::string &sourceFile, int line,
                    const BreakpointCondition &condition = {
                        BreakpointType::LOCATION, "", {}});

  /**
   * @brief Remove a breakpoint
   * @param breakpointId The ID of the breakpoint to remove
   * @return True if the breakpoint was removed, false otherwise
   */
  bool removeBreakpoint(int breakpointId);

  /**
   * @brief Get the current state of the program
   * @return The current debugger state
   */
  const DebuggerState &getCurrentState() const;

  /**
   * @brief Get the state history
   * @return The state history
   */
  const StateHistory &getStateHistory() const;

  /**
   * @brief Get the value of a variable at the current point
   * @param variableName The name of the variable
   * @return The value of the variable as a string
   */
  std::string getVariableValue(const std::string &variableName) const;

  /**
   * @brief Get the value of a variable at a specific point in history
   * @param variableName The name of the variable
   * @param historyIndex The index in the state history
   * @return The value of the variable as a string
   */
  std::string getVariableValueAt(const std::string &variableName,
                                 int historyIndex) const;

  /**
   * @brief Get the current timeline ID
   * @return The current timeline ID
   */
  int getCurrentTimelineId() const;

  /**
   * @brief Get the current resource usage
   * @return A map of resource names to their current usage values
   */
  std::map<std::string, double> getCurrentResourceUsage() const;

  /**
   * @brief Register a callback for breakpoint events
   * @param callback The callback function to register
   * @return A handle that can be used to unregister the callback
   */
  int registerBreakpointCallback(
      std::function<void(const DebuggerState &)> callback);

  /**
   * @brief Unregister a breakpoint callback
   * @param handle The handle of the callback to unregister
   * @return True if the callback was unregistered, false otherwise
   */
  bool unregisterBreakpointCallback(int handle);

  /**
   * @brief Generate a visualization of the current state
   * @param format The format of the visualization (e.g., "text", "json",
   * "html")
   * @return The visualization as a string
   */
  std::string visualizeCurrentState(const std::string &format = "text") const;

  /**
   * @brief Generate a visualization of the state history
   * @param format The format of the visualization (e.g., "text", "json",
   * "html")
   * @return The visualization as a string
   */
  std::string visualizeStateHistory(const std::string &format = "text") const;

  /**
   * @brief Generate a visualization of the temporal flow
   * @param format The format of the visualization (e.g., "text", "json",
   * "html")
   * @return The visualization as a string
   */
  std::string visualizeTemporalFlow(const std::string &format = "text") const;

private:
  DiagnosticReporter &diagnostics; ///< Diagnostic reporter
  std::unique_ptr<DebuggerState>
      currentState;                           ///< Current state of the debugger
  std::unique_ptr<StateHistory> stateHistory; ///< History of program states
  std::unique_ptr<BreakpointManager> breakpointMgr; ///< Breakpoint manager
  std::unique_ptr<StateVisualizer> stateVisualizer; ///< State visualizer
  bytecode::BytecodeModule bytecodeModule; ///< Bytecode module being debugged
  bool isRunning; ///< Whether the debugger is currently running

  // Map of callback handles to callback functions
  std::unordered_map<int, std::function<void(const DebuggerState &)>>
      breakpointCallbacks;
  int nextCallbackHandle; ///< Next handle to assign to a callback

  /**
   * @brief Check if any breakpoints are hit at the current position
   * @return True if a breakpoint is hit, false otherwise
   */
  bool checkBreakpoints();

  /**
   * @brief Execute a single instruction
   * @return True if the instruction was executed successfully, false otherwise
   */
  bool executeInstruction();

  /**
   * @brief Update the state history with the current state
   */
  void updateStateHistory();

  /**
   * @brief Notify all registered callbacks about a breakpoint hit
   */
  void notifyBreakpointCallbacks();
};

/**
 * @class DebuggerState
 * @brief Represents the current state of the program being debugged
 */
class DebuggerState {
public:
  DebuggerState();
  ~DebuggerState();

  /**
   * @brief Get the current source location
   * @return A pair containing the source file and line number
   */
  std::pair<std::string, int> getSourceLocation() const;

  /**
   * @brief Get the current instruction pointer
   * @return The current instruction pointer
   */
  size_t getInstructionPointer() const;

  /**
   * @brief Get the current call stack
   * @return The call stack as a vector of function names
   */
  std::vector<std::string> getCallStack() const;

  /**
   * @brief Get all local variables and their values
   * @return A map of variable names to their values
   */
  std::map<std::string, std::string> getLocalVariables() const;

  /**
   * @brief Get all global variables and their values
   * @return A map of variable names to their values
   */
  std::map<std::string, std::string> getGlobalVariables() const;

  /**
   * @brief Get the current timeline ID
   * @return The current timeline ID
   */
  int getTimelineId() const;

  /**
   * @brief Get the current thread ID (for parallel execution)
   * @return The current thread ID
   */
  int getThreadId() const;

  /**
   * @brief Get the current resource usage
   * @return A map of resource names to their current usage values
   */
  std::map<std::string, double> getResourceUsage() const;

  /**
   * @brief Check if the state represents a potential paradox
   * @return True if a potential paradox is detected, false otherwise
   */
  bool isPotentialParadox() const;

  // Methods to update the state (used internally by the debugger)
  void setSourceLocation(const std::string &file, int line);
  void setInstructionPointer(size_t ip);
  void pushCallStack(const std::string &functionName);
  void popCallStack();
  void setLocalVariable(const std::string &name, const std::string &value);
  void setGlobalVariable(const std::string &name, const std::string &value);
  void setTimelineId(int id);
  void setThreadId(int id);
  void setResourceUsage(const std::string &resource, double value);
  void setPotentialParadox(bool value);

private:
  std::string sourceFile;                       ///< Current source file
  int sourceLine;                               ///< Current line number
  size_t instructionPointer;                    ///< Current instruction pointer
  std::vector<std::string> callStack;           ///< Call stack
  std::map<std::string, std::string> localVars; ///< Local variables
  std::map<std::string, std::string> globalVars; ///< Global variables
  int timelineId;                                ///< Current timeline ID
  int threadId;                                  ///< Current thread ID
  std::map<std::string, double> resourceUsage;   ///< Resource usage
  bool potentialParadox; ///< Whether a potential paradox is detected
};

/**
 * @class StateHistory
 * @brief Manages the history of program states
 */
class StateHistory {
public:
  StateHistory();
  ~StateHistory();

  /**
   * @brief Add a state to the history
   * @param state The state to add
   */
  void addState(const DebuggerState &state);

  /**
   * @brief Get a state from the history
   * @param index The index of the state to get
   * @return The state at the specified index
   */
  const DebuggerState &getState(size_t index) const;

  /**
   * @brief Get the number of states in the history
   * @return The number of states
   */
  size_t getStateCount() const;

  /**
   * @brief Clear the state history
   */
  void clear();

  /**
   * @brief Get the history of a specific variable
   * @param variableName The name of the variable
   * @return A vector of values for the variable across the history
   */
  std::vector<std::string>
  getVariableHistory(const std::string &variableName) const;

  /**
   * @brief Get the history of resource usage
   * @param resourceName The name of the resource
   * @return A vector of resource usage values across the history
   */
  std::vector<double> getResourceHistory(const std::string &resourceName) const;

  /**
   * @brief Get the timeline branching history
   * @return A map of timeline IDs to their parent timeline IDs
   */
  std::map<int, int> getTimelineBranchingHistory() const;

private:
  std::vector<DebuggerState> states; ///< History of states
  std::map<std::string, std::vector<std::string>>
      variableHistory; ///< History of variable values
  std::map<std::string, std::vector<double>>
      resourceHistory;                  ///< History of resource usage
  std::map<int, int> timelineBranching; ///< Timeline branching history
                                        ///< (timeline ID -> parent timeline ID)

  /**
   * @brief Update the variable history with a new state
   * @param state The new state
   */
  void updateVariableHistory(const DebuggerState &state);

  /**
   * @brief Update the resource history with a new state
   * @param state The new state
   */
  void updateResourceHistory(const DebuggerState &state);

  /**
   * @brief Update the timeline branching history with a new state
   * @param state The new state
   */
  void updateTimelineBranchingHistory(const DebuggerState &state);
};

/**
 * @class BreakpointManager
 * @brief Manages breakpoints for the debugger
 */
class BreakpointManager {
public:
  BreakpointManager();
  ~BreakpointManager();

  /**
   * @brief Set a breakpoint
   * @param sourceFile The source file where the breakpoint is set
   * @param line The line number where the breakpoint is set
   * @param condition Optional condition for the breakpoint
   * @return The ID of the created breakpoint, or -1 if creation failed
   */
  int setBreakpoint(const std::string &sourceFile, int line,
                    const BreakpointCondition &condition);

  /**
   * @brief Remove a breakpoint
   * @param breakpointId The ID of the breakpoint to remove
   * @return True if the breakpoint was removed, false otherwise
   */
  bool removeBreakpoint(int breakpointId);

  /**
   * @brief Check if any breakpoints are hit with the current state
   * @param state The current debugger state
   * @return True if a breakpoint is hit, false otherwise
   */
  bool isBreakpointHit(const DebuggerState &state) const;

  /**
   * @brief Get the ID of the hit breakpoint
   * @return The ID of the hit breakpoint, or -1 if no breakpoint is hit
   */
  int getHitBreakpointId() const;

  /**
   * @brief Get information about all breakpoints
   * @return A map of breakpoint IDs to their information
   */
  std::map<int, std::pair<std::string, int>> getAllBreakpoints() const;

private:
  struct Breakpoint {
    std::string sourceFile;
    int line;
    BreakpointCondition condition;
  };

  std::map<int, Breakpoint>
      breakpoints;      ///< Map of breakpoint IDs to breakpoint information
  int nextBreakpointId; ///< Next ID to assign to a breakpoint
  mutable int hitBreakpointId; ///< ID of the last hit breakpoint

  /**
   * @brief Check if a specific breakpoint is hit with the current state
   * @param breakpointId The ID of the breakpoint to check
   * @param state The current debugger state
   * @return True if the breakpoint is hit, false otherwise
   */
  bool isSpecificBreakpointHit(int breakpointId, const Breakpoint &bp,
                               const DebuggerState &state) const;

  /**
   * @brief Evaluate a condition for a breakpoint
   * @param condition The condition to evaluate
   * @param state The current debugger state
   * @return True if the condition is met, false otherwise
   */
  bool evaluateCondition(const BreakpointCondition &condition,
                         const DebuggerState &state) const;
};

/**
 * @class StateVisualizer
 * @brief Visualizes program states and temporal flow
 */
class StateVisualizer {
public:
  StateVisualizer();
  ~StateVisualizer();

  /**
   * @brief Generate a visualization of a single state
   * @param state The state to visualize
   * @param format The format of the visualization (e.g., "text", "json",
   * "html")
   * @return The visualization as a string
   */
  std::string visualizeState(const DebuggerState &state,
                             const std::string &format) const;

  /**
   * @brief Generate a visualization of the state history
   * @param history The state history to visualize
   * @param format The format of the visualization (e.g., "text", "json",
   * "html")
   * @return The visualization as a string
   */
  std::string visualizeHistory(const StateHistory &history,
                               const std::string &format) const;

  /**
   * @brief Generate a visualization of the temporal flow
   * @param history The state history containing timeline information
   * @param format The format of the visualization (e.g., "text", "json",
   * "html")
   * @return The visualization as a string
   */
  std::string visualizeTemporalFlow(const StateHistory &history,
                                    const std::string &format) const;

private:
  /**
   * @brief Generate a text visualization of a state
   * @param state The state to visualize
   * @return The visualization as a string
   */
  std::string generateTextVisualization(const DebuggerState &state) const;

  /**
   * @brief Generate a JSON visualization of a state
   * @param state The state to visualize
   * @return The visualization as a string
   */
  std::string generateJsonVisualization(const DebuggerState &state) const;

  /**
   * @brief Generate an HTML visualization of a state
   * @param state The state to visualize
   * @return The visualization as a string
   */
  std::string generateHtmlVisualization(const DebuggerState &state) const;

  /**
   * @brief Generate a text visualization of the state history
   * @param history The state history to visualize
   * @return The visualization as a string
   */
  std::string
  generateTextHistoryVisualization(const StateHistory &history) const;

  /**
   * @brief Generate a JSON visualization of the state history
   * @param history The state history to visualize
   * @return The visualization as a string
   */
  std::string
  generateJsonHistoryVisualization(const StateHistory &history) const;

  /**
   * @brief Generate an HTML visualization of the state history
   * @param history The state history to visualize
   * @return The visualization as a string
   */
  std::string
  generateHtmlHistoryVisualization(const StateHistory &history) const;

  /**
   * @brief Generate a text visualization of the temporal flow
   * @param history The state history containing timeline information
   * @return The visualization as a string
   */
  std::string
  generateTextTemporalFlowVisualization(const StateHistory &history) const;

  /**
   * @brief Generate a JSON visualization of the temporal flow
   * @param history The state history containing timeline information
   * @return The visualization as a string
   */
  std::string
  generateJsonTemporalFlowVisualization(const StateHistory &history) const;

  /**
   * @brief Generate an HTML visualization of the temporal flow
   * @param history The state history containing timeline information
   * @return The visualization as a string
   */
  std::string
  generateHtmlTemporalFlowVisualization(const StateHistory &history) const;
};

} // namespace compiler
} // namespace chronovyan