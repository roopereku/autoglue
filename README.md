# Autoglue

Autoglue is a library that aims to provide an easy interface for generating language bindings from any language to any language.

## Building

To build Autoglue, execute the following while in the Autoglue root directory:
```
python3 build.py
```
Without any additional arguments only the Autoglue core will be built. Different subsystems can be built as well by passing the appropriate command line parameters following this pattern:
```
--subsystem              ->    Build backend and generator
--subsystem-backend      ->    Build backend
--subsystem-generator    ->    Build generator
```
Replace `subsystem` with the subsystem of your choice, such as `csharp` or `clang`.

To view all of the available options, execute the following:
```
python3 build.py -h
```

## Usage

Check out the subsystem specific guides:

- [Clang](clang/README.md)