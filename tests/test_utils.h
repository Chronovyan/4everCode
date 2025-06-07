#pragma once

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace chronovyan::test {

// Test fixture with common utilities
class ChronovyanTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }

    void TearDown() override {
        // Cleanup code for each test
    }

    // Helper to read a test file
    static std::string readTestFile(const std::string& filename) {
        auto path = std::filesystem::path("tests") / "fixtures" / filename;
        std::ifstream file(path);
        if (!file) {
            throw std::runtime_error("Failed to open test file: " + path.string());
        }
        return std::string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    }
};

// Custom test macros
#define TEST_FILE(test_suite_name, test_name, filename) \
    TEST_F(test_suite_name, test_name) {                \
        std::string source = readTestFile(filename);    \
        /* Test implementation */                       \
    }

// Custom assertions
#define EXPECT_NO_THROW_MSG(statement, message)                            \
    try {                                                                  \
        statement;                                                         \
        SUCCEED();                                                         \
    } catch (const std::exception& e) {                                    \
        FAIL() << "Unexpected exception: " << e.what() << "\n" << message; \
    } catch (...) {                                                        \
        FAIL() << "Unknown exception thrown\n" << message;                 \
    }

}  // namespace chronovyan::test
