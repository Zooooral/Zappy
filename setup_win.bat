@echo off
setlocal enabledelayedexpansion

echo.
echo ================================================================
echo                    ZAPPY GUI WINDOWS SETUP
echo ================================================================
echo.

set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "MAGENTA=[95m"
set "CYAN=[96m"
set "WHITE=[97m"
set "NC=[0m"

net session >nul 2>&1
if %errorLevel% neq 0 (
    echo %RED%ERROR: This script requires administrator privileges%NC%
    echo %YELLOW%Please run as administrator and try again%NC%
    pause
    exit /b 1
)

set "PROJECT_DIR=%~dp0"
cd /d "%PROJECT_DIR%"

echo %CYAN%📁 Project Directory: %PROJECT_DIR%%NC%
echo.

echo %BLUE%🔍 Checking for required tools...%NC%

where git >nul 2>&1
if %errorLevel% neq 0 (
    echo %RED%❌ Git not found%NC%
    echo %YELLOW%Please install Git from https://git-scm.com/download/win%NC%
    echo %YELLOW%Then restart this script%NC%
    pause
    exit /b 1
) else (
    echo %GREEN%✅ Git found%NC%
)

where cmake >nul 2>&1
if %errorLevel% neq 0 (
    echo %YELLOW%⚠️  CMake not found. Installing via winget...%NC%
    winget install Kitware.CMake
    if %errorLevel% neq 0 (
        echo %RED%❌ Failed to install CMake via winget%NC%
        echo %YELLOW%Please install CMake manually from https://cmake.org/download/%NC%
        pause
        exit /b 1
    )
    echo %GREEN%✅ CMake installed%NC%
) else (
    echo %GREEN%✅ CMake found%NC%
)

echo %BLUE%🔍 Checking for Visual Studio Build Tools...%NC%

set "VS_FOUND=0"
set "VS_PATH="

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_FOUND=1"
)

if %VS_FOUND%==0 (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
        set "VS_FOUND=1"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
        set "VS_FOUND=1"
    )
)

if %VS_FOUND%==0 (
    echo %RED%❌ Visual Studio Build Tools not found%NC%
    echo %YELLOW%Installing Visual Studio Build Tools...%NC%
    
    echo %CYAN%Downloading Visual Studio Build Tools installer...%NC%
    powershell -Command "Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_buildtools.exe' -OutFile 'vs_buildtools.exe'"
    
    echo %CYAN%Installing Visual Studio Build Tools (this may take a while)...%NC%
    start /wait vs_buildtools.exe --quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows10SDK.19041
    
    del vs_buildtools.exe
    
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
        set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
        set "VS_FOUND=1"
    )
    
    if %VS_FOUND%==0 (
        echo %RED%❌ Failed to install Visual Studio Build Tools%NC%
        echo %YELLOW%Please install manually from https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022%NC%
        pause
        exit /b 1
    )
)

echo %GREEN%✅ Visual Studio Build Tools found at: %VS_PATH%%NC%

echo.
echo %BLUE%📦 Setting up vcpkg package manager...%NC%

set "VCPKG_DIR=%PROJECT_DIR%vcpkg"

if not exist "%VCPKG_DIR%" (
    echo %CYAN%Cloning vcpkg...%NC%
    git clone https://github.com/Microsoft/vcpkg.git "%VCPKG_DIR%"
    if %errorLevel% neq 0 (
        echo %RED%❌ Failed to clone vcpkg%NC%
        pause
        exit /b 1
    )
)

cd /d "%VCPKG_DIR%"

if not exist "vcpkg.exe" (
    echo %CYAN%Bootstrapping vcpkg...%NC%
    call bootstrap-vcpkg.bat
    if %errorLevel% neq 0 (
        echo %RED%❌ Failed to bootstrap vcpkg%NC%
        pause
        exit /b 1
    )
)

echo %CYAN%Integrating vcpkg with Visual Studio...%NC%
vcpkg integrate install

echo %CYAN%Installing raylib...%NC%
vcpkg install raylib:x64-windows
if %errorLevel% neq 0 (
    echo %RED%❌ Failed to install raylib%NC%
    pause
    exit /b 1
)

