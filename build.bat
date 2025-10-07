@echo off
setlocal

set BUILD_DIR="build"
set BUILD_TYPE="Debug"

REM แก้ไขโดยการเพิ่ม "" ครอบตัวแปรและค่าที่เปรียบเทียบ
if "%CLEAN_BUILD%"=="true" (
  rmdir /s /q %BUILD_DIR%
  mkdir %BUILD_DIR%
)

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

pushd %BUILD_DIR%

REM รวมคำสั่ง conan install ให้อยู่ในบรรทัดเดียวกัน
conan install .. --output-folder=. --build=missing --settings=build_type=%BUILD_TYPE%

cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .

virtuniverse.exe

popd

endlocal