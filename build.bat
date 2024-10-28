@echo off
setlocal

set BUILD_DIR="build"

if %CLEAN_BUILD%==true (
  rmdir /s /q %BUILD_DIR%
  mkdir %BUILD_DIR%
)

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

pushd %BUILD_DIR%

cmake .. -G "MinGW Makefiles"
cmake --build .

virtuniverse.exe

popd

endlocal
