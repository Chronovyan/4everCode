# Chronovyan Script Tests

This directory contains test scripts for the Chronovyan language interpreter. These tests are designed to validate the functionality of the Chronovyan interpreter and ensure that it correctly processes Chronovyan language constructs.

## Test Structure

Each test consists of two files:
- `.cvy` file - Contains the Chronovyan script to be executed
- `.expected` file - Contains the expected output from running the script

For example, the test for basic math operations consists of:
- `basic_math.cvy` - The script that performs various math operations
- `basic_math.expected` - The expected output when `basic_math.cvy` is executed

## Running Tests

The tests are automatically run as part of the CMake test suite using the `chronovyan_test_runner`. To run the tests manually:

```bash
# From the build directory
ctest -R ChronovyanScriptTests

# Or run the test runner directly
./tests/chronovyan_test_runner --test-dir <path-to-tests/scripts> --interpreter <path-to-chronovyan_interpreter>
```

## Available Tests

- **Hello World** (`hello_world.cvy`): A simple test that prints "Hello, Chronovyan!"
- **Basic Math** (`basic_math.cvy`): Tests basic mathematical operations including addition, subtraction, multiplication, division, and modulo
- **Variables and Control Flow** (`variables_and_control.cvy`): Tests variable assignment, conditionals, and looping constructs
- **Functions** (`functions.cvy`): Tests function definition and calling, including recursive functions
- **Temporal Features** (`temporal_features.cvy`): Tests Chronovyan-specific temporal features like temporal runtime, timestreams, and debt tracking

## Adding New Tests

To add a new test:

1. Create a `.cvy` file with your test script
2. Create a `.expected` file with the expected output
3. The test runner will automatically detect and run your new test

## Test Runner Options

The test runner supports several command-line options:

- `--verbose`: Display detailed information about each test
- `--no-diff`: Don't display diff output for failed tests
- `--test-dir <path>`: Specify the directory containing test files (default: "./tests/scripts")
- `--interpreter <path>`: Specify the path to the Chronovyan interpreter (default: "./chronovyan_interpreter")

## Writing Effective Tests

When creating new tests, consider:

1. Testing a single feature or a small group of related features in each test
2. Including both normal and edge cases
3. Writing clear, descriptive comments in your test scripts
4. Making expected outputs as specific as possible
5. Using meaningful filenames that describe what's being tested 