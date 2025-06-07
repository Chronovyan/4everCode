#include <algorithm>
#include <cmath>
#include <sstream>

#include "../include/error_handler.h"
#include "../include/vector_type.h"

namespace chronovyan {

// Constructor for empty vector
ChronovyanVector::ChronovyanVector(std::shared_ptr<TemporalRuntime> runtime) : m_runtime(runtime) {
    // Track resource usage for initialization
    trackResourceUsage("init");
}

// Constructor with initial elements
ChronovyanVector::ChronovyanVector(std::vector<Value> elements,
                                   std::shared_ptr<TemporalRuntime> runtime)
    : m_elements(std::move(elements)), m_runtime(runtime) {
    // Track resource usage for initialization with elements
    trackResourceUsage("init_with_elements");
}

// Size of the vector
size_t ChronovyanVector::size() const { return m_elements.size(); }

// Check if vector is empty
bool ChronovyanVector::empty() const { return m_elements.empty(); }

// Get element at index (const)
const Value& ChronovyanVector::at(size_t index) const {
    if (index >= m_elements.size()) {
        throw std::out_of_range("Vector index out of range");
    }
    return m_elements[index];
}

// Get element at index (mutable)
Value& ChronovyanVector::at(size_t index) {
    if (index >= m_elements.size()) {
        throw std::out_of_range("Vector index out of range");
    }
    return m_elements[index];
}

// Add element to the end
void ChronovyanVector::push_back(const Value& value) {
    trackResourceUsage("push_back");
    m_elements.push_back(value);
}

// Remove last element
void ChronovyanVector::pop_back() {
    if (m_elements.empty()) {
        throw std::out_of_range("Cannot pop from an empty vector");
    }
    trackResourceUsage("pop_back");
    m_elements.pop_back();
}

// Insert element at position
void ChronovyanVector::insert(size_t index, const Value& value) {
    if (index > m_elements.size()) {
        throw std::out_of_range("Vector insert index out of range");
    }
    trackResourceUsage("insert");
    m_elements.insert(m_elements.begin() + index, value);
}

// Remove element at position
void ChronovyanVector::erase(size_t index) {
    if (index >= m_elements.size()) {
        throw std::out_of_range("Vector erase index out of range");
    }
    trackResourceUsage("erase");
    m_elements.erase(m_elements.begin() + index);
}

// Clear all elements
void ChronovyanVector::clear() {
    trackResourceUsage("clear");
    m_elements.clear();
}

// Get a slice of the vector
std::shared_ptr<ChronovyanVector> ChronovyanVector::slice(size_t start, size_t end) const {
    if (start > m_elements.size() || end > m_elements.size() || start > end) {
        throw std::out_of_range("Vector slice indices out of range");
    }

    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("slice");

    std::vector<Value> sliced_elements(m_elements.begin() + start, m_elements.begin() + end);

    return std::make_shared<ChronovyanVector>(sliced_elements, m_runtime);
}

// Concatenate with another vector
std::shared_ptr<ChronovyanVector> ChronovyanVector::concat(const ChronovyanVector& other) const {
    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("concat");

    std::vector<Value> combined = m_elements;
    combined.insert(combined.end(), other.getData().begin(), other.getData().end());

    return std::make_shared<ChronovyanVector>(combined, m_runtime);
}

// Apply a function to each element
std::shared_ptr<ChronovyanVector> ChronovyanVector::map(
    std::function<Value(const Value&)> func) const {
    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("map");

    std::vector<Value> mapped;
    mapped.reserve(m_elements.size());

    for (const auto& element : m_elements) {
        mapped.push_back(func(element));
    }

    return std::make_shared<ChronovyanVector>(mapped, m_runtime);
}

// Filter elements based on predicate
std::shared_ptr<ChronovyanVector> ChronovyanVector::filter(
    std::function<bool(const Value&)> pred) const {
    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("filter");

    std::vector<Value> filtered;

    for (const auto& element : m_elements) {
        if (pred(element)) {
            filtered.push_back(element);
        }
    }

    return std::make_shared<ChronovyanVector>(filtered, m_runtime);
}

// Reduce to a single value
Value ChronovyanVector::reduce(std::function<Value(const Value&, const Value&)> func,
                               const Value& initial) const {
    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("reduce");

    Value result = initial;

    for (const auto& element : m_elements) {
        result = func(result, element);
    }

    return result;
}

// Sort the vector
void ChronovyanVector::sort(std::function<bool(const Value&, const Value&)> comparator) {
    trackResourceUsage("sort");

    if (comparator) {
        std::sort(m_elements.begin(), m_elements.end(), comparator);
    } else {
        // Default comparator - try to compare values based on type
        std::sort(m_elements.begin(), m_elements.end(), [](const Value& a, const Value& b) {
            // Simple comparison based on type and value
            if (a.getType() != b.getType()) {
                // Different types, order by type enum
                return static_cast<int>(a.getType()) < static_cast<int>(b.getType());
            }

            // Same type, compare values
            switch (a.getType()) {
                case Value::Type::BOOLEAN:
                    return a.asBool() < b.asBool();
                case Value::Type::NUMBER:
                    return a.asNumber() < b.asNumber();
                case Value::Type::STRING:
                    return a.asString() < b.asString();
                case Value::Type::NIL:
                    return false;  // nil values are equal
                default:
                    // For complex types, use pointer comparison as fallback
                    return &a < &b;
            }
        });
    }
}

// Helper function to compare two Value objects for equality
bool areEqual(const Value& a, const Value& b) {
    if (a.getType() != b.getType()) {
        return false;
    }

    switch (a.getType()) {
        case Value::Type::NIL:
            return true;  // All nil values are equal
        case Value::Type::BOOLEAN:
            return a.asBool() == b.asBool();
        case Value::Type::NUMBER:
            return a.asNumber() == b.asNumber();
        case Value::Type::STRING:
            return a.asString() == b.asString();
        case Value::Type::ARRAY:
            return &a == &b;  // For now, only same array object is equal
        case Value::Type::MAP:
            return &a == &b;  // For now, only same map object is equal
        default:
            return &a == &b;  // For other types, compare addresses
    }
}

// Find index of value in vector
int64_t ChronovyanVector::indexOf(const Value& value, size_t start_index) const {
    if (start_index >= m_elements.size()) {
        return -1;
    }

    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("indexOf");

    for (size_t i = start_index; i < m_elements.size(); ++i) {
        if (areEqual(m_elements[i], value)) {
            return static_cast<int64_t>(i);
        }
    }

    return -1;
}

// Check if vector contains value
bool ChronovyanVector::contains(const Value& value) const { return indexOf(value) != -1; }

// Join elements into string
std::string ChronovyanVector::join(const std::string& separator) const {
    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("join");

    std::stringstream ss;
    bool first = true;

    for (const auto& element : m_elements) {
        if (!first) {
            ss << separator;
        }
        ss << element.toString();
        first = false;
    }

    return ss.str();
}

// String representation
std::string ChronovyanVector::toString() const {
    // Use a mutable function to track resource usage
    const_cast<ChronovyanVector*>(this)->trackResourceUsage("toString");

    std::stringstream ss;
    ss << "[";
    bool first = true;

    for (const auto& element : m_elements) {
        if (!first) {
            ss << ", ";
        }
        ss << element.toString();
        first = false;
    }

    ss << "]";
    return ss.str();
}

// Convert to standard vector
std::vector<Value> ChronovyanVector::toStdVector() const { return m_elements; }

// Get the underlying data
const std::vector<Value>& ChronovyanVector::getData() const { return m_elements; }

// Temporal features

// Get element at a specific cycle
Value ChronovyanVector::getAtCycle(size_t index, int cycle) const {
    // Check if we have history for this cycle
    auto it = m_history.find(cycle);
    if (it == m_history.end()) {
        // No history for this cycle, return current value
        return at(index);
    }

    // Check if index is valid for the historical vector
    if (index >= it->second.size()) {
        throw std::out_of_range("Vector index out of range for historical data");
    }

    return it->second[index];
}

// Save current state as checkpoint
void ChronovyanVector::saveHistoryCheckpoint(int cycle) {
    trackResourceUsage("saveHistoryCheckpoint");
    m_history[cycle] = m_elements;
}

// Restore from checkpoint
bool ChronovyanVector::restoreFromCycle(int cycle) {
    auto it = m_history.find(cycle);
    if (it == m_history.end()) {
        return false;
    }

    trackResourceUsage("restoreFromCycle");
    m_elements = it->second;
    return true;
}

// Get all cycles with history
std::vector<int> ChronovyanVector::getHistoricalCycles() const {
    std::vector<int> cycles;
    cycles.reserve(m_history.size());

    for (const auto& entry : m_history) {
        cycles.push_back(entry.first);
    }

    return cycles;
}

// Clear history
void ChronovyanVector::clearHistory() {
    trackResourceUsage("clearHistory");
    m_history.clear();
}

// Set temporal runtime
void ChronovyanVector::setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime) {
    m_runtime = runtime;
}

