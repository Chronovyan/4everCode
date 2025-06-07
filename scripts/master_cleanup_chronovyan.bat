@echo off
echo Chronovyan Project - Master Cleanup Suite
echo ========================================
echo.

echo This script will run various cleanup tools for the Chronovyan workspace.
echo.

:: Create scripts directory if it doesn't exist
if not exist ".\scripts" mkdir ".\scripts"

:: Check if PowerShell is available
where powershell >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo PowerShell is not available. Please install PowerShell to run the cleanup scripts.
    exit /b 1
)

set /p confirm="Would you like to proceed with a cleanup operation? (y/n): "
if /i NOT "%confirm%"=="y" goto :end_script

:menu
echo.
echo Chronovyan Cleanup Options:
echo 1. Cleanup Build Artifacts
echo 2. Cleanup Debug and Fix Files
echo 3. Identify Potential Dead Code / TODOs
echo 4. Review Documentation Organization (Reminders)
echo 5. Generate Cleanup Summary (Append to CHRONOLOG.md)
echo 6. Run Complete Cleanup (All of the above sequentially)
echo 7. Exit
echo.

set /p choice="Enter your choice (1-7): "

if "%choice%"=="1" (
    call :run_cleanup_artifacts
    goto menu
) else if "%choice%"=="2" (
    call :run_cleanup_debug_files
    goto menu
) else if "%choice%"=="3" (
    call :run_identify_dead_code
    goto menu
) else if "%choice%"=="4" (
    call :run_review_documentation
    goto menu
) else if "%choice%"=="5" (
    call :run_generate_summary
    goto menu
) else if "%choice%"=="6" (
    call :run_complete_cleanup
    goto menu
) else if "%choice%"=="7" (
    goto end_script
) else (
    echo Invalid choice. Please try again.
    goto menu
)

goto :end_script

:run_cleanup_artifacts
echo.
echo --- Running Build Artifact Cleanup ---
echo.
powershell -ExecutionPolicy Bypass -File ".\scripts\cleanup_artifacts_chronovyan.ps1"
if %ERRORLEVEL% neq 0 echo Error during artifact cleanup.
goto :eof

:run_cleanup_debug_files
echo.
echo --- Running Debug and Fix File Cleanup ---
echo.
powershell -ExecutionPolicy Bypass -File ".\scripts\cleanup_debug_files_chronovyan.ps1"
if %ERRORLEVEL% neq 0 echo Error during debug file cleanup.
goto :eof

:run_identify_dead_code
echo.
echo --- Running Potential Dead Code / TODO Identification ---
echo.
powershell -ExecutionPolicy Bypass -File ".\scripts\identify_potential_dead_code_chronovyan.ps1"
if %ERRORLEVEL% neq 0 echo Error during dead code identification.
goto :eof

:run_review_documentation
echo.
echo --- Running Documentation Organization Review ---
echo.
powershell -ExecutionPolicy Bypass -File ".\scripts\review_documentation_chronovyan.ps1"
if %ERRORLEVEL% neq 0 echo Error during documentation review.
goto :eof

:run_generate_summary
echo.
echo --- Generating Cleanup Summary Report ---
echo.
powershell -ExecutionPolicy Bypass -File ".\scripts\generate_cleanup_summary_chronovyan.ps1"
if %ERRORLEVEL% neq 0 echo Error generating summary report.
goto :eof

:run_complete_cleanup
echo.
echo === Running Complete Chronovyan Cleanup Process ===
echo.

echo Step 1: Cleanup Build Artifacts
call :run_cleanup_artifacts
echo.

echo Step 2: Cleanup Debug and Fix Files
call :run_cleanup_debug_files
echo.

echo Step 3: Identify Potential Dead Code / TODOs
call :run_identify_dead_code
echo.

echo Step 4: Review Documentation Organization (Reminders)
call :run_review_documentation
echo.

echo Step 5: Generate Cleanup Summary (Append to CHRONOLOG.md)
call :run_generate_summary
echo.

echo.
echo Complete cleanup process finished!
echo Please review CHRONOLOG.md and any script output for details.
echo.
goto :eof

:end_script
echo.
if /i NOT "%confirm%"=="y" echo Cleanup process canceled or not started.
echo Exiting...
exit /b 0 