#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "temporal_debt_tracker.h"
#include "timestream_manager.h"

using namespace chronovyan;

// Helper function to print timestream info
void printTimestreamInfo(const std::shared_ptr<Timestream>& timestream) {
    if (!timestream) {
        std::cout << "Null timestream!" << std::endl;
        return;
    }

    std::cout << "Timestream: " << timestream->getName() << " (ID: " << timestream->getId() << ")"
              << std::endl;

    std::cout << "  Stability: " << timestream->calculateStability() << std::endl;

    auto parent = timestream->getParent();
    if (parent) {
        std::cout << "  Parent: " << parent->getName() << " (ID: " << parent->getId() << ")"
                  << std::endl;
    } else {
        std::cout << "  Parent: None (Main Timestream)" << std::endl;
    }

    auto source_anchor = timestream->getSourceAnchor();
    if (source_anchor) {
        std::cout << "  Source Anchor: " << source_anchor->getId()
                  << " (Stability: " << source_anchor->getStability() << ")" << std::endl;
    }

    std::cout << "  Anchors: " << std::endl;
    for (const auto& anchor : timestream->getAnchors()) {
        std::cout << "    - " << anchor->getId() << " (Stability: " << anchor->getStability()
                  << ", Description: " << anchor->getDescription() << ")" << std::endl;
    }

    std::cout << std::endl;
}

// Helper function to print debt info
void printDebtInfo(const TemporalDebtTracker& tracker) {
    std::cout << "Current Stability: " << tracker.getStability() << std::endl;

    std::cout << "Debts:" << std::endl;
    for (const auto& debt : tracker.getAllDebts()) {
        // Determine debt type based on which value is higher
        std::string type_str = (debt.chronon_debt > 0) ? "CHRONON" : "AETHEL";
        // Calculate if debt is past due based on current cycle
        bool isPastDue = debt.due_cycle < tracker.getCurrentCycle();
        std::string status = isPastDue ? "PAST DUE" : "Current";
        std::string critical = debt.critical ? "CRITICAL" : "Normal";

        std::cout << "  - " << debt.operation_id << std::endl;
        std::cout << "    Amount: "
                  << (debt.chronon_debt > 0 ? debt.chronon_debt : debt.aethel_debt)
                  << " | Type: " << type_str << " | Status: " << status << " | " << critical
                  << std::endl;
        std::cout << "    Due in cycle " << debt.due_cycle
                  << " | Interest Rate: " << (debt.interest_rate * 100) << "%" << std::endl;
    }

    std::cout << std::endl;
}

