@echo off
if not exist "build/" (
    mkdir build
    cd build
    cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
    cd ..
)

cmake --build build --config Release
if %errorlevel% neq 0 (
    exit /b %errorlevel%
)

"build/bin/splashgame.exe"
