# About
STpointer is a library to automatically manage your memory, it automatically handles circular references and frees memory as soon as possible.
When using the STpointer library every object has exactly one special (automatically assigned) pointer which will trigger local garbage collection if deleted, the rest will not perform checks. Local garbage collection will not check unrelated parts of memory. 
This library manages memory 100% automatically.

# Usage
1. Download STpointer.h and add it to your project
2. add `#include "STpointer.h"`
3. create a class `class MyClass : public STnode`
4. add pointer fields to your class `STpointer<MyClass> pointer;`
5. set the pointer parent inside your class constructor (skip this if your field is outside of an object) `pointer.setParent(this);`
6. access your fields with `pointer->field`
7. you can set your pointer to null with `pointer.unset()`

# Efficency
Currently it's not *perfectly* optimized, feel free to make a PR to optimize the obvious spot (or some other place) where it's a bit slow (eg. the unloop method)

# How it works
https://en.wikipedia.org/wiki/Spanning_tree
should be obvious from there
