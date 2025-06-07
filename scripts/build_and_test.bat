@echo off
REM Script to build the Chronovyan project and run the enhanced REWIND_FLOW tests

echo === Building Chronovyan with Enhanced REWIND_FLOW Controls ===

REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

REM Configure with CMake
echo === Configuring with CMake ===
cmake .. -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed. Exiting.
    exit /b 1
)

REM Build the project
echo === Compiling the project ===
cmake --build . --config Debug
if %ERRORLEVEL% neq 0 (
    echo Compilation failed. Exiting.
    exit /b 1
)

REM Run the rewind flow tests
echo === Running Enhanced REWIND_FLOW Tests ===
.\bin\Debug\rewind_flow_test.exe

echo === All tests completed === 