echo %GREEN%✅ vcpkg and raylib installed successfully%NC%

cd /d "%PROJECT_DIR%"

echo.
echo %BLUE%📝 Creating CMakeLists.txt...%NC%

(
echo cmake_minimum_required^(VERSION 3.16^)
echo project^(zappy_gui^)
echo.
echo set^(CMAKE_CXX_STANDARD 17^)
echo set^(CMAKE_CXX_STANDARD_REQUIRED ON^)
echo.
echo # Find raylib
echo find_package^(raylib REQUIRED^)
echo.
echo # Include directories
echo include_directories^(${CMAKE_CURRENT_SOURCE_DIR}/src/gui^)
echo include_directories^(${CMAKE_CURRENT_SOURCE_DIR}/include^)
echo.
echo # Source files
echo set^(GUI_SOURCES
echo     # Main
echo     src/gui/main.cpp
echo.
echo     # Core
echo     src/gui/core/CameraController.cpp
echo     src/gui/core/ChatSystem.cpp
echo     src/gui/core/ConfigManager.cpp
echo     src/gui/core/DebugSystem.cpp
echo     src/gui/core/Environment.cpp
echo     src/gui/core/FontManager.cpp
echo     src/gui/core/GameStateManager.cpp
echo     src/gui/core/GameWorld.cpp
echo     src/gui/core/SoundManager.cpp
echo     src/gui/core/TileInteraction.cpp
echo.
echo     # Entities
echo     src/gui/entities/Character.cpp
echo     src/gui/entities/CharacterManager.cpp
echo     src/gui/entities/PropManager.cpp
echo.
echo     # Network
echo     src/gui/network/NetworkManager.cpp
echo     src/gui/network/NetworkPlatform.cpp
echo     src/gui/network/ProtocolHandler.cpp
echo.
echo     # Screens
echo     src/gui/screens/ConnectingScreen.cpp
echo     src/gui/screens/EndScreen.cpp
echo     src/gui/screens/GameScreen.cpp
echo     src/gui/screens/MainMenu.cpp
echo     src/gui/screens/SettingsMenu.cpp
echo     src/gui/screens/SplashScreen.cpp
echo.
echo     # UI
echo     src/gui/ui/Button.cpp
echo     src/gui/ui/InventoryUI.cpp
echo     src/gui/ui/KeyBindButton.cpp
echo     src/gui/ui/Slider.cpp
echo.
echo     # Utils
echo     src/gui/utils/ErrorHandler.cpp
echo ^)
echo.
echo # Create executable
echo add_executable^(zappy_gui ${GUI_SOURCES}^)
echo.
echo # Link libraries
echo target_link_libraries^(zappy_gui raylib^)
echo.
echo # Platform-specific linking
echo if^(WIN32^)
echo     target_link_libraries^(zappy_gui ws2_32^)
echo else^(^)
echo     target_link_libraries^(zappy_gui pthread^)
echo endif^(^)
echo.
echo # Compiler-specific options
echo if^(MSVC^)
echo     target_compile_options^(zappy_gui PRIVATE /W4 /std:c++17^)
echo     # Disable specific warnings for Windows
echo     target_compile_definitions^(zappy_gui PRIVATE _CRT_SECURE_NO_WARNINGS^)
echo else^(^)
echo     target_compile_options^(zappy_gui PRIVATE -Wall -Wextra -Wpedantic -std=c++17^)
echo endif^(^)
echo.
echo # Copy assets to build directory
echo add_custom_command^(TARGET zappy_gui POST_BUILD
echo     COMMAND ${CMAKE_COMMAND} -E copy_directory
echo     ${CMAKE_SOURCE_DIR}/assets $^<TARGET_FILE_DIR:zappy_gui^>/assets
echo     COMMENT "Copying assets to build directory"^)
echo.
echo # Copy config file
echo add_custom_command^(TARGET zappy_gui POST_BUILD
echo     COMMAND ${CMAKE_COMMAND} -E copy_if_different
echo     ${CMAKE_SOURCE_DIR}/config.ini $^<TARGET_FILE_DIR:zappy_gui^>/config.ini
echo     COMMENT "Copying config file"^)
echo.
echo # Set working directory for debugging ^(Visual Studio^)
echo if^(MSVC^)
echo     set_target_properties^(zappy_gui PROPERTIES
echo         VS_DEBUGGER_WORKING_DIRECTORY "$^<TARGET_FILE_DIR:zappy_gui^>"^)
echo endif^(^)
) > CMakeLists.txt

