@ECHO OFF

SETLOCAL

SET DEBUG_BIN_DIR=%~dp0build\bin\Debug\
IF NOT EXIST %DEBUG_BIN_DIR% MKDIR %DEBUG_BIN_DIR%
PUSHD %DEBUG_BIN_DIR%
DEL *.pdb >nul 2>&1
POPD

cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug -j

ENDLOCAL