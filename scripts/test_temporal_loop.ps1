# PowerShell script to test the TEMPORAL_LOOP implementation

Write-Host "Building the Chronovyan interpreter..."
cd build
cmake --build .
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}
cd ..

Write-Host "`nRunning the TEMPORAL_LOOP basic test..."
Write-Host "-------------------------------------"
.\build\chronovyan.exe .\test\temporal_loop_basic_test.cvy

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nSuccess! The TEMPORAL_LOOP test passed."
} else {
    Write-Error "Test failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
} 