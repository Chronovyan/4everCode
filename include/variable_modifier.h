#ifndef CHRONOVYAN_VARIABLE_MODIFIER_H
#define CHRONOVYAN_VARIABLE_MODIFIER_H

namespace chronovyan {

/**
 * @enum VariableModifier
 * @brief Defines the variable modifiers (CONF or REB)
 */
enum class VariableModifier {
    CONF,  // Conformist path - deterministic, stable
    REB    // Rebel path - probabilistic, quantum
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_VARIABLE_MODIFIER_H