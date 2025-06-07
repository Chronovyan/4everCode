#include <algorithm>
#include <unordered_set>

#include "compiler/proving_grounds/ParadoxDetector.h"

namespace chronovyan {
namespace compiler {
namespace proving_grounds {

// Rule names
const std::string RULE_RESOURCE_VIOLATIONS = "resource_violations";
const std::string RULE_TIMELINE_INCONSISTENCIES = "timeline_inconsistencies";
const std::string RULE_CAUSALITY_VIOLATIONS = "causality_violations";
const std::string RULE_VARIABLE_STATE_INCONSISTENCIES = "variable_state_inconsistencies";
const std::string RULE_THREAD_SYNCHRONIZATION = "thread_synchronization";

ParadoxDetector::ParadoxDetector(DiagnosticReporter& diag)
    : diagnostics(diag), severityThreshold(5) {
    // Enable all rules by default
    ruleEnabled[RULE_RESOURCE_VIOLATIONS] = true;
    ruleEnabled[RULE_TIMELINE_INCONSISTENCIES] = true;
    ruleEnabled[RULE_CAUSALITY_VIOLATIONS] = true;
    ruleEnabled[RULE_VARIABLE_STATE_INCONSISTENCIES] = true;
    ruleEnabled[RULE_THREAD_SYNCHRONIZATION] = true;
}

ParadoxDetector::~ParadoxDetector() = default;

std::vector<ParadoxDetector::Paradox> ParadoxDetector::detectParadoxes(
    const debugger::DejaVuDebugger& debugger) const {
    std::vector<Paradox> paradoxes;

    // Run each enabled paradox detection rule
    if (isRuleEnabled(RULE_RESOURCE_VIOLATIONS)) {
        detectResourceViolations(debugger, paradoxes);
    }

    if (isRuleEnabled(RULE_TIMELINE_INCONSISTENCIES)) {
        detectTimelineInconsistencies(debugger, paradoxes);
    }

    if (isRuleEnabled(RULE_CAUSALITY_VIOLATIONS)) {
        detectCausalityViolations(debugger, paradoxes);
    }

    if (isRuleEnabled(RULE_VARIABLE_STATE_INCONSISTENCIES)) {
        detectVariableStateInconsistencies(debugger, paradoxes);
    }

    if (isRuleEnabled(RULE_THREAD_SYNCHRONIZATION)) {
        detectThreadSynchronizationParadoxes(debugger, paradoxes);
    }

    return paradoxes;
}

void ParadoxDetector::setRuleEnabled(const std::string& ruleName, bool enabled) {
    if (ruleEnabled.find(ruleName) != ruleEnabled.end()) {
        ruleEnabled[ruleName] = enabled;
    } else {
        diagnostics.addWarning(SourceLocation(), "Unknown paradox detection rule: " + ruleName);
    }
}

bool ParadoxDetector::isRuleEnabled(const std::string& ruleName) const {
    auto it = ruleEnabled.find(ruleName);
    return it != ruleEnabled.end() && it->second;
}

std::vector<std::string> ParadoxDetector::getRules() const {
    std::vector<std::string> rules;
    for (const auto& [ruleName, enabled] : ruleEnabled) {
        rules.push_back(ruleName);
    }
    return rules;
}

void ParadoxDetector::setSeverityThreshold(int threshold) {
    if (threshold >= 1 && threshold <= 10) {
        severityThreshold = threshold;
    } else {
        diagnostics.addWarning(SourceLocation(),
                               "Invalid severity threshold: " + std::to_string(threshold) +
                                   ". Must be between 1 and 10.");
    }
}

int ParadoxDetector::getSeverityThreshold() const { return severityThreshold; }

void ParadoxDetector::detectResourceViolations(const debugger::DejaVuDebugger& debugger,
                                               std::vector<Paradox>& paradoxes) const {
    // Check for excessive resource usage
    const auto& resourceUsage = debugger.getCurrentResourceUsage();

    // In a real implementation, we would have predefined resource limits
    // For now, we'll use some hardcoded thresholds

    // Check Chronon usage
    auto chrononsIt = resourceUsage.find("chronons");
    if (chrononsIt != resourceUsage.end()) {
        double chronons = chrononsIt->second;
        if (chronons > 1000.0) {
            // Very high chronon usage is a severe paradox
            paradoxes.push_back(
                {"Excessive Chronon usage detected: " + std::to_string(chronons) + " chronons",
                 8,  // High severity
                 debugger.getCurrentState().getSourceLocation()});
        } else if (chronons > 500.0) {
            // High chronon usage is a moderate paradox
            paradoxes.push_back(
                {"High Chronon usage detected: " + std::to_string(chronons) + " chronons",
                 5,  // Moderate severity
                 debugger.getCurrentState().getSourceLocation()});
        }
    }

    // Check Aethel usage
    auto aethelIt = resourceUsage.find("aethel");
    if (aethelIt != resourceUsage.end()) {
        double aethel = aethelIt->second;
        if (aethel < 0.0) {
            // Negative Aethel is a severe paradox
            paradoxes.push_back({"Negative Aethel detected: " + std::to_string(aethel) + " aethel",
                                 9,  // Very high severity
                                 debugger.getCurrentState().getSourceLocation()});
        } else if (aethel > 1000.0) {
            // Very high Aethel usage is a moderate paradox
            paradoxes.push_back(
                {"Excessive Aethel usage detected: " + std::to_string(aethel) + " aethel",
                 6,  // Moderate-high severity
                 debugger.getCurrentState().getSourceLocation()});
        }
    }

    // Check temporal debt
    auto debtIt = resourceUsage.find("temporal_debt");
    if (debtIt != resourceUsage.end()) {
        double debt = debtIt->second;
        if (debt > 100.0) {
            // High temporal debt is a severe paradox
            paradoxes.push_back({"Critical temporal debt detected: " + std::to_string(debt),
                                 10,  // Maximum severity
                                 debugger.getCurrentState().getSourceLocation()});
        } else if (debt > 50.0) {
            // Moderate temporal debt is a moderate paradox
            paradoxes.push_back({"Significant temporal debt detected: " + std::to_string(debt),
                                 7,  // High severity
                                 debugger.getCurrentState().getSourceLocation()});
        }
    }
}

void ParadoxDetector::detectTimelineInconsistencies(const debugger::DejaVuDebugger& debugger,
                                                    std::vector<Paradox>& paradoxes) const {
    // Access the state history to detect timeline inconsistencies
    const auto& history = debugger.getStateHistory();

    // In a real implementation, we would analyze the timeline branching history
    // For now, we'll check if the current timeline ID is greater than 10,
    // which might indicate excessive timeline branching

    int currentTimelineId = debugger.getCurrentTimelineId();
    if (currentTimelineId > 10) {
        paradoxes.push_back({"Excessive timeline branching detected: " +
                                 std::to_string(currentTimelineId) + " timelines",
                             8,  // High severity
                             debugger.getCurrentState().getSourceLocation()});
    }

    // Check for timeline convergence to detect if timelines were properly merged
    // This would require more complex analysis in a real implementation
    // For now, this is just a placeholder
}

void ParadoxDetector::detectCausalityViolations(const debugger::DejaVuDebugger& debugger,
                                                std::vector<Paradox>& paradoxes) const {
    // Causality violations are some of the most serious paradoxes
    // These would involve analyzing the execution history to detect if
    // an effect happens before its cause

    // For example, if a variable is used before it's initialized
    // or if a function is called before it's defined

    // For now, this is just a placeholder for future implementation

    // We can check if the current state is explicitly marked as a potential paradox
    if (debugger.getCurrentState().isPotentialParadox()) {
        paradoxes.push_back({"Causality violation detected by the debugger",
                             10,  // Maximum severity
                             debugger.getCurrentState().getSourceLocation()});
    }
}

void ParadoxDetector::detectVariableStateInconsistencies(const debugger::DejaVuDebugger& debugger,
                                                         std::vector<Paradox>& paradoxes) const {
    // Variable state inconsistencies involve checking if variables have
    // incompatible values across different timelines or execution paths

    // Access the state history to analyze variable values
    const auto& history = debugger.getStateHistory();

    // This would require tracking variable values across the execution history
    // and detecting incompatible states

    // For now, this is just a placeholder for future implementation

    // As a simple example, we can check if a variable changes too frequently,
    // which might indicate a variable state inconsistency

    // In a real implementation, we would have a more sophisticated analysis
}

void ParadoxDetector::detectThreadSynchronizationParadoxes(const debugger::DejaVuDebugger& debugger,
                                                           std::vector<Paradox>& paradoxes) const {
    // Thread synchronization paradoxes involve detecting if multiple threads
    // access the same resources in an inconsistent way

    // This would require analyzing the thread IDs and resource access patterns

    // For now, this is just a placeholder for future implementation

    // As a simple example, we can check if the current thread ID is non-zero,
    // indicating that parallel execution is in progress

    int threadId = debugger.getCurrentState().getThreadId();
    if (threadId > 0) {
        // This is just a placeholder to demonstrate the concept
        // In a real implementation, we would have a more sophisticated analysis
        // of thread interactions
    }
}

}  // namespace proving_grounds
}  // namespace compiler
}  // namespace chronovyan