echo %GREEN%✅ CMakeLists.txt created%NC%

echo.
echo %BLUE%🏗️  Setting up build environment...%NC%

if exist "build" (
    echo %YELLOW%Cleaning existing build directory...%NC%
    rmdir /s /q build
)

mkdir build
cd build

echo %CYAN%Initializing Visual Studio environment...%NC%
call "%VS_PATH%"

echo %CYAN%Generating build files...%NC%
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" -G "Visual Studio 17 2022" -A x64
if %errorLevel% neq 0 (
    echo %RED%❌ Failed to generate build files%NC%
    echo %YELLOW%Trying with Visual Studio 16 2019...%NC%
    cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" -G "Visual Studio 16 2019" -A x64
    if %errorLevel% neq 0 (
        echo %RED%❌ Failed to generate build files%NC%
        pause
        exit /b 1
    )
)

echo %GREEN%✅ Build files generated%NC%

echo.
echo %BLUE%🔨 Building Zappy GUI...%NC%
echo %CYAN%This may take a few minutes...%NC%

cmake --build . --config Release
if %errorLevel% neq 0 (
    echo %RED%❌ Build failed%NC%
    echo %YELLOW%Check the output above for errors%NC%
    pause
    exit /b 1
)

echo %GREEN%✅ Build completed successfully!%NC%

echo.
echo %BLUE%📜 Creating run script...%NC%

cd Release
(
echo @echo off
echo echo Starting Zappy GUI...
echo echo.
echo echo Usage: run_gui.bat [options]
echo echo   -h ^<hostname^>  Server hostname ^(default: localhost^)
echo echo   -p ^<port^>      Server port ^(default: 4242^)
echo echo.
echo echo Example: run_gui.bat -h 192.168.1.100 -p 4242
echo echo.
echo.
echo if "%%1"=="" ^(
echo     echo Using default settings: localhost:4242
echo     zappy_gui.exe -h localhost -p 4242
echo ^) else ^(
echo     echo Using provided arguments: %%*
echo     zappy_gui.exe %%*
echo ^)
echo.
echo pause
) > run_gui.bat

echo %GREEN%✅ Run script created%NC%

echo.
echo %GREEN%================================================================%NC%
echo %GREEN%                    🎉 SETUP COMPLETE! 🎉%NC%
echo %GREEN%================================================================%NC%
echo.
echo %CYAN%The Zappy GUI has been successfully built!%NC%
echo.
echo %YELLOW%📍 Build location:%NC% %PROJECT_DIR%build\Release\
echo %YELLOW%📍 Executable:%NC% zappy_gui.exe
echo %YELLOW%📍 Run script:%NC% run_gui.bat
echo.
echo %BLUE%🚀 To start the GUI:%NC%
echo   1. Make sure the Zappy server is running
echo   2. Navigate to: %PROJECT_DIR%build\Release\
echo   3. Run: run_gui.bat
echo   4. Or run directly: zappy_gui.exe -h localhost -p 4242
echo.
echo %BLUE%🎮 Controls:%NC%
echo   - Use mouse to navigate menus
echo   - WASD or arrow keys to move camera in game
echo   - Mouse wheel to zoom
echo   - Click on characters to select them
echo   - Press ESC to return to menu
echo.
echo %MAGENTA%💡 Tips:%NC%
echo   - Assets are automatically copied to the build directory
echo   - Config file can be edited to change default settings
echo   - Log files are created in the logs directory
echo.
echo %GREEN%Happy gaming! 🎮%NC%
echo.
pause

start explorer "%PROJECT_DIR%build\Release"

endlocal