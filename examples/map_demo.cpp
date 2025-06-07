#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../include/resource_visualization.h"
#include "../include/temporal_map.h"
#include "../include/temporal_runtime.h"

using namespace chronovyan;

// Helper function to print a separator line
void printSeparator() { std::cout << "\n" << std::string(70, '-') << "\n" << std::endl; }

// Helper function to print map contents
void printMap(const TemporalMap& map, const std::string& name) {
    std::cout << name << " (size: " << map.size() << "):" << std::endl;
    std::cout << map.toString() << std::endl;
}

// Helper function to print key history
void printKeyHistory(const std::map<int, Value>& history, const std::string& keyName) {
    std::cout << "History for key '" << keyName << "':" << std::endl;
    for (const auto& [cycle, value] : history) {
        std::cout << "  Cycle " << cycle << ": " << value.toString() << std::endl;
    }
}

int main() {
    // Initialize the temporal runtime
    auto runtime = std::make_shared<TemporalRuntime>();
    runtime->setChrononsLimit(10000);  // Set a high limit for the demo

    std::cout << "Chronovyan TEMPORAL_MAP Type Demonstration" << std::endl;
    printSeparator();

    // Basic Map Operations
    std::cout << "1. Basic Map Operations" << std::endl;
    TemporalMap basicMap(runtime);

    std::cout << "Creating a map with key-value pairs..." << std::endl;
    basicMap.set(Value("name"), Value("Chronovyan"));
    basicMap.set(Value("type"), Value("Language"));
    basicMap.set(Value("version"), Value(1.0));
    basicMap.set(Value("features"),
                 Value(std::vector<Value>{Value("Temporal Types"), Value("Resource Management"),
                                          Value("Pattern Detection")}));

    printMap(basicMap, "Basic Map");

    // Accessing and modifying
    std::cout << "\nAccessing and modifying values:" << std::endl;
    std::cout << "Value for key 'name': " << basicMap.at(Value("name")).toString() << std::endl;

    std::cout << "Updating version to 1.5..." << std::endl;
    basicMap.set(Value("version"), Value(1.5));
    std::cout << "New value for key 'version': " << basicMap.at(Value("version")).toString()
              << std::endl;

    std::cout << "Removing key 'type'..." << std::endl;
    basicMap.remove(Value("type"));
    std::cout << "Map contains key 'type': " << (basicMap.contains(Value("type")) ? "Yes" : "No")
              << std::endl;

    // Keys and values
    std::cout << "\nGetting all keys:" << std::endl;
    std::vector<Value> keys = basicMap.keys();
    for (const auto& key : keys) {
        std::cout << "  " << key.toString() << std::endl;
    }

    printSeparator();

    // Higher-order Functions
    std::cout << "2. Higher-order Functions" << std::endl;

    // Create a numeric map
    TemporalMap numericMap(runtime);
    numericMap.set(Value(1), Value(10));
    numericMap.set(Value(2), Value(20));
    numericMap.set(Value(3), Value(30));
    numericMap.set(Value(4), Value(40));
    numericMap.set(Value(5), Value(50));

    printMap(numericMap, "Numeric Map");

    // Using forEach
    std::cout << "\nUsing forEach to calculate sum of values:" << std::endl;
    int sum = 0;
    numericMap.forEach([&sum](const Value& key, const Value& value) { sum += value.asInteger(); });
    std::cout << "Sum: " << sum << std::endl;

    // Using mapValues
    std::cout << "\nUsing mapValues to square each value:" << std::endl;
    auto squaredMap = numericMap.mapValues([](const Value& key, const Value& value) {
        int64_t val = value.asInteger();
        return Value(static_cast<int64_t>(val * val));
    });
    printMap(*squaredMap, "Squared Map");

    // Using filter
    std::cout << "\nUsing filter to keep only odd keys:" << std::endl;
    auto oddKeysMap = numericMap.filter(
        [](const Value& key, const Value& value) { return key.asInteger() % 2 != 0; });
    printMap(*oddKeysMap, "Odd Keys Map");

    // Using merge
    std::cout << "\nUsing merge to combine two maps:" << std::endl;
    TemporalMap extraMap(runtime);
    extraMap.set(Value(5), Value(500));  // Conflicting key
    extraMap.set(Value(6), Value(60));   // New key

    // Custom conflict resolution function
    auto mergedMap =
        numericMap.merge(extraMap, [](const Value& key, const Value& val1, const Value& val2) {
            return Value(static_cast<int64_t>(val1.asInteger() + val2.asInteger()));
        });

    printMap(*mergedMap, "Merged Map (with custom conflict resolution)");
    printSeparator();

    // Temporal Features
    std::cout << "3. Temporal Features" << std::endl;

    // Create a map for temporal operations
    TemporalMap temporalMap(runtime);
    temporalMap.set(Value("counter"), Value(100));
    temporalMap.set(Value("message"), Value("Initial state"));

    std::cout << "Creating a map and saving its state at cycle 1..." << std::endl;
    printMap(temporalMap, "Initial Map");
    temporalMap.saveHistoryCheckpoint(1);

    // Modify map at cycle 2
    std::cout << "\nModifying the map at cycle 2..." << std::endl;
    temporalMap.set(Value("counter"), Value(200));
    temporalMap.set(Value("message"), Value("State at cycle 2"));
    temporalMap.set(Value("new_key"), Value("Added at cycle 2"));
    printMap(temporalMap, "Map at Cycle 2");
    temporalMap.saveHistoryCheckpoint(2);

    // Modify map at cycle 3
    std::cout << "\nModifying the map at cycle 3..." << std::endl;
    temporalMap.set(Value("counter"), Value(300));
    temporalMap.set(Value("message"), Value("State at cycle 3"));
    temporalMap.remove(Value("new_key"));
    printMap(temporalMap, "Map at Cycle 3");
    temporalMap.saveHistoryCheckpoint(3);

    // Accessing historical states
    std::cout << "\nAccessing values at different cycles:" << std::endl;
    std::cout << "Value of 'counter' at cycle 1: "
              << temporalMap.getAtCycle(Value("counter"), 1).toString() << std::endl;
    std::cout << "Value of 'counter' at cycle 2: "
              << temporalMap.getAtCycle(Value("counter"), 2).toString() << std::endl;
    std::cout << "Value of 'counter' at cycle 3: "
              << temporalMap.getAtCycle(Value("counter"), 3).toString() << std::endl;

    // Check key existence at different cycles
    std::cout << "\nChecking key existence at different cycles:" << std::endl;
    std::cout << "Key 'new_key' existed at cycle 1: "
              << (temporalMap.containedAtCycle(Value("new_key"), 1) ? "Yes" : "No") << std::endl;
    std::cout << "Key 'new_key' existed at cycle 2: "
              << (temporalMap.containedAtCycle(Value("new_key"), 2) ? "Yes" : "No") << std::endl;
    std::cout << "Key 'new_key' existed at cycle 3: "
              << (temporalMap.containedAtCycle(Value("new_key"), 3) ? "Yes" : "No") << std::endl;

    // Get all keys at different cycles
    std::cout << "\nKeys at different cycles:" << std::endl;
    std::cout << "Cycle 1: ";
    for (const auto& key : temporalMap.keysAtCycle(1)) {
        std::cout << key.toString() << " ";
    }
    std::cout << std::endl;

    std::cout << "Cycle 2: ";
    for (const auto& key : temporalMap.keysAtCycle(2)) {
        std::cout << key.toString() << " ";
    }
    std::cout << std::endl;

    std::cout << "Cycle 3: ";
    for (const auto& key : temporalMap.keysAtCycle(3)) {
        std::cout << key.toString() << " ";
    }
    std::cout << std::endl;

    // Restoring from checkpoints
    std::cout << "\nRestoring from cycle 1..." << std::endl;
    temporalMap.restoreFromCycle(1);
    printMap(temporalMap, "Map After Restoring to Cycle 1");

    std::cout << "\nRestoring from cycle 3..." << std::endl;
    temporalMap.restoreFromCycle(3);
    printMap(temporalMap, "Map After Restoring to Cycle 3");

    printSeparator();

    // Advanced Temporal Features
    std::cout << "4. Advanced Temporal Features" << std::endl;

    // Create a map for advanced temporal features
    TemporalMap advancedMap(runtime);

    // Set up pattern for linear growth
    std::cout << "Setting up a key with linear growth pattern..." << std::endl;
    advancedMap.set(Value("linear"), Value(10));
    advancedMap.saveHistoryCheckpoint(1);

    advancedMap.set(Value("linear"), Value(20));
    advancedMap.saveHistoryCheckpoint(2);

    advancedMap.set(Value("linear"), Value(30));
    advancedMap.saveHistoryCheckpoint(3);

    advancedMap.set(Value("linear"), Value(40));
    advancedMap.saveHistoryCheckpoint(4);

    // Set up pattern for cyclic changes
    std::cout << "Setting up a key with cyclic pattern..." << std::endl;
    advancedMap.set(Value("cyclic"), Value("A"));
    advancedMap.saveHistoryCheckpoint(5);

    advancedMap.set(Value("cyclic"), Value("B"));
    advancedMap.saveHistoryCheckpoint(6);

    advancedMap.set(Value("cyclic"), Value("A"));
    advancedMap.saveHistoryCheckpoint(7);

    advancedMap.set(Value("cyclic"), Value("B"));
    advancedMap.saveHistoryCheckpoint(8);

    // Track key history
    std::cout << "\nTracking history of keys:" << std::endl;
    auto linearHistory = advancedMap.trackKeyHistory(Value("linear"), 1, 4);
    printKeyHistory(linearHistory, "linear");

    auto cyclicHistory = advancedMap.trackKeyHistory(Value("cyclic"), 5, 8);
    printKeyHistory(cyclicHistory, "cyclic");

    // Detect patterns
    std::cout << "\nDetecting patterns in key changes:" << std::endl;
    std::string linearPattern = advancedMap.detectKeyPattern(Value("linear"), {1, 2, 3, 4});
    std::cout << "Pattern for 'linear' key: " << linearPattern << std::endl;

    std::string cyclicPattern = advancedMap.detectKeyPattern(Value("cyclic"), {5, 6, 7, 8});
    std::cout << "Pattern for 'cyclic' key: " << cyclicPattern << std::endl;

    // Predict future values
    std::cout << "\nPredicting future values:" << std::endl;
    Value predictedLinear = advancedMap.predictFutureValue(Value("linear"), 5, {1, 2, 3, 4});
    std::cout << "Predicted value for 'linear' at cycle 5: " << predictedLinear.toString()
              << std::endl;

    Value predictedCyclic = advancedMap.predictFutureValue(Value("cyclic"), 9, {5, 6, 7, 8});
    std::cout << "Predicted value for 'cyclic' at cycle 9: " << predictedCyclic.toString()
              << std::endl;

    // Get frequently changed keys
    std::cout << "\nFrequently changed keys:" << std::endl;
    auto changedKeys = advancedMap.getFrequentlyChangedKeys(1, 8);
    for (const auto& [key, count] : changedKeys) {
        std::cout << "Key '" << key.toString() << "' changed " << count << " times" << std::endl;
    }

    printSeparator();

    // Resource Usage Tracking
    std::cout << "5. Resource Usage Tracking" << std::endl;

    // Get chronons consumed
    int chrononsUsed = runtime->getChrononsUsed();
    std::cout << "Total Chronons used during this demonstration: " << chrononsUsed << std::endl;

    // Visualize resource usage if available
    try {
        ResourceVisualization resourceViz;
        std::cout << "Resource usage visualization:" << std::endl;
        resourceViz.generateResourceUsageReport(*runtime);
    } catch (const std::exception& e) {
        std::cout << "Resource visualization not available: " << e.what() << std::endl;
    }

    printSeparator();
    std::cout << "TEMPORAL_MAP Type Demonstration Completed" << std::endl;

    return 0;
}