int main() {
    std::cout << "=== Timestream Version Control System Test ===" << std::endl;
    std::cout << std::endl;

    // Create a temporal runtime and debt tracker
    auto runtime = std::make_shared<TemporalRuntime>();
    TemporalDebtTracker debt_tracker(runtime);

    // Create a timestream manager
    TimestreamManager manager(debt_tracker);

    // Get the main timestream
    auto main_timestream = manager.getMainTimestream();
    printTimestreamInfo(main_timestream);

    std::cout << "--- Initial Debt State ---" << std::endl;
    printDebtInfo(debt_tracker);

    // Create some anchors in the main timestream
    std::cout << "Creating anchors in main timestream..." << std::endl;
    auto anchor1 = manager.createAnchor(0.95, "Feature A implementation");
    auto anchor2 = manager.createAnchor(0.85, "Feature B implementation");
    auto anchor3 = manager.createAnchor(0.90, "Bug fixes for Feature A");

    // Print updated main timestream info
    printTimestreamInfo(main_timestream);

    // Create an Echo (branch) from anchor2
    std::cout << "Creating Echo 'Feature B Experimental' from anchor2..." << std::endl;
    auto echo1 =
        manager.createEcho("Feature B Experimental", main_timestream->getId(), anchor2->getId());

    // Print Echo info
    printTimestreamInfo(echo1);

    // Print debt state after creating Echo
    std::cout << "--- Debt State After Creating Echo ---" << std::endl;
    printDebtInfo(debt_tracker);

    // Create some anchors in the Echo
    std::cout << "Setting Echo as active timestream and creating anchors..." << std::endl;
    manager.setActiveTimestream(echo1->getId());
    manager.createAnchor(0.80, "Experimental changes to Feature B");
    manager.createAnchor(0.70, "More experimental changes");

    // Print updated Echo info
    printTimestreamInfo(echo1);

    // Switch back to main timestream
    std::cout << "Switching back to main timestream..." << std::endl;
    manager.setActiveTimestream(main_timestream->getId());

    // Create a second Echo from anchor3
    std::cout << "Creating Echo 'Bug Fix Branch' from anchor3..." << std::endl;
    auto echo2 = manager.createEcho("Bug Fix Branch", main_timestream->getId(), anchor3->getId());

    // Print Echo info
    printTimestreamInfo(echo2);

    // Create anchors in the second Echo
    std::cout << "Setting Bug Fix Branch as active timestream and creating anchors..." << std::endl;
    manager.setActiveTimestream(echo2->getId());
    manager.createAnchor(0.92, "Additional bug fixes");
    manager.createAnchor(0.95, "Performance improvements");

    // Print updated second Echo info
    printTimestreamInfo(echo2);

    // Calculate harmonization impact
    std::cout << "Calculating harmonization impact..." << std::endl;
    double impact = manager.calculateHarmonizationImpact(echo2->getId(), main_timestream->getId());
    std::cout << "Harmonization impact of Bug Fix Branch into Main: " << impact << std::endl;
    std::cout << "(Positive means stability improvement, negative means destabilization)"
              << std::endl;
    std::cout << std::endl;

    // Harmonize the second Echo into main
    std::cout << "Harmonizing Bug Fix Branch into Main timestream..." << std::endl;
    bool success = manager.harmonizeTimestreams(echo2->getId(), main_timestream->getId(),
                                                RepaymentStrategy::STABILITY_OPTIMIZED);

    if (success) {
        std::cout << "Harmonization successful!" << std::endl;
    } else {
        std::cout << "Harmonization failed! Timestreams too divergent." << std::endl;
    }

    // Print updated main timestream info
    printTimestreamInfo(main_timestream);

    // Print debt state after harmonization
    std::cout << "--- Debt State After Harmonization ---" << std::endl;
    printDebtInfo(debt_tracker);

    // Calculate Echo paradox risk
    std::cout << "Calculating paradox risk of creating a new Echo..." << std::endl;
    double risk = manager.calculateEchoParadoxRisk(main_timestream->getId(), anchor1->getId());
    std::cout << "Paradox risk of creating Echo from anchor1: " << risk << std::endl;
    std::cout << "(0.0 means no risk, 1.0 means maximum risk)" << std::endl;
    std::cout << std::endl;

    // Demonstrate debt repayment
    std::cout << "Applying debt repayment strategy..." << std::endl;
    double chronon_resources = 15.0;
    double aethel_resources = 20.0;

    std::cout << "Available resources - Chronon: " << chronon_resources
              << ", Aethel: " << aethel_resources << std::endl;

    double stability_before = debt_tracker.getStability();
    std::cout << "Stability before repayment: " << stability_before << std::endl;

    debt_tracker.applyRepaymentStrategy(RepaymentStrategy::BALANCED, chronon_resources,
                                        aethel_resources);

    double stability_after = debt_tracker.getStability();
    std::cout << "Stability after repayment: " << stability_after << std::endl;
    std::cout << "Stability improvement: " << (stability_after - stability_before) << std::endl;

    // Print final debt state
    std::cout << "--- Final Debt State ---" << std::endl;
    printDebtInfo(debt_tracker);

    // Print all timestreams
    std::cout << "--- All Timestreams ---" << std::endl;
    for (const auto& ts : manager.getAllTimestreams()) {
        std::cout << ts->getName() << " (ID: " << ts->getId() << ")" << std::endl;
    }

    return 0;
}