# Autoglue Design

Autoglue is designed to be able to generate language bindings from any language to any language.

## Simplified hierarchy

Autoglue provides an abstraction known as the simplified hierarchy to describe an API that language bindings are generated for. A simplified hierarchy consists of entities, which are abstractions of common concepts that can be found in the majority of programming languages.

### Entities

Every entity inherits `ag::Entity` and has an entry in `ag::Entity::Type`. When any given entity type passes the corresponding type to the constructor of `ag::Entity`, the type of any given entity can be easily checked through `ag::Entity::getType()`.

Entities should override the following functions from `ag::Entity`:

- To give users easy access to the string representation of the entity type, `ag::Entity::getTypeString()` should be overridden.
- To let `ag::BindingGenerator` generate any given entity without having to check its type, `ag::Entity::onGenerate(ag::BindingGenerator&)` should be overridden. This will be invoked upon an entity being generated and an entity can call desired functions from `ag::BindingGenerator` through the passed object among other code that may be needed.

## Bridge functions

In order for the language bindings to invoke functions from the API that the language bindings are generated for, bridge functions should be generated that are able to invoke the functions. These should be generated as C-compatible functions so that any given language can call them.

## Generators

To generate language bindings for any given language, a generator can be defined to generate code specific to the given programming language.

Every generator inherits `ag::BindingGenerator` and can override the desired functions such as `generateClass` or `generateFunction`.

## Backends

Backends are used to construct a simplified hierarchy and define how bridge functions for calling the original functionality are generated. Any backend must inherit `ag::Backend` and overridde the following interface functions:
- `ag::Backend::generateHierarchy` which is used to generate the hierarchy from any suitable data, such as an AST. Everything related to an API should be exported to the hierarchy which allows for the user to call `ag::Entity::use` for any given entity that they want to generate.
- `ag::Backend::generateGlue` which is implicitly invoked right before any binding generator that is given a backend generates language bindings. During this step glue code should be generated which is required in order for any foreign language to properly use the generated language bindings.