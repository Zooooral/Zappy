@echo off
setlocal enabledelayedexpansion

if "%1"=="clean" goto clean

set RAYLIB_INCLUDE="C:\Program Files (x86)\raylib\include"
set RAYLIB_LIB="C:\Program Files (x86)\raylib\lib"
set INCLUDE_PATHS=-Iinclude -Isrc -I%RAYLIB_INCLUDE%
set LIB_PATHS=-L%RAYLIB_LIB%
set LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32
set CPPFLAGS=-Wall -Wextra -std=c++17 -O2 -DNDEBUG

echo Building Zappy GUI...

if not exist build\gui mkdir build\gui 2>nul
if not exist build\gui\core mkdir build\gui\core 2>nul
if not exist build\gui\ui mkdir build\gui\ui 2>nul
if not exist build\gui\network mkdir build\gui\network 2>nul
if not exist build\gui\entities mkdir build\gui\entities 2>nul
if not exist build\gui\screens mkdir build\gui\screens 2>nul

for %%f in (src\gui\*.cpp) do g++ %CPPFLAGS% %INCLUDE_PATHS% -c "%%f" -o "build\gui\%%~nf.o" 2>nul
for %%f in (src\gui\core\*.cpp) do g++ %CPPFLAGS% %INCLUDE_PATHS% -c "%%f" -o "build\gui\core\%%~nf.o" 2>nul
for %%f in (src\gui\ui\*.cpp) do g++ %CPPFLAGS% %INCLUDE_PATHS% -c "%%f" -o "build\gui\ui\%%~nf.o" 2>nul
for %%f in (src\gui\network\*.cpp) do g++ %CPPFLAGS% %INCLUDE_PATHS% -c "%%f" -o "build\gui\network\%%~nf.o" 2>nul
for %%f in (src\gui\entities\*.cpp) do g++ %CPPFLAGS% %INCLUDE_PATHS% -c "%%f" -o "build\gui\entities\%%~nf.o" 2>nul
for %%f in (src\gui\screens\*.cpp) do g++ %CPPFLAGS% %INCLUDE_PATHS% -c "%%f" -o "build\gui\screens\%%~nf.o" 2>nul

g++ -o zappy_gui.exe build\gui\*.o build\gui\core\*.o build\gui\ui\*.o build\gui\network\*.o build\gui\entities\*.o build\gui\screens\*.o %LIB_PATHS% %LIBS% 2>nul

if exist zappy_gui.exe (
    echo Build complete: zappy_gui.exe
) else (
    echo Build failed
    exit /b 1
)
exit /b 0

:clean
echo Cleaning...
if exist "build" rd /s /q "build" 2>nul
if exist "zappy_gui.exe" del "zappy_gui.exe" 2>nul
echo Clean complete
exit /b 0