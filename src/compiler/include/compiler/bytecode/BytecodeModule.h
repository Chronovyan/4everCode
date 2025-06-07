#ifndef CHRONOVYAN_COMPILER_BYTECODE_MODULE_H
#define CHRONOVYAN_COMPILER_BYTECODE_MODULE_H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace chronovyan {
namespace compiler {
namespace bytecode {

// Forward declaration
class BytecodeConstant;

/**
 * @brief Enumeration of bytecode operation codes
 */
enum class OpCode : uint8_t {
    // Stack operations
    NOP = 0,
    PUSH_CONST,
    PUSH_VAR,
    POP,
    DUP,
    SWAP,

    // Memory operations
    LOAD,
    STORE,

    // Arithmetic operations
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NEG,

    // Logical operations
    AND,
    OR,
    NOT,

    // Comparison operations
    EQ,
    NE,
    LT,
    LE,
    GT,
    GE,

    // Control flow
    JUMP,
    JUMP_IF_TRUE,
    JUMP_IF_FALSE,
    CALL,
    RETURN,

    // Temporal operations
    REWIND,
    BRANCH,
    MERGE,
    STABILIZE,

    // Resource operations
    CONSUME_RESOURCE,
    CHECK_RESOURCE,

    // Paradox management operations
    CHECK_PARADOX_LEVEL,
    REDUCE_PARADOX_LEVEL,
    CREATE_CONTAINMENT_FIELD,
    REGISTER_DEPENDENCY,
    CHECK_DEPENDENCY_CONFLICT,

    // Debug operations
    DEBUG_PRINT,

    // Special operations
    HALT
};

/**
 * @brief Represents a bytecode instruction
 */
class BytecodeInstruction {
public:
    BytecodeInstruction() : opcode(OpCode::NOP) {}
    BytecodeInstruction(OpCode opcode) : opcode(opcode) {}
    BytecodeInstruction(OpCode opcode, uint16_t operand) : opcode(opcode) {
        operands.push_back(operand);
    }

    OpCode getOpcode() const { return opcode; }
    const std::vector<uint16_t>& getOperands() const { return operands; }

    void addOperand(uint16_t operand) { operands.push_back(operand); }

    // Serialize the instruction to a byte vector
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> bytes;
        bytes.push_back(static_cast<uint8_t>(opcode));
        bytes.push_back(static_cast<uint8_t>(operands.size()));

        for (uint16_t operand : operands) {
            bytes.push_back(static_cast<uint8_t>(operand >> 8));    // High byte
            bytes.push_back(static_cast<uint8_t>(operand & 0xFF));  // Low byte
        }

        return bytes;
    }

private:
    OpCode opcode;
    std::vector<uint16_t> operands;
};

/**
 * @brief Represents a constant in the bytecode
 */
class BytecodeConstant {
public:
    enum class Type { NULL_VALUE, INTEGER, FLOAT, STRING, BOOLEAN };

    BytecodeConstant() : type(Type::NULL_VALUE), intValue(0) {}
    explicit BytecodeConstant(int value) : type(Type::INTEGER), intValue(value) {}
    explicit BytecodeConstant(double value) : type(Type::FLOAT), floatValue(value) {}
    explicit BytecodeConstant(const std::string& value) : type(Type::STRING), stringValue(value) {}
    explicit BytecodeConstant(bool value) : type(Type::BOOLEAN), boolValue(value) {}

    Type getType() const { return type; }

    int getIntValue() const { return intValue; }
    double getFloatValue() const { return floatValue; }
    const std::string& getStringValue() const { return stringValue; }
    bool getBoolValue() const { return boolValue; }

    // Serialize the constant to a byte vector
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> bytes;
        bytes.push_back(static_cast<uint8_t>(type));

        switch (type) {
            case Type::INTEGER: {
                // Convert int to bytes (assuming 32-bit int)
                bytes.push_back(static_cast<uint8_t>((intValue >> 24) & 0xFF));
                bytes.push_back(static_cast<uint8_t>((intValue >> 16) & 0xFF));
                bytes.push_back(static_cast<uint8_t>((intValue >> 8) & 0xFF));
                bytes.push_back(static_cast<uint8_t>(intValue & 0xFF));
                break;
            }
            case Type::FLOAT: {
                // Convert double to bytes (this is a simplification - real implementation would
                // handle endianness)
                uint64_t bits;
                std::memcpy(&bits, &floatValue, sizeof(double));
                for (int i = 7; i >= 0; --i) {
                    bytes.push_back(static_cast<uint8_t>((bits >> (i * 8)) & 0xFF));
                }
                break;
            }
            case Type::STRING: {
                // String length followed by string bytes
                uint16_t length = static_cast<uint16_t>(stringValue.size());
                bytes.push_back(static_cast<uint8_t>(length >> 8));
                bytes.push_back(static_cast<uint8_t>(length & 0xFF));

                for (char c : stringValue) {
                    bytes.push_back(static_cast<uint8_t>(c));
                }
                break;
            }
            case Type::BOOLEAN: {
                bytes.push_back(boolValue ? 1 : 0);
                break;
            }
            case Type::NULL_VALUE:
                // No additional data for null
                break;
        }

