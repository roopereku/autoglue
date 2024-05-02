#!/bin/bash

# Build Autoglue.
cmake -S .. -B build/autoglue -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DAUTOGLUE_BUILD_CLANG_BACKEND=ON -DAUTOGLUE_BUILD_JAVA_GENERATOR=ON
cmake --build build/autoglue
cmake --install build/autoglue/ --prefix build/autoglue/prefix

# Build Autoglue test.
cmake -S . -B build/test -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/test

# Run the test program.
echo "Generating autoglue bindings"
cd build/test
./autogluetest ../autoglue/compile_commands.json
cd ../..
