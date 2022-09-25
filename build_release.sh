set echo off
mkdir -p -- "build/bin/Release/"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j