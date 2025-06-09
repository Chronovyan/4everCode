#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../include/temporal_debt_tracker.h"
#include "../include/temporal_runtime.h"
#include "../include/timestream_manager.h"

using namespace chronovyan;

// Define a TimestreamType enum if it doesn't exist in the actual codebase
enum class TimestreamType { MAIN, FEATURE, BUGFIX, EXPERIMENTAL };

// ANSI color codes for terminal output
namespace Color {
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string BOLD = "\033[1m";
} // namespace Color

// Helper function to print a separator line
void printSeparator(char c = '-', int width = 80) {
  std::cout << std::string(width, c) << std::endl;
}

// Helper function to print a header
void printHeader(const std::string &title) {
  printSeparator('=');
  std::cout << Color::BOLD << Color::CYAN << title << Color::RESET << std::endl;
  printSeparator('=');
  std::cout << std::endl;
}

// Helper function to print a section header
void printSection(const std::string &title) {
  std::cout << std::endl;
  std::cout << Color::BOLD << Color::YELLOW << "[ " << title << " ]"
            << Color::RESET << std::endl;
  printSeparator('-');
}

// Helper function to print timestream info
void printTimestreamInfo(const std::shared_ptr<Timestream> &timestream) {
  if (!timestream) {
    std::cout << "Null timestream!" << std::endl;
    return;
  }

  std::cout << Color::BOLD << "Timestream: " << Color::GREEN
            << timestream->getName() << Color::RESET
            << " (ID: " << timestream->getId() << ")" << std::endl;

  std::cout << "  Stability: " << std::fixed << std::setprecision(2)
            << timestream->calculateStability() << std::endl;

  auto parent = timestream->getParent();
  if (parent) {
    std::cout << "  Parent: " << parent->getName()
              << " (ID: " << parent->getId() << ")" << std::endl;
  } else {
    std::cout << "  Parent: None (Main Timestream)" << std::endl;
  }

  auto source_anchor = timestream->getSourceAnchor();
  if (source_anchor) {
    std::cout << "  Source Anchor: " << source_anchor->getId()
              << " (Stability: " << std::fixed << std::setprecision(2)
              << source_anchor->getStability() << ")" << std::endl;
  }

  std::cout << "  Anchors: " << std::endl;
  for (const auto &anchor : timestream->getAnchors()) {
    std::cout << "    - " << Color::CYAN << anchor->getId() << Color::RESET
              << " (Stability: " << std::fixed << std::setprecision(2)
              << anchor->getStability()
              << ", Description: " << anchor->getDescription() << ")"
              << std::endl;
  }

  std::cout << std::endl;
}

// Helper function to print debt info
void printDebtInfo(const TemporalDebtTracker &tracker) {
  std::cout << Color::BOLD << "Temporal Debt Status:" << Color::RESET
            << std::endl;

  // Get overall debt metrics
  double stability = tracker.getStability();
  double debt_limit = tracker.getDebtLimit();
  double total_chronon_debt = tracker.getTotalChrononsDebt();
  double total_aethel_debt = tracker.getTotalAethelDebt();

  // Print overall metrics
  std::cout << "  Stability: " << std::fixed << std::setprecision(2)
            << stability;
  if (stability > 0.8) {
    std::cout << " " << Color::GREEN << "(Excellent)" << Color::RESET;
  } else if (stability > 0.6) {
    std::cout << " " << Color::BLUE << "(Good)" << Color::RESET;
  } else if (stability > 0.4) {
    std::cout << " " << Color::YELLOW << "(Moderate)" << Color::RESET;
  } else {
    std::cout << " " << Color::RED << "(Critical)" << Color::RESET;
  }
  std::cout << std::endl;

  std::cout << "  Debt Limit: " << std::fixed << std::setprecision(2)
            << debt_limit << std::endl;
  std::cout << "  Total Chronon Debt: " << std::fixed << std::setprecision(2)
            << total_chronon_debt << std::endl;
  std::cout << "  Total Aethel Debt: " << std::fixed << std::setprecision(2)
            << total_aethel_debt << std::endl;

  // List individual debts
  std::cout << "  Individual Debts:" << std::endl;
  auto debts = tracker.getAllDebts();
  if (debts.empty()) {
    std::cout << "    No debt recorded." << std::endl;
  } else {
    for (const auto &debt : debts) {
      std::cout << "    * Operation " << debt.operation_id
                << " - Chronons: " << std::fixed << std::setprecision(2)
                << debt.chronon_debt << ", Aethel: " << debt.aethel_debt;

      if (debt.critical) {
        std::cout << " " << Color::RED << "(CRITICAL)" << Color::RESET;
      }

      std::cout << std::endl;
    }
  }

  std::cout << std::endl;
}

