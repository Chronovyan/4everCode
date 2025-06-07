/**
 * @file utility_functions_demo.cpp
 * @brief Demonstration of Chronovyan Standard Library utility functions
 *
 * This example demonstrates the usage of various utility functions from
 * the Chronovyan Standard Library, showing their temporal awareness and
 * general-purpose capabilities.
 *
 * @copyright Chronovyan Project
 */

#include <iomanip>
#include <iostream>

#include "../include/stdlib/chronovyan_stdlib.h"

using namespace chronovyan;
using namespace chronovyan::stdlib;

// Helper function to print a divider with a section title
void print_section(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(80, '=') << "\n";
}

// Helper function to print a vector of values
void print_values(const std::vector<Value>& values, const std::string& label = "Values") {
    std::cout << label << ": [";
    for (size_t i = 0; i < values.size(); ++i) {
        std::cout << utility::to_string(values[i], "default");
        if (i < values.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

int main() {
    // Initialize the standard library
    if (!stdlib::initialize()) {
        std::cerr << "Failed to initialize the Chronovyan Standard Library.\n";
        return 1;
    }

    std::cout << "Chronovyan Standard Library - Utility Functions Demo\n";
    std::cout << "===================================================\n\n";

    // Initialize the temporal runtime with some initial chronons
    TemporalRuntime::get_instance().add_chronons(100.0);

    //------------------------------------------------------
    // String Utilities
    //------------------------------------------------------
    print_section("String Utilities");

    // Create a value with a timestamp
    Value temporal_value(42.0);
    temporal_value.set_creation_time(1000.0);

    std::cout << "Default string: " << utility::to_string(temporal_value, "default") << "\n";
    std::cout << "Temporal string: " << utility::to_string(temporal_value, "temporal") << "\n";
    std::cout << "JSON string: " << utility::to_string(temporal_value, "json") << "\n";

    // String parsing
    std::cout << "\nParsing strings:\n";
    Value parsed_number = utility::parse_string("123.45", "number");
    Value parsed_boolean = utility::parse_string("true", "boolean");
    Value parsed_string = utility::parse_string("Hello, world!", "string");

    std::cout << "Parsed number: " << parsed_number.as_number() << "\n";
    std::cout << "Parsed boolean: " << (parsed_boolean.as_boolean() ? "true" : "false") << "\n";
    std::cout << "Parsed string: " << parsed_string.as_string() << "\n";

    // String splitting and joining
    std::cout << "\nSplitting and joining strings:\n";
    std::string csv_data = "apple,banana,cherry,date";
    std::vector<std::string> parts = utility::split_string(csv_data, ",");

    std::cout << "Split string into: [";
    for (size_t i = 0; i < parts.size(); ++i) {
        std::cout << parts[i];
        if (i < parts.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";

    std::string joined = utility::join_strings(parts, " | ");
    std::cout << "Joined with pipes: " << joined << "\n";

    // Temporal string transformation
    std::cout << "\nTemporal string transformations:\n";
    std::string message = "This message is revealed over time.";

    // Set the current time in the temporal runtime to simulate time passing
    TemporalRuntime::get_instance().set_current_time(2.5);

    std::string revealed =
        utility::temporal_string_transform(message, "reveal", {{"duration", Value(5.0)}});
    std::cout << "Revealed at t=2.5: \"" << revealed << "\"\n";

    TemporalRuntime::get_instance().set_current_time(5.0);
    revealed = utility::temporal_string_transform(message, "reveal", {{"duration", Value(5.0)}});
    std::cout << "Revealed at t=5.0: \"" << revealed << "\"\n";

    std::string encrypted = utility::temporal_string_transform(message, "encrypt");
    std::cout << "Encrypted: \"" << encrypted << "\"\n";

    std::string decrypted = utility::temporal_string_transform(encrypted, "decrypt");
    std::cout << "Decrypted: \"" << decrypted << "\"\n";

    //------------------------------------------------------
    // Conversion Utilities
    //------------------------------------------------------
    print_section("Conversion Utilities");

    // Convert values between types
    Value string_value(std::string("42"));
    Value bool_value(true);
    Value num_value(3.14159);
    Value hex_string(std::string("0xFF"));

    std::cout << "Converting values between types:\n";
    std::cout << "String '42' to number: "
              << utility::convert_value(string_value, "number").as_number() << "\n";

    std::cout << "Boolean true to number: "
              << utility::convert_value(bool_value, "number").as_number() << "\n";

    std::cout << "Number 3.14159 to string: "
              << utility::convert_value(num_value, "string").as_string() << "\n";

    std::cout << "Hex string '0xFF' to number: "
              << utility::convert_value(hex_string, "number", {{"base", Value(16)}}).as_number()
              << "\n";

    // Create a temporal sequence and convert it to different formats
    TemporalSequence sequence;
    sequence.add_entry(1.0, Value(10));
    sequence.add_entry(2.0, Value(20));
    sequence.add_entry(3.0, Value(30));

    std::cout << "\nConverting temporal sequence to different formats:\n";

    Value seq_as_vector = utility::convert_sequence(sequence, "vector");
    std::cout << "As vector: ";
    for (const auto& val : seq_as_vector.as_array()) {
        std::cout << val.as_number() << " ";
    }
    std::cout << "\n";

    Value seq_as_string = utility::convert_sequence(sequence, "string");
    std::cout << "As string: " << seq_as_string.as_string() << "\n";

    Value seq_as_json = utility::convert_sequence(sequence, "string", {{"format", Value("json")}});
    std::cout << "As JSON: " << seq_as_json.as_string() << "\n";

    //------------------------------------------------------
    // Collection Utilities
    //------------------------------------------------------
    print_section("Collection Utilities");

    // Create a vector of values
    std::vector<Value> values = {Value(5), Value(2), Value(8), Value(1), Value(3), Value(5)};

    print_values(values, "Original values");

    // Sort values
    std::vector<Value> sorted = utility::sort_values(values);
    print_values(sorted, "Sorted values");

    // Sort with custom comparison (descending order)
    std::vector<Value> sorted_desc = utility::sort_values(
        values, [](const Value& a, const Value& b) { return a.as_number() > b.as_number(); });
    print_values(sorted_desc, "Sorted descending");

    // Find a value
    int pos = utility::find_value(values, Value(8));
    std::cout << "Position of value 8: " << pos << "\n";

    // Remove duplicates
    std::vector<Value> unique = utility::unique_values(values);
    print_values(unique, "Unique values");

    //------------------------------------------------------
    // Math and Numeric Utilities
    //------------------------------------------------------
    print_section("Math and Numeric Utilities");

    // Create a vector of numeric values
    std::vector<Value> numbers = {Value(10), Value(20), Value(30), Value(40), Value(50)};

    print_values(numbers, "Numbers");

    // Calculate sum
    Value sum = utility::sum_values(numbers);
    std::cout << "Sum: " << sum.as_number() << "\n";

    // Calculate average
    Value avg = utility::average_values(numbers);
    std::cout << "Average: " << avg.as_number() << "\n";

    // Find min and max
    auto [min, max] = utility::minmax_values(numbers);
    std::cout << "Min: " << min.as_number() << ", Max: " << max.as_number() << "\n";

    // Linear interpolation
    Value start(0);
    Value end(100);
    std::cout << "\nLinear interpolation from 0 to 100:\n";

    for (double t = 0.0; t <= 1.0; t += 0.25) {
        Value interpolated = utility::lerp(start, end, t);
        std::cout << "At t=" << t << ": " << interpolated.as_number() << "\n";
    }

    // Clamp values
    Value to_clamp(150);
    Value clamped = utility::clamp(to_clamp, Value(0), Value(100));
    std::cout << "\nClamping " << to_clamp.as_number()
              << " between 0 and 100: " << clamped.as_number() << "\n";

    //------------------------------------------------------
    // General Utilities
    //------------------------------------------------------
    print_section("General Utilities");

    // Generate temporal IDs
    std::cout << "Generated temporal IDs:\n";
    std::cout << "Standard: " << utility::generate_temporal_id("user") << "\n";
    std::cout << "Hex format: " << utility::generate_temporal_id("user", {{"format", Value("hex")}})
              << "\n";
    std::cout << "Compact format: "
              << utility::generate_temporal_id("user", {{"format", Value("compact")}}) << "\n";
    std::cout << "With temporal info: "
              << utility::generate_temporal_id("user", {{"include_temporal", Value(true)}}) << "\n";

    // Measure temporal cost
    std::cout << "\nMeasuring temporal cost of a function:\n";

    // Add some chronons for the measurement
    TemporalRuntime::get_instance().add_chronons(1000.0);

    auto metrics = utility::measure_temporal_cost([]() {
        // Simulate some work that consumes chronons
        auto& runtime = TemporalRuntime::get_instance();
        runtime.consume_chronons(50.0);
        runtime.generate_aethel(25.0);

        // Simulate a slow operation
        for (int i = 0; i < 10000000; ++i) {
            // Just waste some cycles
            volatile int x = i * i;
        }

        return Value(42);
    });

    std::cout << "Function result: " << metrics["result"].as_number() << "\n";
    std::cout << "Execution time: " << metrics["execution_time_ms"].as_number() << " ms\n";
    std::cout << "Chronon usage: " << metrics["chronon_usage"].as_number() << "\n";
    std::cout << "Aethel generation: " << metrics["aethel_generation"].as_number() << "\n";
    std::cout << "Efficiency: " << metrics["efficiency"].as_number() << "\n";

    // Cached computation
    std::cout << "\nCached computation:\n";

    // Define an expensive function
    auto expensive_func = []() {
        std::cout << "Computing expensive result...\n";
        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return Value(12345);
    };

    std::cout << "First call: ";
    Value result1 = utility::cached_compute("test_key", expensive_func, 5);
    std::cout << "Result: " << result1.as_number() << "\n";

    std::cout << "Second call (should use cache): ";
    Value result2 = utility::cached_compute("test_key", expensive_func, 5);
    std::cout << "Result: " << result2.as_number() << "\n";

    // Wait for cache to expire
    std::cout << "Waiting for cache to expire (5 seconds)...\n";
    std::this_thread::sleep_for(std::chrono::seconds(6));

    std::cout << "Third call (after expiry): ";
    Value result3 = utility::cached_compute("test_key", expensive_func, 5);
    std::cout << "Result: " << result3.as_number() << "\n";

    // Shutdown the standard library
    stdlib::shutdown();

    return 0;
}