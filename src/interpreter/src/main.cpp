#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "compiler/bytecode/BytecodeModule.h"
#include "interpreter/BytecodeInterpreter.h"
#include "stdlib/paradox_management.h"

namespace fs = std::filesystem;

namespace chronovyan {
namespace interpreter {

// Load bytecode module from a binary file
bool deserializeBytecodeModule(const std::string& inputFile,
                               compiler::bytecode::BytecodeModule& module) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Error: Could not open input file: " << inputFile << std::endl;
        return false;
    }

    // Read header
    char header[4];
    in.read(header, 4);
    if (in.gcount() != 4 || header[0] != 'C' || header[1] != 'H' || header[2] != 'R' ||
        header[3] != 'O') {
        std::cerr << "Error: Invalid bytecode file format. Not a Chronovyan bytecode file."
                  << std::endl;
        return false;
    }

    // Read version
    uint8_t version;
    in.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (in.gcount() != sizeof(version) || version != 1) {
        std::cerr << "Error: Unsupported bytecode version: " << static_cast<int>(version)
                  << std::endl;
        return false;
    }

    // Read constant pool
    uint32_t constantPoolSize;
    in.read(reinterpret_cast<char*>(&constantPoolSize), sizeof(constantPoolSize));
    if (in.gcount() != sizeof(constantPoolSize)) {
        std::cerr << "Error: Failed to read constant pool size" << std::endl;
        return false;
    }

    std::vector<compiler::bytecode::BytecodeConstant> constants;
    constants.reserve(constantPoolSize);

    for (uint32_t i = 0; i < constantPoolSize; ++i) {
        // Read constant type
        uint8_t type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        if (in.gcount() != sizeof(type)) {
            std::cerr << "Error: Failed to read constant type" << std::endl;
            return false;
        }

        // Read constant value based on type
        switch (static_cast<compiler::bytecode::BytecodeConstant::Type>(type)) {
            case compiler::bytecode::BytecodeConstant::Type::NULL_VALUE:
                constants.emplace_back(compiler::bytecode::BytecodeConstant());
                break;
            case compiler::bytecode::BytecodeConstant::Type::BOOLEAN: {
                bool value;
                in.read(reinterpret_cast<char*>(&value), sizeof(value));
                if (in.gcount() != sizeof(value)) {
                    std::cerr << "Error: Failed to read boolean constant" << std::endl;
                    return false;
                }
                constants.emplace_back(compiler::bytecode::BytecodeConstant(value));
            } break;
            case compiler::bytecode::BytecodeConstant::Type::INTEGER: {
                int32_t value;
                in.read(reinterpret_cast<char*>(&value), sizeof(value));
                if (in.gcount() != sizeof(value)) {
                    std::cerr << "Error: Failed to read integer constant" << std::endl;
                    return false;
                }
                constants.emplace_back(compiler::bytecode::BytecodeConstant(value));
            } break;
            case compiler::bytecode::BytecodeConstant::Type::FLOAT: {
                double value;
                in.read(reinterpret_cast<char*>(&value), sizeof(value));
                if (in.gcount() != sizeof(value)) {
                    std::cerr << "Error: Failed to read float constant" << std::endl;
                    return false;
                }
                constants.emplace_back(compiler::bytecode::BytecodeConstant(value));
            } break;
            case compiler::bytecode::BytecodeConstant::Type::STRING: {
                uint32_t length;
                in.read(reinterpret_cast<char*>(&length), sizeof(length));
                if (in.gcount() != sizeof(length)) {
                    std::cerr << "Error: Failed to read string length" << std::endl;
                    return false;
                }

                std::string value(length, '\0');
                in.read(&value[0], length);
                if (in.gcount() != length) {
                    std::cerr << "Error: Failed to read string constant" << std::endl;
                    return false;
                }
                constants.emplace_back(compiler::bytecode::BytecodeConstant(value));
            } break;
            default:
                std::cerr << "Error: Unknown constant type: " << static_cast<int>(type)
                          << std::endl;
                return false;
        }
    }

    // Read functions
    uint32_t functionCount;
    in.read(reinterpret_cast<char*>(&functionCount), sizeof(functionCount));
    if (in.gcount() != sizeof(functionCount)) {
        std::cerr << "Error: Failed to read function count" << std::endl;
        return false;
    }

    std::vector<std::shared_ptr<compiler::bytecode::BytecodeFunction>> functions;
    functions.reserve(functionCount);

    for (uint32_t i = 0; i < functionCount; ++i) {
        // Read function name
        uint32_t nameLength;
        in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        if (in.gcount() != sizeof(nameLength)) {
            std::cerr << "Error: Failed to read function name length" << std::endl;
            return false;
        }

        std::string name(nameLength, '\0');
        in.read(&name[0], nameLength);
        if (in.gcount() != nameLength) {
            std::cerr << "Error: Failed to read function name" << std::endl;
            return false;
        }

        // Read parameter count
        uint16_t paramCount;
        in.read(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));
        if (in.gcount() != sizeof(paramCount)) {
            std::cerr << "Error: Failed to read function parameter count" << std::endl;
            return false;
        }

