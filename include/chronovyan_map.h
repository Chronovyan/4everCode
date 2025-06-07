#ifndef CHRONOVYAN_MAP_H
#define CHRONOVYAN_MAP_H

#include <map>
#include <memory>
#include <string>

namespace chronovyan {

// Forward declarations
class Value;

/**
 * @class ChronovyanMap
 * @brief Represents a map value in the Chronovyan language
 *
 * A map is a collection of key-value pairs where keys are strings.
 */
class ChronovyanMap : public std::enable_shared_from_this<ChronovyanMap> {
public:
    /**
     * @brief Default constructor
     */
    ChronovyanMap() = default;

    /**
     * @brief Constructor
     * @param elements The initial elements of the map
     */
    explicit ChronovyanMap(std::map<std::string, Value> elements);

    /**
     * @brief Get the size of the map
     * @return The number of key-value pairs in the map
     */
    size_t size() const;

    /**
     * @brief Check if the map contains a key
     * @param key The key to check
     * @return Whether the map contains the key
     */
    bool contains(const std::string& key) const;

    /**
     * @brief Get a value from the map
     * @param key The key to look up
     * @return The value associated with the key
     * @throws std::out_of_range if the key is not found
     */
    const Value& at(const std::string& key) const;

    /**
     * @brief Get a mutable reference to a value in the map
     * @param key The key to look up
     * @return A reference to the value associated with the key
     * @throws std::out_of_range if the key is not found
     */
    Value& at(const std::string& key);

    /**
     * @brief Set a value in the map
     * @param key The key to set
     * @param value The value to set
     */
    void set(const std::string& key, Value value);

    /**
     * @brief Get all elements of the map
     * @return A map of all key-value pairs
     */
    std::map<std::string, Value> getElements() const { return m_elements; }

private:
    std::map<std::string, Value> m_elements;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_MAP_H