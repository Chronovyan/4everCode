#ifndef CHRONOVYAN_REBEL_OPERATION_H
#define CHRONOVYAN_REBEL_OPERATION_H

namespace chronovyan {

/**
 * @enum RebelOperationType
 * @brief Types of rebel operations that can affect the timeline
 *
 * Rebel operations are actions that alter the normal flow of time,
 * potentially creating temporal debt or instability.
 */
enum class RebelOperationType {
    NONE,                  // No specific operation type
    REWIND_FLOW,           // Rewinding the flow of time
    TEMPORAL_ECHO_LOOP,    // Creating an echo in the time loop
    QUANTUM_MANIPULATION,  // Manipulating quantum states
    SUSPEND_TIMELINE,      // Suspending a timeline temporarily
    SHATTER_ITERATION,     // Shattering a time iteration
    TIMELINE_ALTERATION,   // As per CD-2023-08-004 for temporal_debt_tracker.cpp
    OBSERVER_EFFECT,       // As per CD-2023-08-004 for temporal_debt_tracker.cpp
    CAUSALITY_VIOLATION,   // As per CD-2023-08-004 for temporal_debt_tracker.cpp
    TIMELINE_CONVERGENCE,  // New operation type
    PARADOX_CREATION,      // New operation type
    UNKNOWN,               // New operation type
    BRANCH_CREATION,       // New operation type
    TIMELINE_INJECTION,    // New operation type
    TEMPORAL_BORROWING,    // New operation type
    LOOP_CREATION          // New operation type
    // Potentially other operations if discovered or needed later
};

/**
 * @brief Get a string representation of a rebel operation type
 * @param type The rebel operation type
 * @return String representation of the operation type
 */
const char* getRebelOperationTypeName(RebelOperationType type);

}  // namespace chronovyan

#endif  // CHRONOVYAN_REBEL_OPERATION_H