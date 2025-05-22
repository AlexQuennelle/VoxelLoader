# Build Instructions:
1. Ensure CMake is installed
1. Ensure You have a valid C and C++ compiler installed
1. Make a file called build.bat that contains the following batch script:
```
@echo off
set buildType=%1
if "%~1" == "" set /p buildType="Build Type: "
echo Generating CMake files
for %%a in ("%~dp0\.") do set "parent=%%~nxa"
if not exist "Build" mkdir Build
if not exist "Bin" mkdir Bin
cd Build
cmake -D CMAKE_BUILD_TYPE=%buildType% ../ -G "Unix Makefiles"

echo Building Executable
make
cd ../Bin
start %parent%
pause
exit
```
4. Put that file in the root folder next to the CMakeLists.txt
5. Run build.bat
