#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// ANSI color codes for terminal output
#ifdef _WIN32
#define RESET ""
#define RED ""
#define GREEN ""
#define YELLOW ""
#define BLUE ""
#define BOLD ""
#else
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define BOLD "\033[1m"
#endif

// Helper function to read the entire contents of a file
std::string read_file(const fs::path& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Warning: Could not open file: " << path << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Helper function to normalize line endings (handles Windows/Unix differences)
std::string normalize_line_endings(const std::string& input) {
    std::string result = input;
    // Replace \r\n with \n
    size_t pos = 0;
    while ((pos = result.find("\r\n", pos)) != std::string::npos) {
        result.replace(pos, 2, "\n");
        pos += 1;
    }
    return result;
}

// Helper function to trim whitespace from the end of a string
std::string trim_end(const std::string& str) {
    auto pos = str.find_last_not_of(" \t\n\r");
    if (pos == std::string::npos) {
        return "";
    }
    return str.substr(0, pos + 1);
}

// Compare outputs with flexible whitespace handling
bool compare_outputs(const std::string& actual, const std::string& expected) {
    // Normalize line endings and trim trailing whitespace
    std::string normalizedActual = trim_end(normalize_line_endings(actual));
    std::string normalizedExpected = trim_end(normalize_line_endings(expected));

    return normalizedActual == normalizedExpected;
}

// Show the difference between actual and expected output
void show_diff(const std::string& actual, const std::string& expected) {
    std::cout << YELLOW << "Diff:" << RESET << std::endl;

    // Normalize line endings
    std::string normalizedActual = normalize_line_endings(actual);
    std::string normalizedExpected = normalize_line_endings(expected);

    // Split into lines
    std::vector<std::string> actualLines, expectedLines;
    std::stringstream actualStream(normalizedActual);
    std::stringstream expectedStream(normalizedExpected);
    std::string line;

    while (std::getline(actualStream, line)) {
        actualLines.push_back(line);
    }

    while (std::getline(expectedStream, line)) {
        expectedLines.push_back(line);
    }

    // Show the diff line by line
    size_t maxLines = std::max(actualLines.size(), expectedLines.size());
    for (size_t i = 0; i < maxLines; i++) {
        if (i < actualLines.size() && i < expectedLines.size()) {
            if (actualLines[i] != expectedLines[i]) {
                std::cout << RED << "- " << expectedLines[i] << RESET << std::endl;
                std::cout << GREEN << "+ " << actualLines[i] << RESET << std::endl;
            } else {
                std::cout << "  " << actualLines[i] << std::endl;
            }
        } else if (i < actualLines.size()) {
            std::cout << GREEN << "+ " << actualLines[i] << RESET << std::endl;
        } else {
            std::cout << RED << "- " << expectedLines[i] << RESET << std::endl;
        }
    }
}

// Create an expected file from the current output of the interpreter
bool create_expected_file(const fs::path& expectedPath, const fs::path& outputPath, bool verbose) {
    try {
        if (verbose) {
            std::cout << "Creating expected file: " << expectedPath << std::endl;
        }

        std::string output = read_file(outputPath);
        std::ofstream expected(expectedPath);

        if (!expected.is_open()) {
            std::cerr << "Error: Could not create expected file: " << expectedPath << std::endl;
            return false;
        }

        expected << output;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating expected file: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    // Process command line arguments
    bool verbose = false;
    bool showDiff = true;
    bool createExpected = false;
    std::string testDir = "tests";
    std::string interpreterPath = "./chronovyan_interpreter";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--no-diff") {
            showDiff = false;
        } else if (arg == "--create-expected") {
            createExpected = true;
        } else if (arg == "--test-dir" && i + 1 < argc) {
            testDir = argv[++i];
        } else if (arg == "--interpreter" && i + 1 < argc) {
            interpreterPath = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --verbose, -v        Show detailed test information" << std::endl;
            std::cout << "  --no-diff            Don't show diff on test failure" << std::endl;
            std::cout
                << "  --create-expected    Create expected files for tests that don't have them"
                << std::endl;
            std::cout << "  --test-dir DIR       Specify test directory (default: tests)"
                      << std::endl;
            std::cout << "  --interpreter PATH   Specify interpreter path (default: "
                         "./chronovyan_interpreter)"
                      << std::endl;
            std::cout << "  --help, -h           Show this help message" << std::endl;
            return 0;
        }
    }

    // Validate test directory
    if (!fs::exists(testDir) || !fs::is_directory(testDir)) {
        std::cerr << "Error: Test directory does not exist: " << testDir << std::endl;
        return 1;
    }

    // Validate interpreter path
    if (!fs::exists(interpreterPath)) {
        std::cerr << "Error: Interpreter not found: " << interpreterPath << std::endl;
        return 1;
    }

    // Start test runner
    std::cout << BOLD << "Chronovyan Test Runner" << RESET << std::endl;
    std::cout << "Test directory: " << testDir << std::endl;
    std::cout << "Interpreter: " << interpreterPath << std::endl;
    std::cout << std::endl;

    int passed = 0;
    int failed = 0;
    int total = 0;
    int created = 0;

    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();

    // Find and run all .cvy test files
    std::vector<fs::path> testFiles;
    for (const auto& entry : fs::recursive_directory_iterator(testDir)) {
        if (entry.path().extension() == ".cvy") {
            testFiles.push_back(entry.path());
        }
    }

    // Sort test files by name for consistent output
    std::sort(testFiles.begin(), testFiles.end());

    // Display total test count
    std::cout << "Found " << testFiles.size() << " test files." << std::endl;

    // Run each test
    for (const auto& testPath : testFiles) {
        // Check if there's a matching .expected file
        fs::path expectedPath = testPath;
        expectedPath.replace_extension(".expected");

        bool hasExpectedFile = fs::exists(expectedPath);

        if (!hasExpectedFile && !createExpected) {
            if (verbose) {
                std::cout << YELLOW << "Skipping " << testPath << " (no .expected file)" << RESET
                          << std::endl;
            }
            continue;
        }

        total++;

        // Prepare output file path
        fs::path outputPath = testPath;
        outputPath.replace_extension(".out");

        // Run the interpreter
        std::string cmd = "\"" + interpreterPath + "\" \"" + testPath.string() + "\" > \"" +
                          outputPath.string() + "\" 2>&1";

        if (verbose) {
            std::cout << "Executing: " << cmd << std::endl;
        }

        int ret = std::system(cmd.c_str());
        if (ret != 0) {
            std::cout << RED << "[FAIL] " << testPath.filename().string()
                      << ": interpreter error (exit code " << ret << ")" << RESET << std::endl;
            failed++;
            continue;
        }

        // If we need to create the expected file, do it now
        if (!hasExpectedFile && createExpected) {
            if (create_expected_file(expectedPath, outputPath, verbose)) {
                std::cout << BLUE << "[NEW] " << testPath.filename().string()
                          << ": created expected file" << RESET << std::endl;
                created++;
                passed++;  // Count as passed since we're setting the expected output to match the
                           // actual
                continue;
            } else {
                std::cout << RED << "[FAIL] " << testPath.filename().string()
                          << ": could not create expected file" << RESET << std::endl;
                failed++;
                continue;
            }
        }

        // Read actual and expected output
        std::string actual = read_file(outputPath);
        std::string expected = read_file(expectedPath);

        // Compare outputs
        if (compare_outputs(actual, expected)) {
            std::cout << GREEN << "[PASS] " << testPath.filename().string() << RESET << std::endl;
            passed++;
        } else {
            std::cout << RED << "[FAIL] " << testPath.filename().string() << ": output mismatch"
                      << RESET << std::endl;

            if (showDiff) {
                show_diff(actual, expected);
            }

            failed++;
        }
    }

    // End timing
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Print summary
    std::cout << std::endl;
    std::cout << BOLD << "Test Summary:" << RESET << std::endl;
    std::cout << "Ran " << total << " tests in " << std::fixed << std::setprecision(2)
              << (duration.count() / 1000.0) << " seconds" << std::endl;
    std::cout << GREEN << "Passed: " << passed << RESET << " | " << RED << "Failed: " << failed
              << RESET;

    if (created > 0) {
        std::cout << " | " << BLUE << "Created: " << created << RESET;
    }
    std::cout << std::endl;

    // Calculate and display pass percentage
    if (total > 0) {
        double passRate = (static_cast<double>(passed) / total) * 100.0;
        std::cout << "Pass rate: " << std::fixed << std::setprecision(1) << passRate << "%"
                  << std::endl;
    }

    return (failed == 0) ? 0 : 1;
}