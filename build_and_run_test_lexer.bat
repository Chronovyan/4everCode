@echo off
setlocal

set BUILD_DIR=build
set TEST_DIR=test_parser
set TEST_FILE=test_lexer

:: Build the test
cmake -S . -B %BUILD_DIR%
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    exit /b %ERRORLEVEL%
)

cmake --build %BUILD_DIR% --target %TEST_FILE% --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed
    exit /b %ERRORLEVEL%
)

echo.
echo ===== Running %TEST_FILE% =====
echo.

:: Run the test with a test file
%BUILD_DIR%\Debug\%TEST_FILE%.exe %TEST_DIR%\string_test.cvy

endlocal
