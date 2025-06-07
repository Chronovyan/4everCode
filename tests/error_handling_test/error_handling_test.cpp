#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "error_handler.h"
#include "source_location.h"

using namespace chronovyan;

// Create a simple test source file
std::shared_ptr<SourceFile> createTestSourceFile() {
    std::string content =
        "let x = 10;\n"
        "let y = 20;\n"
        "if (x > y) {\n"
        "    print(\"x is greater\");\n"
        "} else {\n"
        "    print(\"y is greater\");\n"
        "}\n"
        "let z = x + y;\n";

    return std::make_shared<SourceFile>("test.chv", content);
}

// Test SourceLocation creation and methods
void testSourceLocation() {
    std::cout << "Testing SourceLocation..." << std::endl;

    auto sourceFile = createTestSourceFile();

    // Test basic location creation
    SourceLocation loc1(sourceFile, 3, 4, 26);  // Line 3, column 4
    assert(loc1.isValid());
    assert(loc1.line == 3);
    assert(loc1.column == 4);

    // Test toString
    std::string locStr = loc1.toString();
    std::cout << "Location string: " << locStr << std::endl;
    assert(locStr == "test.chv:3:4");

    // Test toDetailedString
    std::string detailedStr = loc1.toDetailedString(true);
    std::cout << "Detailed location:\n" << detailedStr << std::endl;

    // Test createRange
    SourceLocation loc2(sourceFile, 5, 1, 50);
    SourceLocation range = loc1.createRange(loc2);
    assert(range.isValid());
    assert(range.line == 3);
    assert(range.column == 4);
    assert(range.endLine == 5);
    assert(range.endColumn == 1);

    std::cout << "Range: " << range.toDetailedString(true) << std::endl;

    // Test isBefore and isAfter
    assert(loc1.isBefore(loc2));
    assert(loc2.isAfter(loc1));
    assert(!loc1.isAfter(loc2));
    assert(!loc2.isBefore(loc1));

    // Test operator== and operator!=
    SourceLocation loc3(sourceFile, 3, 4, 26);
    assert(loc1 == loc3);
    assert(loc1 != loc2);

    // Test getFilename
    assert(loc1.getFilename() == "test.chv");

    // Test mergeLocations
    SourceLocation merged = mergeLocations(loc1, loc2);
    assert(merged.isValid());
    assert(merged.line == 3);
    assert(merged.column == 4);
    assert(merged.endLine == 5);
    assert(merged.endColumn == 1);

    std::cout << "SourceLocation tests passed!" << std::endl;
}

// Test error handler functionality
void testErrorHandler() {
    std::cout << "\nTesting ErrorHandler..." << std::endl;

    // Get the error handler
    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    errorHandler.reset();

    // Set up the source file
    auto sourceFile = createTestSourceFile();

    // Test basic error reporting
    SourceLocation loc1(sourceFile, 1, 5, 5);
    int errorId = errorHandler.reportError(loc1, "Variable 'x' already defined");
    std::cout << "Reported error ID: " << errorId << std::endl;

    // Test warning reporting
    SourceLocation loc2(sourceFile, 8, 9, 80);
    int warningId = errorHandler.reportWarning(loc2, "Unused variable 'z'");
    std::cout << "Reported warning ID: " << warningId << std::endl;

    // Test paradox reporting
    SourceLocation loc3(sourceFile, 3, 1, 22);
    int paradoxId = errorHandler.reportParadox(
        loc3, "Timeline branch creates a stable loop that conflicts with parent timeline",
        ParadoxType::TIMELINE_CONFLICT, 7);
    std::cout << "Reported paradox ID: " << paradoxId << std::endl;

    // Test glitch reporting
    SourceLocation loc4(sourceFile, 4, 5, 35);
    int glitchId = errorHandler.reportGlitch(loc4, "Variable 'x' experiences temporal fluctuation",
                                             GlitchType::VARIABLE_INSTABILITY, 0.75);
    std::cout << "Reported glitch ID: " << glitchId << std::endl;

    // Test error count
    size_t errorCount = errorHandler.getErrorCount(false);
    size_t totalCount = errorHandler.getErrorCount(true);
    std::cout << "Error count: " << errorCount << " (excluding warnings)" << std::endl;
    std::cout << "Total count: " << totalCount << " (including warnings)" << std::endl;
    assert(errorCount == 1);
    assert(totalCount == 2);

    // Test paradox resolution
    bool resolved = errorHandler.resolveParadox(paradoxId);
    std::cout << "Paradox resolved: " << (resolved ? "Yes" : "No") << std::endl;
    assert(resolved);

    // Test glitch fixing
    bool fixed = errorHandler.fixGlitch(glitchId);
    std::cout << "Glitch fixed: " << (fixed ? "Yes" : "No") << std::endl;
    assert(fixed);

    // Test full report
    std::string report = errorHandler.getReport(true, true);
    std::cout << "\n===== FULL ERROR REPORT =====\n" << report << std::endl;

    std::cout << "ErrorHandler tests passed!" << std::endl;
}