// Get temporal runtime
std::shared_ptr<TemporalRuntime> ChronovyanVector::getTemporalRuntime() const { return m_runtime; }

// Resource costs
int64_t ChronovyanVector::getChrononsForOperation(const std::string& operation, size_t vectorSize) {
    // Base cost for any operation
    int64_t baseCost = 1;

    // Scaling factor based on vector size (logarithmic to be efficient for large vectors)
    double sizeFactor = vectorSize > 0 ? std::log2(vectorSize + 1) : 0;

    // Operation-specific costs
    if (operation == "init") {
        return baseCost;
    } else if (operation == "init_with_elements") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "push_back" || operation == "pop_back") {
        return baseCost;
    } else if (operation == "insert" || operation == "erase") {
        // These operations can shift elements, so cost increases with size
        return baseCost + static_cast<int64_t>(sizeFactor * 1.5);
    } else if (operation == "clear") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "slice" || operation == "concat") {
        return baseCost + static_cast<int64_t>(sizeFactor * 2);
    } else if (operation == "map" || operation == "filter") {
        // Linear operations that touch every element
        return baseCost + static_cast<int64_t>(vectorSize);
    } else if (operation == "reduce") {
        return baseCost + static_cast<int64_t>(vectorSize);
    } else if (operation == "sort") {
        // Sorting is O(n log n)
        return baseCost + static_cast<int64_t>(vectorSize * sizeFactor);
    } else if (operation == "indexOf" || operation == "contains") {
        // Linear search
        return baseCost + static_cast<int64_t>(vectorSize / 2);  // Average case
    } else if (operation == "join" || operation == "toString") {
        return baseCost + static_cast<int64_t>(vectorSize);
    } else if (operation == "saveHistoryCheckpoint" || operation == "restoreFromCycle") {
        return baseCost + static_cast<int64_t>(vectorSize * 2);
    } else if (operation == "clearHistory") {
        // This is a static method, so we can't access m_history directly
        // Just provide a reasonable cost estimate based on vectorSize
        return baseCost + static_cast<int64_t>(vectorSize);
    }

    // Default cost for unknown operations
    return baseCost;
}

