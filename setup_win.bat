@echo off
setlocal enabledelayedexpansion

echo.
echo ================================================================
echo                    ZAPPY GUI WINDOWS SETUP (FIXED)
echo ================================================================
echo.

set "PROJECT_DIR=%~dp0"
cd /d "%PROJECT_DIR%"

echo Project Directory: %PROJECT_DIR%
echo.

echo [1/5] Checking for Git...
where git >nul 2>&1
if %errorLevel% neq 0 (
    echo ❌ Git not found
    echo Please install Git from https://git-scm.com/download/win
    pause
    exit /b 1
) else (
    echo ✅ Git found
)

echo [2/5] Checking for CMake...
where cmake >nul 2>&1
if %errorLevel% neq 0 (
    echo Searching for CMake in common locations...
    
    if exist "C:\Program Files\CMake\bin\cmake.exe" (
        echo Found CMake, adding to PATH...
        set "PATH=C:\Program Files\CMake\bin;%PATH%"
        goto cmake_found
    )
    
    if exist "C:\Program Files (x86)\CMake\bin\cmake.exe" (
        echo Found CMake, adding to PATH...
        set "PATH=C:\Program Files (x86)\CMake\bin;%PATH%"
        goto cmake_found
    )
    
    echo ❌ CMake not found
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
    
    :cmake_found
    echo ✅ CMake found and added to PATH
) else (
    echo ✅ CMake found
)

echo [3/5] Checking for Visual Studio Build Tools...

set "VS_FOUND=0"
set "VS_PATH="

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
    goto vs_found
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
    goto vs_found
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
    goto vs_found
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
    goto vs_found
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
    goto vs_found
)

echo [4/5] Setting up vcpkg...

set "VCPKG_DIR=%PROJECT_DIR%vcpkg"

if not exist "%VCPKG_DIR%" (
    echo Cloning vcpkg...
    git clone https://github.com/Microsoft/vcpkg.git "%VCPKG_DIR%"
    if %errorLevel% neq 0 (
        echo ❌ Failed to clone vcpkg
        pause
        exit /b 1
    )
)

cd /d "%VCPKG_DIR%"

if not exist "vcpkg.exe" (
    echo Bootstrapping vcpkg...
    call bootstrap-vcpkg.bat
    if %errorLevel% neq 0 (
        echo ❌ Failed to bootstrap vcpkg
        pause
        exit /b 1
    )
)

echo Integrating vcpkg...
vcpkg integrate install

echo Installing raylib...
vcpkg install raylib:x64-windows
if %errorLevel% neq 0 (
    echo ❌ Failed to install raylib
    pause
    exit /b 1
)

echo ✅ vcpkg and raylib installed

cd /d "%PROJECT_DIR%"

echo [5/5] Creating build files...

(
echo cmake_minimum_required^(VERSION 3.16^)
echo project^(zappy_gui^)
echo set^(CMAKE_CXX_STANDARD 17^)
echo find_package^(raylib REQUIRED^)
echo.
echo # Collect source files
echo file^(GLOB_RECURSE GUI_SOURCES "src/gui/*.cpp"^)
echo list^(FILTER GUI_SOURCES EXCLUDE REGEX ".*test.*"^)
echo.
echo # Include directories
echo include_directories^(src/gui^)
echo include_directories^(include^)
echo.
echo add_executable^(zappy_gui ${GUI_SOURCES}^)
echo target_link_libraries^(zappy_gui raylib ws2_32^)
echo.
echo # Copy assets
echo add_custom_command^(TARGET zappy_gui POST_BUILD
echo     COMMAND ${CMAKE_COMMAND} -E copy_directory
echo     ${CMAKE_SOURCE_DIR}/assets $^<TARGET_FILE_DIR:zappy_gui^>/assets^)
) > CMakeLists.txt

if exist "build" rmdir /s /q build
mkdir build
cd build

echo Initializing Visual Studio environment...
call "%VS_PATH%"

echo Generating build files...
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"
if %errorLevel% neq 0 (
    echo ❌ CMake generation failed
    pause
    exit /b 1
)

echo Building project...
cmake --build . --config Release
if %errorLevel% neq 0 (
    echo ❌ Build failed
    pause
    exit /b 1
)

cd Release
(
echo @echo off
echo echo Starting Zappy GUI...
echo zappy_gui.exe -h localhost -p 4242
echo pause
) > run.bat

echo.
echo ================================================================
echo                        ✅ SUCCESS!
echo ================================================================
echo.
echo GUI built successfully!
echo Location: %PROJECT_DIR%build\Release\
echo.
echo To run: cd build\Release && run.bat
echo.

start explorer "%PROJECT_DIR%build\Release"
pause