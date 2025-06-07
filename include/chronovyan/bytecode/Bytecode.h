#ifndef CHRONOVYAN_COMPILER_BYTECODE_H
#define CHRONOVYAN_COMPILER_BYTECODE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../SourceLocation.h"

namespace chronovyan {
namespace compiler {
namespace bytecode {

/**
 * @brief Enumeration of bytecode operation codes
 */
enum class OpCode {
    // Stack operations
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

    // Debug operations
    DEBUG_PRINT,

    // Special operations
    NOP,
    HALT
};

/**
 * @brief Represents a single bytecode instruction
 */
class Instruction {
public:
    Instruction(OpCode opcode) : opcode(opcode) {}
    Instruction(OpCode opcode, int operand) : opcode(opcode), operands({operand}) {}

    OpCode getOpCode() const { return opcode; }
    const std::vector<int>& getOperands() const { return operands; }

    void addOperand(int operand) { operands.push_back(operand); }

private:
    OpCode opcode;
    std::vector<int> operands;
    SourceLocation location;
};

/**
 * @brief Represents a constant in the constant pool
 */
class Constant {
public:
    enum class Type { INTEGER, FLOAT, STRING, BOOLEAN };

    explicit Constant(int value) : type(Type::INTEGER), intValue(value) {}
    explicit Constant(double value) : type(Type::FLOAT), floatValue(value) {}
    explicit Constant(const std::string& value) : type(Type::STRING), stringValue(value) {}
    explicit Constant(bool value) : type(Type::BOOLEAN), boolValue(value) {}

    Type getType() const { return type; }

    int getIntValue() const { return intValue; }
    double getFloatValue() const { return floatValue; }
    const std::string& getStringValue() const { return stringValue; }
    bool getBoolValue() const { return boolValue; }

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
class Function {
public:
    explicit Function(const std::string& name, int numParams = 0)
        : name(name), numParameters(numParams) {}

    const std::string& getName() const { return name; }
    int getNumParameters() const { return numParameters; }

    void addInstruction(const Instruction& instruction) { instructions.push_back(instruction); }

    const std::vector<Instruction>& getInstructions() const { return instructions; }

private:
    std::string name;
    int numParameters;
    std::vector<Instruction> instructions;
};

/**
 * @brief Represents a complete bytecode module
 */
class Module {
public:
    Module() = default;
    explicit Module(const std::string& name) : name(name) {}

    const std::string& getName() const { return name; }

    void addFunction(std::shared_ptr<Function> function) {
        functions[function->getName()] = function;
    }

    void addConstant(const Constant& constant) { constants.push_back(constant); }

    int getConstantIndex(const Constant& constant) {
        // This is a simplification - a real implementation would check if the constant already
        // exists
        constants.push_back(constant);
        return constants.size() - 1;
    }

    const std::vector<Constant>& getConstants() const { return constants; }

    std::shared_ptr<Function> getFunction(const std::string& name) {
        auto it = functions.find(name);
        if (it != functions.end()) {
            return it->second;
        }
        return nullptr;
    }

    const std::unordered_map<std::string, std::shared_ptr<Function>>& getFunctions() const {
        return functions;
    }

private:
    std::string name;
    std::vector<Constant> constants;
    std::unordered_map<std::string, std::shared_ptr<Function>> functions;
};

}  // namespace bytecode
}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_BYTECODE_H