// Test fatal error reporting (this should cause the program to terminate)
void testFatalError() {
    std::cout << "\nTesting Fatal Error..." << std::endl;

    auto sourceFile = createTestSourceFile();
    SourceLocation loc(sourceFile, 2, 10, 18);

    // This will throw an exception
    ErrorHandler::getInstance().reportFatalError(
        loc, "Critical system failure: Memory corruption detected");

    // This should never be reached
    assert(false);
}

// Test callback functionality
void testCallbacks() {
    std::cout << "\nTesting Callbacks..." << std::endl;

    ErrorHandler& errorHandler = ErrorHandler::getInstance();
    errorHandler.reset();

    // Set up callback counters
    int errorCallbackCount = 0;
    int paradoxCallbackCount = 0;

    // Register error callback
    int errorCallbackId =
        errorHandler.registerErrorCallback([&errorCallbackCount](const ChronovyanError& error) {
            std::cout << "Error callback triggered: " << error.toString() << std::endl;
            errorCallbackCount++;
            return true;
        });

    // Register paradox callback
    int paradoxCallbackId =
        errorHandler.registerParadoxCallback([&paradoxCallbackCount](const ParadoxEvent& paradox) {
            std::cout << "Paradox callback triggered: " << paradox.toString() << std::endl;
            paradoxCallbackCount++;
            return true;
        });

    // Generate errors and paradoxes
    auto sourceFile = createTestSourceFile();
    SourceLocation loc(sourceFile, 3, 4, 26);

    errorHandler.reportError(loc, "Test error for callback");
    errorHandler.reportParadox(loc, "Test paradox for callback", ParadoxType::STABILITY_BREACH, 5);

    // Check callback counts
    assert(errorCallbackCount == 1);
    assert(paradoxCallbackCount == 1);

    // Remove callbacks
    bool errorRemoved = errorHandler.removeErrorCallback(errorCallbackId);
    bool paradoxRemoved = errorHandler.removeParadoxCallback(paradoxCallbackId);

    assert(errorRemoved);
    assert(paradoxRemoved);

    // Generate more errors and paradoxes (should not trigger callbacks)
    errorHandler.reportError(loc, "Another test error");
    errorHandler.reportParadox(loc, "Another test paradox", ParadoxType::QUANTUM_CONTRADICTION, 3);

    // Check callback counts (should be unchanged)
    assert(errorCallbackCount == 1);
    assert(paradoxCallbackCount == 1);

    std::cout << "Callback tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Error Handling System Test ===\n" << std::endl;

    try {
        // Test SourceLocation functionality
        testSourceLocation();

        // Test ErrorHandler functionality
        testErrorHandler();

        // Test callback functionality
        testCallbacks();

        // Uncomment to test fatal error
        // testFatalError();

        std::cout << "\nAll tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}