#!/bin/bash

# Build Autoglue.
cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DAUTOGLUE_BUILD_CLANG_BACKEND=ON -DAUTOGLUE_BUILD_JAVA_GENERATOR=ON
cmake --build ../build
cmake --install ../build --prefix ../build/prefix

# Build Autoglue test.
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build

# Run the test program.
echo "Generating autoglue bindings"
cd build
./autogluetest ../../build/compile_commands.json
cd ..