// Helper function to simulate a delay with a loading indicator
void simulateProcessing(const std::string &message, int milliseconds = 1000) {
  std::cout << message;
  std::cout.flush();

  for (int i = 0; i < 3; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds / 3));
    std::cout << ".";
    std::cout.flush();
  }

  std::cout << " " << Color::GREEN << "Done!" << Color::RESET << std::endl;
  std::cout << std::endl;
}

// Helper function to display a visualization of timestreams
void visualizeTimestreams(const TimestreamManager &manager,
                          TimeAnchor *selected_anchor = nullptr) {
  printSection("Timestream Visualization");

  const auto &timestreams = manager.getAllTimestreams();
  std::cout << "Timestream Graph:" << std::endl;
  std::cout << std::endl;

  // Identify the main timestream
  auto main_timestream = manager.getMainTimestream();

  // Create a mapping of parent IDs to child timestreams
  std::map<std::string, std::vector<std::shared_ptr<Timestream>>> children;

  for (const auto &ts : timestreams) {
    if (ts->getParent()) {
      children[ts->getParent()->getId()].push_back(ts);
    }
  }

  // Display the main timestream
  std::cout << Color::BOLD << Color::CYAN << "Main Timestream: " << Color::RESET
            << main_timestream->getName()
            << " (ID: " << main_timestream->getId() << ")" << std::endl;

  // Display anchors for the main timestream
  const auto &main_anchors = main_timestream->getAnchors();
  for (size_t i = 0; i < main_anchors.size(); i++) {
    const auto &anchor = main_anchors[i];

    std::cout << "│" << std::endl;
    std::cout << "├── " << Color::YELLOW << "Anchor: " << Color::RESET
              << anchor->getDescription() << " (Stability: " << std::fixed
              << std::setprecision(2) << anchor->getStability() << ")"
              << std::endl;

    // Check if this anchor has child timestreams
    auto branch_it = children.find(main_timestream->getId());
    if (branch_it != children.end() && i < main_anchors.size() - 1) {
      // Check which child timestreams branch from this anchor
      for (const auto &child : branch_it->second) {
        // Check if this child branches from the current anchor
        const auto &anchors = child->getAnchors();
        if (!anchors.empty()) {
          const auto &origin_anchor =
              anchors[0]; // Get the first anchor as origin
          if (origin_anchor && origin_anchor->getId() == anchor->getId()) {
            std::cout << "│   │" << std::endl;
            std::cout << "│   ├── " << Color::MAGENTA
                      << "Branch: " << Color::RESET << child->getName()
                      << " (ID: " << child->getId() << ")" << std::endl;

            // Display anchors for this branch
            const auto &child_anchors = child->getAnchors();
            for (const auto &child_anchor : child_anchors) {
              std::cout << "│   │   │" << std::endl;
              std::cout << "│   │   ├── " << Color::YELLOW
                        << "Anchor: " << Color::RESET
                        << child_anchor->getDescription()
                        << " (Stability: " << std::fixed << std::setprecision(2)
                        << child_anchor->getStability() << ")" << std::endl;
            }
            std::cout << "│   │" << std::endl;
          }
        }
      }
    }
  }

  std::cout << "│" << std::endl;
  std::cout << "└── " << Color::GREEN << "Current" << Color::RESET << std::endl;

  std::cout << std::endl;
}

