# Build Instructions:
1. Ensure CMake is installed
1. Ensure You have a valid C and C++ compiler installed
1. Make a file called build.bat that contains the following batch script:
```
@echo off
echo Generating CMake files
for %%a in ("%~dp0\.") do set "parent=%%~nxa"
if not exist "Build" mkdir Build
cd Build
cmake ../ -G "Unix Makefiles"

echo Building Executable
make
start %parent%
exit
```
4. Put that file in the root folder next to the CMakeLists.txt
5. Run build.bat
