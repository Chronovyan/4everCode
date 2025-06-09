/**
 * @file exploration.cpp
 * @brief Implementation of exploration and loot functions for the Chronovyan
 * Standard Library
 *
 * This file contains the implementation of functions that allow exploration of
 * timelines and collection of rewards in Chronovyan programs. These functions
 * enable users to probe timelines for insights and claim loot from temporal
 * rifts.
 *
 * @copyright Chronovyan Project
 */

#include <algorithm>
#include <ctime>
#include <iostream>
#include <mutex>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "stdlib/exploration.h"
#include "stdlib/paradox_management.h"
#include "stdlib/resource_management.h"

namespace chronovyan {
namespace stdlib {

// Forward declarations for external functions
extern double get_paradox_level(); // From paradox_management.cpp
extern bool
consume_aethel(AETHEL_RESERVE amount); // From resource_management.cpp
extern void
increase_paradox_level(double amount); // From paradox_management.cpp

// Global state for exploration management
namespace {
std::mutex exploration_mutex;
bool initialized = false;

// Storage for discovered regions and their properties
std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
    discovered_regions;

// Storage for exploration teams
struct ExplorationTeam {
  std::string name;
  int size;
  bool active;
  std::string current_mission;
  int remaining_ticks;
};
std::unordered_map<std::string, ExplorationTeam> exploration_teams;

// Random generators for exploration outcomes
std::mt19937 rng{std::random_device{}()};
std::uniform_real_distribution<double> discovery_chance(0.0, 1.0);
std::uniform_real_distribution<double> signature_variation(0.0, 1.0);

// Predefined temporal events that can occur during exploration
const std::vector<std::string> temporal_events = {
    "Temporal_Echo",   "Chronon_Storm",        "Reality_Shift",
    "Time_Dilation",   "Paradox_Wave",         "Memory_Ripple",
    "Causality_Break", "Timeline_Convergence", "Temporal_Anomaly",
    "Aethel_Surge"};
} // namespace

// Initialize the exploration subsystem
bool initialize_exploration() {
  std::lock_guard<std::mutex> lock(exploration_mutex);
  if (initialized) {
    return true;
  }

  try {
    // Register default discovered regions
    discovered_regions["Alpha_Timeline"] = {{"stability", "0.9"},
                                            {"resources", "Time_Crystals"},
                                            {"danger", "0.1"},
                                            {"mapped", "true"}};

    discovered_regions["Temporal_Rift"] = {{"stability", "0.4"},
                                           {"resources", "Aethel_Shard"},
                                           {"danger", "0.7"},
                                           {"mapped", "false"}};

    initialized = true;
    std::cout << "Exploration subsystem initialized." << std::endl;
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to initialize exploration subsystem: " << e.what()
              << std::endl;
    return false;
  }
}

// Shutdown the exploration subsystem
bool shutdown_exploration() {
  std::lock_guard<std::mutex> lock(exploration_mutex);
  if (!initialized) {
    return true;
  }

  try {
    // Clean up resources
    discovered_regions.clear();
    exploration_teams.clear();

    initialized = false;
    std::cout << "Exploration subsystem shut down." << std::endl;
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to shut down exploration subsystem: " << e.what()
              << std::endl;
    return false;
  }
}

// Implementation of deepen_exploration function
TEMPORAL_SIGNATURE deepen_exploration(const std::string &target) {
  std::lock_guard<std::mutex> lock(exploration_mutex);
  if (!initialized) {
    std::cerr << "Exploration subsystem not initialized." << std::endl;
    return "";
  }

  try {
    // Check if the target exists
    if (discovered_regions.find(target) == discovered_regions.end()) {
      std::cerr << "Unknown exploration target: " << target << std::endl;
      return "";
    }

    // Apply Aethel cost for exploration
    std::cout << "Consuming 40 Aethel for exploration..." << std::endl;
    if (!consume_aethel(40)) {
      std::cerr << "Insufficient Aethel for exploration." << std::endl;
      return "";
    }

    // Get the target stability
    double stability = std::stod(discovered_regions[target]["stability"]);

    // If the target is unstable, increase paradox level
    if (stability < 0.6) {
      std::cout << "WARNING: Exploring unstable region (" << target
                << ") increases PARADOX_LEVEL by 0.1" << std::endl;
      increase_paradox_level(0.1);
    }

    // Generate a temporal signature based on the target and some randomness
    std::string signature = target + "_";

    // Add a timestamp to make it unique
    signature += std::to_string(std::time(nullptr));

    // Add a random component based on stability
    double random_factor = signature_variation(rng);
    signature += "_" + std::to_string(int(random_factor * 10000));

    std::cout << "Generated temporal signature: " << signature << " from "
              << target << std::endl;

    // Random chance to discover something new based on stability
    if (discovery_chance(rng) > stability) {
      // Create a new region derived from the target
      std::string new_region =
          target + "_Branch_" + std::to_string(int(random_factor * 100));

      // Set properties for the new region
      discovered_regions[new_region] = {
          {"stability",
           std::to_string(stability * 0.8)}, // Less stable than parent
          {"resources",
           discovered_regions[target]["resources"]}, // Same resources initially
          {"danger", std::to_string(std::min(
                         1.0, std::stod(discovered_regions[target]["danger"]) *
                                  1.2))}, // More dangerous
          {"mapped", "false"} // Newly discovered regions are unmapped
      };

      std::cout << "Discovered new region: " << new_region << std::endl;
    }

    return signature;
  } catch (const std::exception &e) {
    std::cerr << "Exploration failed: " << e.what() << std::endl;
    return "";
  }
}

// Implementation of loot_drop function
CHRONON_STREAM loot_drop(const LOOT_TABLE &table) {
  std::lock_guard<std::mutex> lock(exploration_mutex);
  if (!initialized) {
    std::cerr << "Exploration subsystem not initialized." << std::endl;
    return "";
  }

  try {
    // Apply Aethel cost for loot resolution
    std::cout << "Consuming 10 Aethel for loot resolution..." << std::endl;
    if (!consume_aethel(10)) {
      std::cerr << "Insufficient Aethel for loot resolution." << std::endl;
      return "";
    }

    // Roll a random number to determine the loot
    double roll = discovery_chance(rng);

    // Find the entry in the loot table that matches the roll
    for (const auto &entry : table) {
      if (roll <= entry.first) {
        std::cout << "Loot roll: " << roll << " yields: " << entry.second
                  << std::endl;
        return entry.second;
      }
    }

    // If no match was found (should not happen with a properly constructed
    // table)
    std::cerr << "No matching loot found for roll: " << roll << std::endl;
    return "";
  } catch (const std::exception &e) {
    std::cerr << "Loot resolution failed: " << e.what() << std::endl;
    return "";
  }
}

// Implementation of LOOT function
CHRONON_STREAM LOOT(const std::string &name) {
  // This is a simple helper function that just returns the name
  return name;
}

// Explorer class implementation
Explorer::Explorer() : internal_state(nullptr) {
  // Create internal state
  internal_state = new std::unordered_map<std::string, std::string>();
}

Explorer::~Explorer() {
  // Clean up internal state
  delete static_cast<std::unordered_map<std::string, std::string> *>(
      internal_state);
}

std::unordered_map<std::string, std::string>
Explorer::analyze_signature(const TEMPORAL_SIGNATURE &signature) {
  if (!initialized) {
    std::cerr << "Exploration subsystem not initialized." << std::endl;
    return {};
  }

  try {
    std::unordered_map<std::string, std::string> properties;

    // Parse the signature to extract information
    size_t first_underscore = signature.find('_');
    if (first_underscore == std::string::npos) {
      std::cerr << "Invalid temporal signature format: " << signature
                << std::endl;
      return properties;
    }

    // Extract the base region from the signature
    std::string base_region = signature.substr(0, first_underscore);

    // Check if the base region exists
    if (discovered_regions.find(base_region) == discovered_regions.end()) {
      std::cerr << "Unknown base region in signature: " << base_region
                << std::endl;
      return properties;
    }

    // Copy properties from the base region
    properties = discovered_regions[base_region];

    // Add signature-specific properties
    properties["signature_timestamp"] = signature.substr(
        first_underscore + 1,
        signature.find('_', first_underscore + 1) - first_underscore - 1);
    properties["signature_variant"] =
        signature.substr(signature.find_last_of('_') + 1);
    properties["base_region"] = base_region;

    std::cout << "Analyzed signature from region: " << base_region << std::endl;
    return properties;
  } catch (const std::exception &e) {
    std::cerr << "Signature analysis failed: " << e.what() << std::endl;
    return {};
  }
}

bool Explorer::create_team(const std::string &name, int size) {
  std::lock_guard<std::mutex> lock(exploration_mutex);
  if (!initialized) {
    std::cerr << "Exploration subsystem not initialized." << std::endl;
    return false;
  }

  try {
    // Check if a team with this name already exists
    if (exploration_teams.find(name) != exploration_teams.end()) {
      std::cerr << "Exploration team already exists: " << name << std::endl;
      return false;
    }

    // Validate team size
    if (size < 1 || size > 10) {
      std::cerr << "Invalid team size. Must be between 1 and 10." << std::endl;
      return false;
    }

    // Apply Aethel cost based on team size
    int cost = size * 30; // 30 Aethel per team member
    std::cout << "Consuming " << cost << " Aethel to create team..."
              << std::endl;
    if (!consume_aethel(cost)) {
      std::cerr << "Insufficient Aethel to create team." << std::endl;
      return false;
    }

    // Create the team
    ExplorationTeam team;
    team.name = name;
    team.size = size;
    team.active = false;
    team.current_mission = "";
    team.remaining_ticks = 0;

    exploration_teams[name] = team;

    std::cout << "Created exploration team: " << name << " with " << size
              << " members." << std::endl;
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Team creation failed: " << e.what() << std::endl;
    return false;
  }
}

TEMPORAL_SIGNATURE Explorer::send_team(const std::string &team_name,
                                       const std::string &target,
                                       int duration) {
  std::lock_guard<std::mutex> lock(exploration_mutex);
  if (!initialized) {
    std::cerr << "Exploration subsystem not initialized." << std::endl;
    return "";
  }

  try {
    // Check if the team exists
    if (exploration_teams.find(team_name) == exploration_teams.end()) {
      std::cerr << "Unknown exploration team: " << team_name << std::endl;
      return "";
    }

    // Check if the target exists
    if (discovered_regions.find(target) == discovered_regions.end()) {
      std::cerr << "Unknown exploration target: " << target << std::endl;
      return "";
    }

    // Check if the team is already on a mission
    ExplorationTeam &team = exploration_teams[team_name];
    if (team.active) {
      std::cerr << "Team is already on a mission: " << team.current_mission
                << std::endl;
      return "";
    }

    // Validate duration
    if (duration < 1 || duration > 100) {
      std::cerr << "Invalid mission duration. Must be between 1 and 100 ticks."
                << std::endl;
      return "";
    }

    // Apply Aethel cost based on team size and duration
    int cost = team.size * duration * 5; // 5 Aethel per team member per tick
    std::cout << "Consuming " << cost << " Aethel to send team on mission..."
              << std::endl;
    if (!consume_aethel(cost)) {
      std::cerr << "Insufficient Aethel to send team on mission." << std::endl;
      return "";
    }

    // Update team state
    team.active = true;
    team.current_mission = target;
    team.remaining_ticks = duration;

    // Generate a temporal signature for the mission
    std::string signature = target + "_Mission_";

    // Add a timestamp to make it unique
    signature += std::to_string(std::time(nullptr));

    // Add team identifier
    signature += "_" + team_name;

    std::cout << "Team " << team_name << " dispatched to " << target << " for "
              << duration << " ticks. Signature: " << signature << std::endl;

    // Mark the region as mapped if the mission is successful
    if (discovered_regions[target]["mapped"] == "false") {
      // Higher chance of successful mapping with larger teams
      double mapping_chance = 0.3 + (team.size * 0.05);
      if (discovery_chance(rng) < mapping_chance) {
        discovered_regions[target]["mapped"] = "true";
        std::cout << "Region " << target << " has been successfully mapped!"
                  << std::endl;
      }
    }

    return signature;
  } catch (const std::exception &e) {
    std::cerr << "Mission dispatch failed: " << e.what() << std::endl;
    return "";
  }
}

// Add a simple internal test function that can be called to verify the module
// works
void _exploration_internal_test() {
  std::cout << "Running internal exploration module test..." << std::endl;

  // Initialize the exploration subsystem
  if (!initialize_exploration()) {
    std::cerr << "Failed to initialize exploration subsystem for test."
              << std::endl;
    return;
  }

  // Test deepen_exploration with a known region
  TEMPORAL_SIGNATURE signature = deepen_exploration("Alpha_Timeline");
  if (signature.empty()) {
    std::cerr << "Failed to generate temporal signature." << std::endl;
  } else {
    std::cout << "Successfully generated temporal signature: " << signature
              << std::endl;
  }

  // Create an Explorer instance
  Explorer explorer;

  // Test creating a team
  bool team_created = explorer.create_team("Test_Team", 3);
  if (!team_created) {
    std::cerr << "Failed to create exploration team." << std::endl;
  } else {
    std::cout << "Successfully created exploration team." << std::endl;
  }

  // Shutdown the exploration subsystem
  if (!shutdown_exploration()) {
    std::cerr << "Failed to shut down exploration subsystem after test."
              << std::endl;
  }

  std::cout << "Exploration module internal test complete." << std::endl;
}

} // namespace stdlib
} // namespace chronovyan