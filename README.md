# Autoglue

Autoglue is a library that aims to provide an easy interface for generating language bindings from any language to any language.

## File structure

- **autoglue** The common parts that can be used to build a simplified hierarchy.
- **clang** The clang backend used to generate a simplified hierarchy from C and C++.
- **java** The binding generator that outputs Java code. Later a backend can be added here aswell.
- **test** Contains things used for testing autoglue.
- **docs** Documentation for autoglue.