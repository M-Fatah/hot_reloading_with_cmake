set echo off

mkdir -p -- "./build/bin/Debug"
pushd "./build/bin/Debug"
rm -rf *.pdb
popd

cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug -j