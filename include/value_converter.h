#ifndef CHRONOVYAN_VALUE_CONVERTER_H
#define CHRONOVYAN_VALUE_CONVERTER_H

#include "enum_variant.h"
#include "value.h"

namespace chronovyan {

// Forward declarations
class Interpreter;

/**
 * @class ValueConverter
 * @brief Utility class for converting between different value representations
 *
 * The ValueConverter class provides static methods for converting between
 * different value representations, such as from EnumVariant to Value.
 */
class ValueConverter {
public:
    /**
     * @brief Convert an EnumVariant to a Value
     * @param variant The EnumVariant to convert
     * @param interpreter The interpreter context
     * @return The Value representation of the variant
     */
    static Value fromEnumVariant(const EnumVariant& variant, Interpreter& interpreter) {
        if (!variant.hasValue()) {
            return Value();  // Nil value
        }

        if (variant.isString()) {
            return Value(variant.asString());
        }

        if (variant.isInteger()) {
            return Value(variant.asInteger());
        }

        if (variant.isDouble()) {
            return Value(variant.asDouble());
        }

        if (variant.isBoolean()) {
            return Value(variant.asBoolean());
        }

        return Value();  // Default to nil value
    }

    /**
     * @brief Convert a Value to a boolean
     * @param value The Value to convert
     * @return The boolean representation of the value
     */
    static bool toBoolean(const Value& value) {
        if (value.isNil()) {
            return false;
        }

        if (value.isBoolean()) {
            return value.asBoolean();
        }

        if (value.isNumeric()) {
            return value.asNumber() != 0.0;
        }

        if (value.isString()) {
            return !value.asString().empty();
        }

        // Arrays, maps, instances, and other types are truthy
        return true;
    }

    /**
     * @brief Convert a Value to a string
     * @param value The Value to convert
     * @return The string representation of the value
     */
    static std::string toString(const Value& value) { return value.toString(); }

    /**
     * @brief Try to convert a Value to a number
     * @param value The Value to convert
     * @return The numeric value, or 0.0 if conversion fails
     */
    static double toNumber(const Value& value) {
        if (value.isNumeric()) {
            return value.asNumber();
        }

        if (value.isBoolean()) {
            return value.asBoolean() ? 1.0 : 0.0;
        }

        if (value.isString()) {
            try {
                return std::stod(value.asString());
            } catch (...) {
                return 0.0;
            }
        }

        return 0.0;
    }
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_VALUE_CONVERTER_H