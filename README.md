# Autoglue

Autoglue is a library that aims to provide an easy interface for generating language bindings from any language to any language.

## Usage

Until Autoglue is properly packaged, you have to build it yourself.
To build Autoglue on Linux:
```
git clone https://github.com/roopereku/autoglue.git
cmake -S autoglue -B build
cmake --build build
sudo cmake --install build
```

To include Autoglue in your project, check out the specific guides:

- [Clang](clang/README.md.md)

## File structure

- **autoglue** The common parts that can be used to build a simplified hierarchy.
- **clang** The clang backend used to generate a simplified hierarchy from C and C++.
- **java** The binding generator that outputs Java code. Later a backend can be added here aswell.
- **test** Contains things used for testing autoglue.
- **docs** Documentation for autoglue.