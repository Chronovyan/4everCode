#ifndef CHRONOVYAN_COMPILER_BYTECODE_MODULE_H
#define CHRONOVYAN_COMPILER_BYTECODE_MODULE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace chronovyan {
namespace compiler {
namespace bytecode {

/**
 * @brief Represents a constant value in bytecode
 */
class BytecodeConstant {
public:
    enum class Type { INT, FLOAT, STRING, BOOL, NULL_VALUE };

    BytecodeConstant() : type(Type::NULL_VALUE), value(0) {}
    explicit BytecodeConstant(int intValue) : type(Type::INT), value(intValue) {}
    explicit BytecodeConstant(double floatValue) : type(Type::FLOAT), value(floatValue) {}
    explicit BytecodeConstant(std::string stringValue)
        : type(Type::STRING), value(std::move(stringValue)) {}
    explicit BytecodeConstant(bool boolValue) : type(Type::BOOL), value(boolValue) {}

    Type getType() const { return type; }

    int getIntValue() const { return std::get<int>(value); }
    double getFloatValue() const { return std::get<double>(value); }
    const std::string& getStringValue() const { return std::get<std::string>(value); }
    bool getBoolValue() const { return std::get<bool>(value); }

private:
    Type type;
    std::variant<int, double, std::string, bool> value;
};

/**
 * @brief Represents a bytecode instruction
 */
class BytecodeInstruction {
public:
    BytecodeInstruction() = default;

    // Basic getters/setters will be implemented later

private:
    // Instruction data will be added later
};

/**
 * @brief Represents a function in bytecode
 */
class BytecodeFunction {
public:
    BytecodeFunction(const std::string& name) : name(name) {}

    const std::string& getName() const { return name; }

    void addInstruction(const BytecodeInstruction& instruction) {
        instructions.push_back(instruction);
    }

    const std::vector<BytecodeInstruction>& getInstructions() const { return instructions; }

private:
    std::string name;
    std::vector<BytecodeInstruction> instructions;
};

/**
 * @brief Represents a module of bytecode
 */
class BytecodeModule {
public:
    BytecodeModule() = default;
    explicit BytecodeModule(const std::string& name) : name(name) {}

    const std::string& getName() const { return name; }

    void addFunction(std::shared_ptr<BytecodeFunction> function) {
        functions[function->getName()] = function;
    }

    std::shared_ptr<BytecodeFunction> getFunction(const std::string& name) {
        auto it = functions.find(name);
        if (it != functions.end()) {
            return it->second;
        }
        return nullptr;
    }

    const std::unordered_map<std::string, std::shared_ptr<BytecodeFunction>>& getFunctions() const {
        return functions;
    }

private:
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<BytecodeFunction>> functions;
};

}  // namespace bytecode
}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_BYTECODE_MODULE_H