// Demo simulation: Developer workflow with timelines
void simulateDeveloperWorkflow(TimestreamManager &manager,
                               TemporalDebtTracker &debt_tracker) {
  printHeader("Chronovyan Timestream Version Control System");
  std::cout << "This demonstration simulates a development workflow using the "
               "Chronovyan "
               "Timestream system."
            << std::endl;
  std::cout << "We'll create anchors, branch into Echoes, and harmonize "
               "changes while managing "
               "temporal debt."
            << std::endl;
  std::cout << std::endl;

  // Display initial state
  printSection("Initial State");
  std::cout << "Let's examine our initial timestream:" << std::endl;
  printTimestreamInfo(manager.getMainTimestream());

  // Create an anchor for the initial implementation
  printSection("Creating Initial Anchor");
  std::cout << "First, we'll create an anchor for our initial implementation:"
            << std::endl;

  auto anchor1 = manager.createAnchor(0.95, "Initial implementation");

  simulateProcessing("Creating anchor", 1500);

  std::cout << "Anchor created: " << anchor1->getDescription()
            << " (ID: " << anchor1->getId() << ")" << std::endl;
  std::cout << "Stability: " << std::fixed << std::setprecision(2)
            << anchor1->getStability() << std::endl;
  std::cout << std::endl;

  // Create a bug fix branch
  printSection("Creating Bug Fix Branch");
  std::cout << "Now, let's create an Echo (branch) to fix a critical bug:"
            << std::endl;

  std::string bugfix_name = "Bug Fix Branch";
  auto bugfix_branch = manager.createEcho(
      bugfix_name, manager.getMainTimestream()->getId(),
      static_cast<int>(
          TimestreamType::BUGFIX) // Cast to int if the API expects an int
  );

  simulateProcessing("Creating bug fix branch", 1500);

  manager.setActiveTimestream(bugfix_branch->getId());

  std::cout << "Echo created: " << bugfix_branch->getName()
            << " (ID: " << bugfix_branch->getId() << ")" << std::endl;
  printTimestreamInfo(bugfix_branch);

  // Show debt accrued from branching
  printSection("Debt From Branching");
  std::cout << "Creating an Echo accrues temporal debt:" << std::endl;
  printDebtInfo(debt_tracker);

  // Create another anchor in the main timestream
  printSection("Creating Feature Anchor in Main Timestream");
  std::cout << "Meanwhile, in the main timestream, we'll create another anchor "
               "for a new feature:"
            << std::endl;

  manager.setActiveTimestream(manager.getMainTimestream()->getId());
  auto anchor2 = manager.createAnchor(0.92, "Feature implementation planning");

  simulateProcessing("Creating feature anchor", 1500);

  std::cout << "Anchor created: " << anchor2->getDescription()
            << " (ID: " << anchor2->getId() << ")" << std::endl;
  std::cout << "Stability: " << std::fixed << std::setprecision(2)
            << anchor2->getStability() << std::endl;
  std::cout << std::endl;

  // Create feature branch
  printSection("Creating Feature Branch");
  std::cout << "Now, we'll create another Echo for developing the new feature:"
            << std::endl;

  std::string feature_name = "Feature Branch";
  auto feature_branch = manager.createEcho(
      feature_name, manager.getMainTimestream()->getId(),
      static_cast<int>(
          TimestreamType::FEATURE) // Cast to int if the API expects an int
  );

  simulateProcessing("Creating feature branch", 1500);

  manager.setActiveTimestream(feature_branch->getId());

  std::cout << "Echo created: " << feature_branch->getName()
            << " (ID: " << feature_branch->getId() << ")" << std::endl;
  printTimestreamInfo(feature_branch);

  // Show updated debt status
  printSection("Updated Debt Status");
  std::cout << "Creating multiple Echoes increases our temporal debt:"
            << std::endl;
  printDebtInfo(debt_tracker);

  // Visualize the timestreams
  visualizeTimestreams(manager);

  // Create an anchor in the bug fix branch
  printSection("Bug Fix Implementation");
  std::cout << "Let's implement our bug fix and create an anchor:" << std::endl;

  manager.setActiveTimestream(bugfix_branch->getId());
  auto bugfix_anchor = manager.createAnchor(0.88, "Bug fix implementation");

  simulateProcessing("Implementing bug fix", 2000);

  std::cout << "Anchor created: " << bugfix_anchor->getDescription()
            << " (ID: " << bugfix_anchor->getId() << ")" << std::endl;
  std::cout << "Stability: " << std::fixed << std::setprecision(2)
            << bugfix_anchor->getStability() << std::endl;
  std::cout << std::endl;

  // Harmonize the bug fix branch back to main
  printSection("Harmonizing Bug Fix");
  std::cout << "Let's harmonize (merge) the bug fix into the main timestream:"
            << std::endl;

  bool success = manager.harmonizeTimestreams(
      bugfix_branch->getId(), manager.getMainTimestream()->getId(),
      RepaymentStrategyType::CRITICAL_FIRST);

  simulateProcessing("Harmonizing bug fix branch", 2000);

  if (success) {
    std::cout << Color::GREEN << "Harmonization successful!" << Color::RESET
              << std::endl;
  } else {
    std::cout << Color::RED
              << "Harmonization failed! Timestreams too divergent."
              << Color::RESET << std::endl;
  }

  // Switch to main timestream
  manager.setActiveTimestream(manager.getMainTimestream()->getId());

  std::cout << "Main timestream after harmonization:" << std::endl;
  printTimestreamInfo(manager.getMainTimestream());

  // Show debt status after harmonization
  printSection("Debt After Harmonization");
  std::cout << "Harmonization also accrues temporal debt:" << std::endl;
  printDebtInfo(debt_tracker);

  // Calculate paradox risk for feature branch
  printSection("Paradox Risk Analysis");
  std::cout << "Now, let's assess the risk of harmonizing our experimental "
               "feature branch:"
            << std::endl;

  double risk = manager.calculateEchoParadoxRisk(
      manager.getMainTimestream()->getId(), anchor2->getId());

  simulateProcessing("Calculating paradox risk", 1500);

  std::cout << "Paradox risk: " << std::fixed << std::setprecision(2) << risk
            << std::endl;
  if (risk < 0.3) {
    std::cout << Color::GREEN
              << "Low risk - safe to proceed with harmonization."
              << Color::RESET << std::endl;
  } else if (risk < 0.7) {
    std::cout << Color::YELLOW << "Moderate risk - proceed with caution."
              << Color::RESET << std::endl;
  } else {
    std::cout << Color::RED
              << "High risk - harmonization may cause instability."
              << Color::RESET << std::endl;
  }

  // Apply debt repayment
  printSection("Debt Repayment");
  std::cout << "Before proceeding further, let's allocate resources to repay "
               "some temporal debt:"
            << std::endl;

  double chronon_resources = 20.0;
  double aethel_resources = 25.0;

  std::cout << "Available resources - Chronon: " << std::fixed
            << std::setprecision(2) << chronon_resources
            << ", Aethel: " << aethel_resources << std::endl;

  double stability_before = debt_tracker.getStability();
  std::cout << "Stability before repayment: " << std::fixed
            << std::setprecision(2) << stability_before << std::endl;

  simulateProcessing("Applying balanced repayment strategy", 2000);

  debt_tracker.applyRepaymentStrategy(RepaymentStrategyType::BALANCED,
                                      chronon_resources, aethel_resources);

  double stability_after = debt_tracker.getStability();
  std::cout << "Stability after repayment: " << std::fixed
            << std::setprecision(2) << stability_after << std::endl;
  std::cout << "Stability improvement: " << std::fixed << std::setprecision(2)
            << (stability_after - stability_before) << std::endl;

  // Final visualization
  visualizeTimestreams(manager);

  // Final debt status
  printSection("Final Debt Status");
  printDebtInfo(debt_tracker);

  // Summary
  printHeader("Simulation Summary");
  std::cout << "This demonstration showed key features of the Chronovyan "
               "Timestream Version "
               "Control System:"
            << std::endl;
  std::cout << "  1. Creating anchors to mark significant points in development"
            << std::endl;
  std::cout << "  2. Creating Echoes (branches) for parallel development"
            << std::endl;
  std::cout << "  3. Harmonizing (merging) changes between timestreams"
            << std::endl;
  std::cout << "  4. Managing and repaying temporal debt" << std::endl;
  std::cout << "  5. Assessing stability impact and paradox risk" << std::endl;
  std::cout << std::endl;

  std::cout << "Final system stability: " << std::fixed << std::setprecision(2)
            << debt_tracker.getStability() << std::endl;
  std::cout << "Total timestreams: " << manager.getAllTimestreams().size()
            << std::endl;
  std::cout << std::endl;

  std::cout << "Thank you for exploring the Chronovyan Timestream Version "
               "Control System!"
            << std::endl;
}

