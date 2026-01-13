@echo off
REM ============================================
REM  OpenGL Terrain Roaming System - Package Script
REM ============================================
echo Packaging OpenGL Terrain Roaming System...

REM Create output directory
set OUTPUT_DIR=..\RoamingSystem_Release
if exist "%OUTPUT_DIR%" rmdir /s /q "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%"

REM Copy executable
echo Copying executable...
copy "..\x64\Release\RoamingSystem.exe" "%OUTPUT_DIR%\"

REM Copy shaders
echo Copying shaders...
xcopy "..\shaders" "%OUTPUT_DIR%\shaders\" /E /I /Q

REM Copy assets
echo Copying assets...
xcopy "..\assets" "%OUTPUT_DIR%\assets\" /E /I /Q

REM Copy README
echo Copying documentation...
copy "..\README.txt" "%OUTPUT_DIR%\"

echo.
echo ============================================
echo Packaging complete!
echo Output directory: %OUTPUT_DIR%
echo ============================================
echo.
echo Contents:
dir /b "%OUTPUT_DIR%"
echo.
pause
