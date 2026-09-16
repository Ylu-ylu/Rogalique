@echo off
setlocal

set "SOLUTION_PATH=%~dp0Game.sln"
set "CONFIGURATION=Release"
set "PLATFORM=x64"
set "MSBUILD_PATH="

if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
)

if not defined MSBUILD_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
)

if not defined MSBUILD_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set "MSBUILD_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)

if not defined MSBUILD_PATH if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
        set "MSBUILD_PATH=%%i"
    )
)

if not defined MSBUILD_PATH (
    echo MSBuild.exe not found. Install Visual Studio with Desktop development with C++ workload.
    exit /b 1
)

echo Building solution: %SOLUTION_PATH%
echo Configuration: %CONFIGURATION%
echo Platform: %PLATFORM%
echo MSBuild: %MSBUILD_PATH%

"%MSBUILD_PATH%" "%SOLUTION_PATH%" ^
    /p:Configuration=%CONFIGURATION% ^
    /p:Platform=%PLATFORM% ^
    /t:Engine;XYZRoguelike ^
    /m

if errorlevel 1 (
    echo Build failed with errors.
    exit /b %ERRORLEVEL%
)

if exist "%~dp0SFML\SFML-2.5.1\bin\" (
    if not exist "%~dp0XYZRoguelike\x64\%CONFIGURATION%\" mkdir "%~dp0XYZRoguelike\x64\%CONFIGURATION%\"
    xcopy /Y "%~dp0SFML\SFML-2.5.1\bin\*.dll" "%~dp0XYZRoguelike\x64\%CONFIGURATION%\" >nul
    echo Copied SFML DLL files to output directory.
)

if exist "%~dp0XYZRoguelike\Resources\" (
    xcopy /E /I /Y "%~dp0XYZRoguelike\Resources\*" "%~dp0XYZRoguelike\x64\%CONFIGURATION%\Resources\" >nul
    echo Copied resource files to output directory.
)

echo Build completed successfully.
exit /b 0
