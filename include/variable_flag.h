#ifndef CHRONOVYAN_VARIABLE_FLAG_H
#define CHRONOVYAN_VARIABLE_FLAG_H

namespace chronovyan {

/**
 * @enum VariableFlag
 * @brief Defines the variable flags that modify behavior
 */
enum class VariableFlag {
    STATIC,    // Immutable once initialized
    VOLATILE,  // Can change across timeline operations
    ANCHOR,    // Creates a fixed point in the timeline
    WEAVER,    // Can influence multiple timelines
    FLUX,      // Can have quantum fluctuations
    ECHO,      // Retains history of previous values
    READONLY,  // Variable is read-only
    TEMPORAL,  // Variable has temporal properties
    MUTABLE    // Variable can be modified
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_VARIABLE_FLAG_H