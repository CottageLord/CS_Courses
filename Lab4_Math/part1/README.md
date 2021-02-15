**============================================================================**

**This section is required reading (and supplemental videos) to help ramp up your Fundamentals with C++**

**============================================================================**

# Part 1 - Managing C++ Projects

There are a few C++ concepts I want to briefly discuss related to managing your C++ projects. As we have been introduced to C++ in the first few weeks, it is worth visiting these topics again.

## Task 1 - Managing and understanding your Build System

### .cpp and .hpp files

Remember that in C++ you have two types of files, .cpp files, and .hpp files.

1. .cpp files are your implementation. This is the actual code that gets compiled.
2. .hpp files are your interface, this serves as a 'blueprint' for your interface.
    - Occassionally you may also define other functions in a .hpp file.
    - Code that is included (e.g. `#include "TinyMath.hpp"`) is merely 'copy and pasted' into that file by the compilers preprocessor prior to actually compiling your code.

### Linkers

The linker is the tool in C++ that 'glues' together object code and library code into a final executable. It's purspose is to allow you to do a few things:

1. Compile code separately
    - Then you only have to compile files that you change (or otherwise you can compile many files in parallel if they have no dependencies)
    - This saves you time as opposed to sequentially compiling your files.
2. Share code
    - By making use of the same library of code in multiple projects.
    - i.e. Common components like a 'math' library could be shared.
3. Optimize code
    - Apply special Link-Time Optimizations to your code (code relocation for caching, dead code elimination, etc.)

Normally when you compile a single C++ file (e.g. `g++ -c main.cpp`) that will generate object code with an extension .o (i.e. a binary file that is not executable, but that you can 'glue' with other files). This step is usually hidden from us when we use `g++ main.cpp -o main`, but ideally we would want to compile several files as .o files and link them together. `g++ main.cpp -o main` hides this step.

#### Library File types and associated extensions based on platform

Depending on your platform you will see different 'extensions' on library code.

- Windows
    - Executable: .exe (PE format)
    - Shared Library that can be dynamically loaded at compile-time or run-time: (.dll extension)
    - Static library that can be loaded at compile-time (.lib or .dll.a extension)
- Mac
    - Executable: .app (Mach-O)
    - Shared Library that can be dynamically loaded: (.dylib extension)
    - Static library that can be loaded at compile-time (.a extension)
- Linux
    - Executable: No extension (ELF Format)
    - Shared Library that can be dynamically loaded: (.so extension)
    - Static library that can be loaded at compile-time (.a extension)

### Build systems and CMake

There exist several build systems to help manage our project and make life easier. For us we are using `build.py` which is probably the simplest script possible. `build.py` solves one problem for us in this course (primarily for the course staff), and that is how to handle compilation on multiple platforms.

Be aware that other build systems exist like `make` or other meta-build systems exist like `cmake` (which generates a `makefile` for your platform or otherwise a project file). For your final project, these may be useful to deploy.

## Task 2 - Managing your memory

There are really three types of memory that we are concerned with when programming--statically alocated memory, stack memory and heap memory.

