#include <cstring>  // For std::memcpy
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "compiler/BytecodeGenerator.h"

namespace chronovyan {
namespace compiler {

BytecodeGenerator::BytecodeGenerator(DiagnosticReporter& diagnostics)
    : diagnostics(diagnostics), labelCounter(0), hasError(false) {}

bytecode::BytecodeModule BytecodeGenerator::generateBytecode(const IRProgram& irProgram) {
    bytecode::BytecodeModule module("generated_module");
    constantPool.clear();
    labelCounter = 0;
    hasError = false;

    // Process each function in the IR program
    for (const auto& irFunction : irProgram.getFunctions()) {
        auto bytecodeFunction = std::make_shared<bytecode::BytecodeFunction>(irFunction.getName());

        // Add function parameters
        for (const auto& param : irFunction.getParameters()) {
            bytecodeFunction->addParameter(param);
        }

        // First pass: collect label offsets for jumps
        std::unordered_map<std::string, size_t> labelOffsets;
        size_t instructionOffset = 0;

        // In the current IRFunction class, we don't have direct access to blocks
        // We'll need to implement a way to get basic blocks from the function
        // For now, we'll skip this part
        // TODO: Implement basic block handling when IRFunction is updated

        // Second pass: generate bytecode instructions
        // In the current implementation, we don't have direct access to instructions
        // We'll need to update the IRFunction class to provide access to instructions
        // For now, we'll skip this part
        // TODO: Implement instruction generation when IRFunction is updated

        // Add the function to the module
        module.addFunction(bytecodeFunction);
    }

    // Add all constants to the module
    for (const auto& constant : constantPool) {
        module.addConstant(constant);
    }

    return module;
}

void BytecodeGenerator::generateInstructionForFunction(
    std::shared_ptr<bytecode::BytecodeFunction> function, const IRInstruction& instruction,
    const std::unordered_map<std::string, size_t>& labelOffsets) {
    // Map IR opcodes to bytecode opcodes
    bytecode::OpCode bytecodeOpcode;
    const auto& operands = instruction.getOperands();

    switch (instruction.getOpcode()) {
        case IROpCode::NOP:
            bytecodeOpcode = bytecode::OpCode::NOP;
            break;
        case IROpCode::PUSH:
            bytecodeOpcode = bytecode::OpCode::PUSH_CONST;
            break;
        case IROpCode::POP:
            bytecodeOpcode = bytecode::OpCode::POP;
            break;
        case IROpCode::ADD:
            bytecodeOpcode = bytecode::OpCode::ADD;
            break;
        case IROpCode::SUB:
            bytecodeOpcode = bytecode::OpCode::SUB;
            break;
        case IROpCode::MUL:
            bytecodeOpcode = bytecode::OpCode::MUL;
            break;
        case IROpCode::DIV:
            bytecodeOpcode = bytecode::OpCode::DIV;
            break;
        case IROpCode::LOAD_VAR:
            bytecodeOpcode = bytecode::OpCode::LOAD;  // Changed from LOAD_VAR to LOAD
            break;
        case IROpCode::STORE_VAR:
            bytecodeOpcode = bytecode::OpCode::STORE;  // Changed from STORE_VAR to STORE
            break;
        case IROpCode::JUMP:
            bytecodeOpcode = bytecode::OpCode::JUMP;
            break;
        case IROpCode::JUMP_IF_TRUE:
            bytecodeOpcode = bytecode::OpCode::JUMP_IF_TRUE;
            break;
        case IROpCode::JUMP_IF_FALSE:
            bytecodeOpcode = bytecode::OpCode::JUMP_IF_FALSE;
            break;
        case IROpCode::CALL:
            bytecodeOpcode = bytecode::OpCode::CALL;
            break;
        case IROpCode::RETURN:
            bytecodeOpcode = bytecode::OpCode::RETURN;
            break;
        case IROpCode::REWIND:
        case IROpCode::BRANCH:
        case IROpCode::MERGE:
            // Handle temporal operations specially
            handleTemporalOperation(function, instruction);
            return;
        default:
            // Unhandled opcodes default to NOP
            diagnostics.addWarning(instruction.getLocation(),
                                   "Unhandled IR opcode, using NOP in bytecode generation");
            bytecodeOpcode = bytecode::OpCode::NOP;
            break;
    }

    // Create the bytecode instruction
    bytecode::BytecodeInstruction bytecodeInstruction(bytecodeOpcode);

    // Add operands based on the instruction type
    switch (instruction.getOpcode()) {
        case IROpCode::PUSH: {
            if (!operands.empty()) {
                // Convert IR value to constant and add to pool
                bytecode::BytecodeConstant constant = convertIRValueToConstant(operands[0]);
                size_t constantIndex = addConstant(constant);
                bytecodeInstruction.addOperand(static_cast<uint16_t>(constantIndex));
            }
            break;
        }
        case IROpCode::LOAD_VAR:
        case IROpCode::STORE_VAR: {
            // Use variable index as operand
            if (!operands.empty()) {
                uint16_t varIndex = 0;  // Placeholder - should get from symbol table
                bytecodeInstruction.addOperand(varIndex);
            }
            break;
        }
        case IROpCode::JUMP:
        case IROpCode::JUMP_IF_TRUE:
        case IROpCode::JUMP_IF_FALSE: {
            // Use label offset as operand
            if (!operands.empty()) {
                std::string label = operands[0].getStringValue();
                auto it = labelOffsets.find(label);
                if (it != labelOffsets.end()) {
                    bytecodeInstruction.addOperand(static_cast<uint16_t>(it->second));
                } else {
                    diagnostics.addError(instruction.getLocation(),
                                         "Unknown jump target: " + label);
                    hasError = true;
                    bytecodeInstruction.addOperand(0);  // Default to offset 0
                }
            }
            break;
        }
        case IROpCode::CALL: {
            // Use function index and argument count as operands
            if (operands.size() >= 2) {
                uint16_t funcIndex = 0;  // Placeholder - should get from symbol table
                uint16_t argCount = static_cast<uint16_t>(operands[1].getIntValue());
                bytecodeInstruction.addOperand(funcIndex);
                bytecodeInstruction.addOperand(argCount);
            }
            break;
        }
        default:
            // No operands for other instructions
            break;
    }

    // Add the instruction to the function
    function->addInstruction(bytecodeInstruction);
}

void BytecodeGenerator::handleTemporalOperation(
    std::shared_ptr<bytecode::BytecodeFunction> function, const IRInstruction& instruction) {
    const auto& operands = instruction.getOperands();
    bytecode::OpCode bytecodeOpcode;

    // Map the temporal operation to a bytecode opcode
    switch (instruction.getOpcode()) {
        case IROpCode::REWIND:
            bytecodeOpcode = bytecode::OpCode::REWIND;
            break;
        case IROpCode::BRANCH:
            bytecodeOpcode = bytecode::OpCode::BRANCH;
            break;
        case IROpCode::MERGE:
            bytecodeOpcode = bytecode::OpCode::MERGE;
            break;
        default:
            // Unrecognized temporal operation
            diagnostics.addWarning(instruction.getLocation(), "Unrecognized temporal operation");
            bytecodeOpcode = bytecode::OpCode::NOP;
            break;
    }

    // Create the bytecode instruction
    bytecode::BytecodeInstruction bytecodeInstruction(bytecodeOpcode);

    // Add operands based on the operation type
    if (instruction.getOpcode() == IROpCode::REWIND && !operands.empty()) {
        // Add time steps as operand
        uint16_t steps = static_cast<uint16_t>(operands[0].getIntValue());
        bytecodeInstruction.addOperand(steps);
    } else if (instruction.getOpcode() == IROpCode::BRANCH) {
        // Add branch ID as operand
        uint16_t branchId = 0;  // Placeholder
        bytecodeInstruction.addOperand(branchId);
    }

    // Add the instruction to the function
    function->addInstruction(bytecodeInstruction);
}

bytecode::BytecodeConstant BytecodeGenerator::convertIRValueToConstant(const IRValue& value) {
    switch (value.getType()) {
        case IRValueType::INTEGER:
            return bytecode::BytecodeConstant(static_cast<int>(value.getIntValue()));
        case IRValueType::FLOAT:
            return bytecode::BytecodeConstant(value.getFloatValue());
        case IRValueType::STRING:
            return bytecode::BytecodeConstant(value.getStringValue());
        case IRValueType::BOOLEAN:
            return bytecode::BytecodeConstant(value.getBoolValue());
        default:
            // For unsupported types, return a null constant
            return bytecode::BytecodeConstant();
    }
}

size_t BytecodeGenerator::addConstant(const bytecode::BytecodeConstant& constant) {
    // Check if the constant already exists in the pool
    for (size_t i = 0; i < constantPool.size(); ++i) {
        // This is a simplified comparison; a real implementation would compare based on type
        if (constant.getType() == constantPool[i].getType()) {
            switch (constant.getType()) {
                case bytecode::BytecodeConstant::Type::INTEGER:
                    if (constant.getIntValue() == constantPool[i].getIntValue()) {
                        return i;
                    }
                    break;
                case bytecode::BytecodeConstant::Type::FLOAT:
                    if (constant.getFloatValue() == constantPool[i].getFloatValue()) {
                        return i;
                    }
                    break;
                case bytecode::BytecodeConstant::Type::STRING:
                    if (constant.getStringValue() == constantPool[i].getStringValue()) {
                        return i;
                    }
                    break;
                case bytecode::BytecodeConstant::Type::BOOLEAN:
                    if (constant.getBoolValue() == constantPool[i].getBoolValue()) {
                        return i;
                    }
                    break;
                default:
                    // For NULL_VALUE, consider them equal
                    if (constant.getType() == bytecode::BytecodeConstant::Type::NULL_VALUE) {
                        return i;
                    }
                    break;
            }
        }
    }

    // Constant not found, add it to the pool
    constantPool.push_back(constant);
    return constantPool.size() - 1;
}

std::string BytecodeGenerator::generateLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(labelCounter++);
}

int BytecodeGenerator::calculateStackEffect(const IRInstruction& instruction) {
    // Calculate the effect on the stack (positive for push, negative for pop)
    const auto& operands = instruction.getOperands();

    switch (instruction.getOpcode()) {
        case IROpCode::PUSH:
            return 1;  // Pushes 1 value
        case IROpCode::POP:
            return -1;  // Pops 1 value
        case IROpCode::ADD:
        case IROpCode::SUB:
        case IROpCode::MUL:
        case IROpCode::DIV:
            return -1;  // Pops 2, pushes 1, net -1
        case IROpCode::LOAD_VAR:
            return 1;  // Pushes 1 value
        case IROpCode::STORE_VAR:
            return -1;  // Pops 1 value
        case IROpCode::CALL: {
            // Pops args, pushes result
            if (operands.size() >= 2) {
                int argCount = static_cast<int>(operands[1].getIntValue());
                return 1 - argCount;  // Pops args, pushes 1 result
            }
            return 0;
        }
        case IROpCode::RETURN:
            return -1;  // Pops return value
        default:
            return 0;  // No effect for other instructions
    }
}

}  // namespace compiler
}  // namespace chronovyan