        return bytes;
    }

private:
    Type type;
    union {
        int intValue;
        double floatValue;
        bool boolValue;
    };
    std::string stringValue;  // Outside union because it has a constructor
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

    // Add parameter info
    void addParameter(const std::string& name) { parameters.push_back(name); }

    size_t getParameterCount() const { return parameters.size(); }

    const std::vector<std::string>& getParameters() const { return parameters; }

    // Serialize the function to a byte vector
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> bytes;

        // Serialize function name length and name
        uint16_t nameLength = static_cast<uint16_t>(name.size());
        bytes.push_back(static_cast<uint8_t>(nameLength >> 8));
        bytes.push_back(static_cast<uint8_t>(nameLength & 0xFF));

        for (char c : name) {
            bytes.push_back(static_cast<uint8_t>(c));
        }

        // Serialize parameter count and names
        uint16_t paramCount = static_cast<uint16_t>(parameters.size());
        bytes.push_back(static_cast<uint8_t>(paramCount >> 8));
        bytes.push_back(static_cast<uint8_t>(paramCount & 0xFF));

        for (const auto& param : parameters) {
            uint16_t paramLength = static_cast<uint16_t>(param.size());
            bytes.push_back(static_cast<uint8_t>(paramLength >> 8));
            bytes.push_back(static_cast<uint8_t>(paramLength & 0xFF));

            for (char c : param) {
                bytes.push_back(static_cast<uint8_t>(c));
            }
        }

        // Serialize instruction count and instructions
        uint32_t instructionCount = static_cast<uint32_t>(instructions.size());
        bytes.push_back(static_cast<uint8_t>((instructionCount >> 24) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((instructionCount >> 16) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((instructionCount >> 8) & 0xFF));
        bytes.push_back(static_cast<uint8_t>(instructionCount & 0xFF));

        for (const auto& instruction : instructions) {
            auto instrBytes = instruction.serialize();
            bytes.insert(bytes.end(), instrBytes.begin(), instrBytes.end());
        }

        return bytes;
    }

private:
    std::string name;
    std::vector<std::string> parameters;
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

    void addConstant(const BytecodeConstant& constant) { constantPool.push_back(constant); }

    size_t getConstantPoolSize() const { return constantPool.size(); }

    const std::vector<BytecodeConstant>& getConstantPool() const { return constantPool; }

    // Serialize the module to a byte vector
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> bytes;

        // Magic number for Chronovyan bytecode (CHRN)
        bytes.push_back('C');
        bytes.push_back('H');
        bytes.push_back('R');
        bytes.push_back('N');

        // Version (1.0)
        bytes.push_back(1);
        bytes.push_back(0);

        // Module name length and name
        uint16_t nameLength = static_cast<uint16_t>(name.size());
        bytes.push_back(static_cast<uint8_t>(nameLength >> 8));
        bytes.push_back(static_cast<uint8_t>(nameLength & 0xFF));

        for (char c : name) {
            bytes.push_back(static_cast<uint8_t>(c));
        }

        // Constant pool size and constants
        uint32_t constPoolSize = static_cast<uint32_t>(constantPool.size());
        bytes.push_back(static_cast<uint8_t>((constPoolSize >> 24) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((constPoolSize >> 16) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((constPoolSize >> 8) & 0xFF));
        bytes.push_back(static_cast<uint8_t>(constPoolSize & 0xFF));

        for (const auto& constant : constantPool) {
            auto constBytes = constant.serialize();
            bytes.insert(bytes.end(), constBytes.begin(), constBytes.end());
        }

        // Function count and functions
        uint32_t functionCount = static_cast<uint32_t>(functions.size());
        bytes.push_back(static_cast<uint8_t>((functionCount >> 24) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((functionCount >> 16) & 0xFF));
        bytes.push_back(static_cast<uint8_t>((functionCount >> 8) & 0xFF));
        bytes.push_back(static_cast<uint8_t>(functionCount & 0xFF));

        for (const auto& functionPair : functions) {
            auto funcBytes = functionPair.second->serialize();
            bytes.insert(bytes.end(), funcBytes.begin(), funcBytes.end());
        }

        return bytes;
    }

private:
    std::string name;
    std::vector<BytecodeConstant> constantPool;
    std::unordered_map<std::string, std::shared_ptr<BytecodeFunction>> functions;
};

}  // namespace bytecode
}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_BYTECODE_MODULE_H