1. [Static memory](https://en.wikipedia.org/wiki/Static_variable) - This is the memory that is 'built-into' your executable, object, or library files. In C++, you use the keyword [static](https://en.cppreference.com/w/cpp/keyword/static) to indicate that a variable will last the lifetime of a program. Static has a few different contexts for which it can be used (i.e. as a static varible, or things like static member functions which are shared amongst all instances of objects in a particular class).
2. [Stack allocated memory](https://www.learncpp.com/cpp-tutorial/the-stack-and-the-heap/) memory that is automatically managed for us, and it lives while a variable or object is in scope. Memory on the stack is automatically managed for us.
3. Heap allocated memory(dynamically allocated memory) is the memory that we dynamically manage during run-time by requesting new memory with [new](https://en.cppreference.com/w/cpp/keyword/new) and [delete](https://en.cppreference.com/w/cpp/keyword/delete) (and delete[] for arrays). This is memory we **must** manage ourselves, and be sure to allocte and free it at the correct time.

### Using smart pointers

With dynamic memory allocation, folks can get into 'trouble' because you are often using a 'raw' pointer and responsible for managing your memory. For example: `GameObject* myObject = new GameObject;` is how we would create a new object with dynamic memory, but remember we are responsible for managing that object during its lifetime (i.e. calling delete when that object is no longer needed).

Some common problems associated with raw pointers are:
1. If we forget to reclaim memory that causing a memory leak.
2. Dereferencing a null pointer (Null Pointer Exception)
3. Freeing memory twice (double free).
4. Having two pointers point to the same location and deleting one (aliased pointers)

Some of these errors can be mitigated by avoiding raw pointers and building an abstraction around them. This was commonly down prior to 2011 (and still done today for other cases) for building your own 'smart pointer' class that builds an abstraction around a raw pointer to manage its lifetime automatically. Luckily for us, it is sufficient to use the built-in smart pointers in Modern C++ in the <memory> library.

**Watch now** (Video) [SMART POINTERS in C++ std::unique_ptr, std::shared_ptr, std::weak_ptr](https://www.youtube.com/watch?v=UOB7-B2MfwA)

## Task 3 - Managing your Code (const, inline, and Templates)

Here are a few C++ concepts to refresh on.

### const

In C++ there are 3 cases of the keyword const.

1. Mark a variable as immutable.
    - e.g. `const int PI=3.1415;`
2. Mark an argument as immutable (i.e. this is usually important to indicate a value will not change passed by reference)
    - e.g. `void foo::member_func(const std::vector<int>& collection)`
        - The above case is passing 'collection' by reference to avoid a copy, and pass the actual reference into the function, and because it is marked 'const' anything in that original collection will not be modified.
3. Indicate that no member variables in a class will be mutated.
    - e.g. `void foo::GetSize() const { ... }`
        -  This means no member variables can be modified at all within this function.
    
**Watch now**(Video )[Const in C++](https://www.youtube.com/watch?v=4fJBrditnJU)
    
### inline

Occassionally in the math library today you are going to see the [inline](https://en.cppreference.com/w/cpp/keyword/inline) keyword. This is a *suggestion* to the compiler to copy and paste the code that is in the function directly into the call site (i.e. where the function was called) rather than making the actual function call. This is a way to optimize your code, by trading space (the amount of lines of code) for some time in making a function call. The overhead associated with making a function call involves copying the arguments and return address to a call stack, which the inline function tries to avoid.

**Watch now**(Video )[C++ inline keyword](https://www.youtube.com/watch?v=Yh1mJlip3hw)

### Templates

You have very likely already seen templates in various container data structres in the standard template library (e.g. `std::vector<int>`). Templates are C++ version of generics in a language like Java. It allows us to 'parameterize' functions and classes (or structs which remember are *almost* the same as classes) to operate on generic types. This helps us avoiding having to write many different versions of the same library.

#### Template example

```cpp
// compile: clang++ -std=c++11 Vector2DTemplate.cpp -o example
// run: ./example
#include <iostream>

// Small example of C++ templates
template <class T>
struct Vector2D{
    T x,y;

    Vector2D() = default;

    Vector2D(T _x, T _y):x(_x),y(_y){
    }

};

int main(){

    Vector2D<int> myVecInt(1,2);
    Vector2D<bool> myVecBool(true,false);

    return 0;
}
```

**Watch now**(Video )[Templates in C++](https://www.youtube.com/watch?v=I-hZkUa9mIs)

Templates can be quite powerful, and you will occasionally run into [template specialization, or partial template specialization](https://www.cprogramming.com/tutorial/template_specialization.html) or [template metaprogramming](https://en.wikipedia.org/wiki/Template_metaprogramming) to get the compiler to generate lots of code for you (which is essentially all a template is, a way to generate code by the compiler for you without you having to do so).

<!-- ### Answer the following:

1. In C++ what are two instances where you would use the keyword const?
2. What does 'inline' do? Why might a programmer use it?
3. Why do you think [TinyMath.hpp](./TinyMath.hpp) is implemented as a header only library? (Sometimes you will see this, and you may research [.inl files](https://stackoverflow.com/questions/1208028/significance-of-a-inl-file-in-c) in C++)

Answer here:

1. *answer to number 1*
2. *answer to number 2*
3. *answer to number 3*
-->

