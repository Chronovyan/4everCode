#include <cassert>
#include <iostream>

#include "stdlib/paradox_management.h"

using namespace chronovyan::stdlib;

// A simple test callback for paradox alerts
void paradox_alert_callback(PARADOX_LEVEL level) {
    std::cout << "ALERT: Paradox level reached " << level << std::endl;
}

int main() {
    std::cout << "=== Paradox Management Test ===\n";

    // Initialize the paradox management subsystem
    bool init_result = initialize_paradox_management();
    std::cout << "Initialization: " << (init_result ? "SUCCESS" : "FAILURE") << std::endl;
    assert(init_result);

    // Get the initial paradox level (should be 0.0)
    PARADOX_LEVEL initial_level = get_paradox_level();
    std::cout << "Initial PARADOX_LEVEL: " << initial_level << std::endl;
    assert(initial_level == 0.0);

    // Register a paradox alert at threshold 0.5
    bool alert_result = set_paradox_alert(0.5, paradox_alert_callback);
    std::cout << "Alert registration: " << (alert_result ? "SUCCESS" : "FAILURE") << std::endl;
    assert(alert_result);

    // Perform a paradox check with non-conflicting dependencies
    bool check_result = paradox_check({"Time_Crystals", "Aethel_Shard"});
    std::cout << "Non-conflicting check: " << (check_result ? "PASS" : "FAIL") << std::endl;

    // Perform a paradox check with conflicting dependencies
    bool conflict_check = paradox_check({"Stability_Field", "Temporal_Rift"});
    std::cout << "Conflicting check: " << (conflict_check ? "PASS" : "FAIL (Expected)")
              << std::endl;

    // Get the new paradox level (should be > 0.0 after the conflict check)
    PARADOX_LEVEL new_level = get_paradox_level();
    std::cout << "New PARADOX_LEVEL: " << new_level << std::endl;
    assert(new_level > 0.0);

    // Create a sample Aethel reserve
    AETHEL_RESERVE reserve = 1000;

    // Stabilize the timeline using Aethel
    PARADOX_LEVEL after_stabilize = stabilize_timeline(reserve, 200);
    std::cout << "PARADOX_LEVEL after stabilization: " << after_stabilize << std::endl;
    std::cout << "Remaining Aethel: " << reserve << std::endl;
    assert(after_stabilize < new_level);
    assert(reserve == 800);

    // Trigger a paradox alert with a high level
    std::cout << "Triggering paradox alert at level 0.6..." << std::endl;
    trigger_paradox_level(0.6);

    // Create a ParadoxManager instance
    std::cout << "Creating ParadoxManager..." << std::endl;
    ParadoxManager manager;

    // Analyze the timeline
    std::cout << "Analyzing timeline..." << std::endl;
    auto issues = manager.analyze_timeline();
    std::cout << "Found " << issues.size() << " issues." << std::endl;

    // Create a containment field
    std::cout << "Creating containment field..." << std::endl;
    bool field_result = manager.create_containment_field("Test_Field", reserve, 300);
    std::cout << "Containment field creation: " << (field_result ? "SUCCESS" : "FAILURE")
              << std::endl;
    std::cout << "Remaining Aethel: " << reserve << std::endl;

    // Get final paradox level
    PARADOX_LEVEL final_level = get_paradox_level();
    std::cout << "Final PARADOX_LEVEL: " << final_level << std::endl;

    // Shutdown the paradox management subsystem
    bool shutdown_result = shutdown_paradox_management();
    std::cout << "Shutdown: " << (shutdown_result ? "SUCCESS" : "FAILURE") << std::endl;
    assert(shutdown_result);

    std::cout << "=== Test Completed Successfully ===\n";
    return 0;
}