// Track resource usage for operations
void ChronovyanVector::trackResourceUsage(const std::string& operation) {
    if (m_runtime) {
        int64_t cost = getChrononsForOperation(operation, m_elements.size());
        m_runtime->consumeChronons(static_cast<int>(cost));
    }
}

// Register the VECTOR type with the custom type system
void registerVectorType(CustomTypeSystem& typeSystem) {
    // Define properties for the VECTOR type
    auto vectorProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"container", Value(true)},
        {"temporal", Value(true)},  // Vector supports temporal features
        {"description", Value("Advanced vector type with temporal features")}};

    // Type parameters for generic vector
    auto vectorParams = std::vector<std::string>{"T"};

    // Create the type definition
    auto vectorType = std::make_shared<CustomTypeDefinition>("VECTOR", CustomTypeKind::STRUCT,
                                                             vectorProps, vectorParams);

    // Register the type
    typeSystem.registerType(vectorType);
}

// Add implementation for registerMultiDimensionalVectorType
void registerMultiDimensionalVectorType(CustomTypeSystem& typeSystem) {
    // Create a type definition for multi-dimensional vector
    auto multiVectorType = std::make_shared<CustomTypeDefinition>(
        "MULTIVECTOR",
        CustomTypeKind::ALIAS,          // It's essentially an alias for a specialized vector
        std::map<std::string, Value>()  // No special properties needed
    );

    // Register the type with the type system
    typeSystem.registerType(multiVectorType);
}

}  // namespace chronovyan