        // Read instruction count
        uint32_t instructionCount;
        in.read(reinterpret_cast<char*>(&instructionCount), sizeof(instructionCount));
        if (in.gcount() != sizeof(instructionCount)) {
            std::cerr << "Error: Failed to read instruction count" << std::endl;
            return false;
        }

        std::vector<compiler::bytecode::BytecodeInstruction> instructions;
        instructions.reserve(instructionCount);

        for (uint32_t j = 0; j < instructionCount; ++j) {
            // Read opcode
            uint8_t opcode;
            in.read(reinterpret_cast<char*>(&opcode), sizeof(opcode));
            if (in.gcount() != sizeof(opcode)) {
                std::cerr << "Error: Failed to read instruction opcode" << std::endl;
                return false;
            }

            // Read operand count
            uint8_t operandCount;
            in.read(reinterpret_cast<char*>(&operandCount), sizeof(operandCount));
            if (in.gcount() != sizeof(operandCount)) {
                std::cerr << "Error: Failed to read operand count" << std::endl;
                return false;
            }

            std::vector<uint16_t> operands;
            operands.reserve(operandCount);

            // Read operands
            for (uint8_t k = 0; k < operandCount; ++k) {
                uint16_t operand;
                in.read(reinterpret_cast<char*>(&operand), sizeof(operand));
                if (in.gcount() != sizeof(operand)) {
                    std::cerr << "Error: Failed to read instruction operand" << std::endl;
                    return false;
                }
                operands.push_back(operand);
            }

            // Create instruction with opcode and add operands one by one
            compiler::bytecode::BytecodeInstruction instr(
                static_cast<compiler::bytecode::OpCode>(opcode));
            for (auto operand : operands) {
                instr.addOperand(operand);
            }
            instructions.push_back(instr);
        }

        // Create function and add instructions
        auto function = std::make_shared<compiler::bytecode::BytecodeFunction>(name);
        for (const auto& instr : instructions) {
            function->addInstruction(instr);
        }
        functions.push_back(function);
    }

    // Create the bytecode module
    module = compiler::bytecode::BytecodeModule("module");

    // Add constants
    for (const auto& constant : constants) {
        module.addConstant(constant);
    }

    // Add functions
    for (const auto& function : functions) {
        module.addFunction(function);
    }

    return true;
}

// Run a bytecode file
bool runBytecodeFile(const std::string& inputFile, bool verbose) {
    if (verbose) {
        std::cout << "Running bytecode file: " << inputFile << std::endl;
    }

    // Initialize paradox management subsystem
    if (!stdlib::initialize_paradox_management()) {
        std::cerr << "Failed to initialize paradox management subsystem" << std::endl;
        return false;
    }

    if (verbose) {
        std::cout << "Paradox management subsystem initialized" << std::endl;
    }

    // Load bytecode module
    compiler::bytecode::BytecodeModule module;
    if (!deserializeBytecodeModule(inputFile, module)) {
        stdlib::shutdown_paradox_management();
        return false;
    }

    if (verbose) {
        std::cout << "Bytecode loaded successfully:" << std::endl;
        std::cout << "  Constants: " << module.getConstantPoolSize() << std::endl;
        std::cout << "  Functions: " << module.getFunctions().size() << std::endl;

        for (const auto& pair : module.getFunctions()) {
            const auto& function = pair.second;
            std::cout << "  Function '" << function->getName()
                      << "': " << function->getInstructions().size() << " instructions, "
                      << function->getParameterCount() << " parameters" << std::endl;
        }
    }

    // Create and initialize interpreter
    BytecodeInterpreter interpreter;
    interpreter.loadModule(module);

    try {
        // Run the main function
        std::vector<Value> args;
        Value result = interpreter.execute(args);

        if (verbose) {
            std::cout << "Program executed successfully with result: " << result.toString()
                      << std::endl;
            std::cout << "Final PARADOX_LEVEL: " << stdlib::get_paradox_level() << std::endl;
        }

        // Shutdown paradox management subsystem
        stdlib::shutdown_paradox_management();
        return true;
    } catch (const RuntimeException& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        stdlib::shutdown_paradox_management();
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        stdlib::shutdown_paradox_management();
        return false;
    } catch (...) {
        std::cerr << "Unknown error occurred during execution" << std::endl;
        stdlib::shutdown_paradox_management();
        return false;
    }
}

}  // namespace interpreter
}  // namespace chronovyan

// Entry point for the Chronovyan interpreter
int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string inputFile;
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        } else {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Usage: " << argv[0] << " [options] bytecode_file" << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  -v, --verbose    Enable verbose output" << std::endl;
        return 1;
    }

    // Run the bytecode file
    bool success = chronovyan::interpreter::runBytecodeFile(inputFile, verbose);
    return success ? 0 : 1;
}