// Demo simulation
void simulateDemo() {
  // Create the temporal runtime, debt tracker, and timestream manager
  auto runtime = std::make_shared<TemporalRuntime>();
  TemporalDebtTracker debt_tracker(runtime);
  TimestreamManager manager(debt_tracker);

  // Create bug fix branch
  std::cout << "Creating bug fix branch..." << std::endl;
  std::string bugFixName = "bug_fix_" + std::to_string(std::time(nullptr));
  auto bugFixBranch =
      manager.createEcho(bugFixName, manager.getMainTimestream()->getId(),
                         static_cast<int>(TimestreamType::BUGFIX));

  // Display the new branch
  std::cout << "Bug fix branch created: " << bugFixBranch->getName()
            << std::endl;

  // Create feature branch
  std::cout << "Creating feature branch..." << std::endl;
  std::string featureName = "feature_" + std::to_string(std::time(nullptr));
  auto featureBranch =
      manager.createEcho(featureName, manager.getMainTimestream()->getId(),
                         static_cast<int>(TimestreamType::FEATURE));

  // Display the new branch
  std::cout << "Feature branch created: " << featureBranch->getName()
            << std::endl;

  // Visualize all timestreams
  visualizeTimestreams(manager);
}

int main() {
  // Create the temporal runtime, debt tracker, and timestream manager
  auto runtime = std::make_shared<TemporalRuntime>();
  TemporalDebtTracker debt_tracker(runtime);
  TimestreamManager manager(debt_tracker);

  // Run the simulation
  simulateDeveloperWorkflow(manager, debt_tracker);

  return 0;
}