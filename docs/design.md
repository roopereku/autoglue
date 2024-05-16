# Autoglue Design

Autoglue is designed to be able to generate language bindings from any language to any language.

## Simplified hierarchy

Autoglue provides an abstraction known as the simplified hierarchy to describe an API that language bindings are generated for. A simplified hierarchy consists of entities, which are abstractions of common concepts that can be found in the majority of programming languages.

### Entities

Every entity inherits `ag::Entity` and has an entry in `ag::Entity::Type`. When any given entity type passes the corresponding type to the constructor of `ag::Entity`, the type of any given entity can be easily checked through `ag::Entity::getType()`.

Entities should override the following functions from `ag::Entity`:

- To give users easy access to the string representation of the entity type, `ag::Entity::getTypeString()` should be overridden.
- To let `ag::BindingGenerator` generate any given entity without having to check its type, `ag::Entity::generate(ag::BindingGenerator&)` should be overridden. This will be invoked upon an entity being generated and an entity can call desired functions from `ag::BindingGenerator` through the passed object among other code that may be needed.

## Generators

To generate language bindings for any given language, a generator can be defined to generate code specific to the given programming language.

Every generator inherits `ag::BindingGenerator` and can override the desired functions such as `generateClass` or `generateFunction`.

## Backends

Backends are used to construct a simplified hierarchy. They process whatever data that describes code written in any given language such as an AST and convert it to the corresponding entities.

For this purpose, a generator that only generates functions can be created.

## Bridge functions

In order for the language bindings to invoke functions from the API that the language bindings are generated for, bridge functions should be generated that are able to invoke the functions. These should be generated as C-compatible functions so that any given language can call them.