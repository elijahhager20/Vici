@echo off

set LOG="%~dp0batch-crash.log"

echo Vici: the one and only worst version control system known to man.
echo I salute you for even attempting to use this mashup of C++ header files I barely know how to use.
echo.

if not exist "viciw.exe" (
    echo [Error] viciw.exe not found. >> %LOG%
    echo [%DATE% %TIME%] ERROR: viciw.exe missing >> %LOG%
    echo Failed to launch Vici. See log: %LOG%
    exit /b 1
)

:: Launch
viciw.exe %*
if errorlevel 9009 (
    echo [%DATE% %TIME%] ERROR: Command or path not recognized >> %LOG%
    echo [Error] viciw.exe launch failed — unknown command or missing path.
    exit /b 1
)