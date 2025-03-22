# Ash Programming Language Manual

## Introduction

Ash aims to be a modern systems programming language design for performance, a fair share of
memory safety, and expressiveness. It tries to combine the low-level of languages like C and C++
with ergonomic features and syntax. Ash is part of the Ember project and compiled using the 
Sparkle backend.

Ash includes strong static typing with type inference, explicit memory ownership model, unambiguous syntax that is easy to read and 
parse, as well as being concise yet readable to human. It also tries to have first-class support for low-level programming.

## Language Basics

### Comments

```c
// single line comment

/*
 * multi-line comments
 */
```

### Variables and Types

Variables can be declared with the `var` or `const` keyword, followed 
by a name, type and the initializer.

```ts
var x: i32 = 42; /* explicit */
var y = 42; /* implicit typing with type inference */
const z: i32 = 7; /* constant value */
var arr: i32[] = { 1, 2, 3 }; /* an array of i32 */
```

#### Primitive Types

| Type                      | Description                                          |
| ------------------------- | ---------------------------------------------------- |
| `i8`, `i16`, `i32`, `i64` | Signed integers of 8, 16, 32, and 64 bits            |
| `u8`, `u16`, `u32`, `u64` | Unsigned integers of 8, 16, 32, and 64 bits          |
| `f32`, `f64`              | Floating point numbers of 32 and 64 bits             |
| `string`                  | Sequence of characters                               |
| `bool`                    | Boolean value (true or false)                        |
| `void`                    | Represents no value                                  |
| `null`                    | Is optional or does not exist                        |
| `T[]`                     | Array of elements of type T                          |
| `ref[T]`                  | Reference to type T                                  |
| `own[T]`                  | Owned pointer to type T                              |
| `share[T]`                | Shared pointer to type T                             |
| `pin[T]`                  | Read-only shared pointer to type T                   |

All types are NOT atomic (thread safe) unless explicitly state so.

### Memory Management

Ash uses a memory ownership model akin to C++'s smart pointers. However, users are able to use
raw pointers and raw references if truly-unsafe operations are needed to be done.

- `own[T]`: An owned pointer | `std::unique_ptr<T>` in C++
- `share[T]`: A non-atomic shared pointer | `std::shared_ptr<T>` in C++
- `ref[T]`: A non-owning reference; doesn't affect ownership
- `*T`: A raw pointer; this is not recommended to use unless absolute neccessary due to the syntax being ambiguous about ownership.
- `&T`: A raw reference and an alias of `ref[T]`. It is present in the language for consistency with the raw pointer.

Pointer types except `*T`, `ref[T]` (extended to `&T`) are depended on the language runtime.

### Control Flow

#### If statements

```c
if (expression)
{
    /* code */
}
else if (expression)
{
    /* code */
}
else
{
    /* code */
}
```

#### Loops

> [!NOTE]
>
> Loops are still in its designing phase, meaning a different form can be semantically different.

```typescript
/* this means i will start from 0 to 5 and increment 1 every time the block inside the scope finished exeucting */
for (var i in 0..5..1) /* ..1 is optional; 5..0..1 can be done as well, however it means start from 5 to 0, decrement by 1 */
{

}

/* ...more loop style to be added... */

/* while loop */
while (condition)
{
    /* code */
}
```

### Functions

Functions are declared using the `function`. Functions can have [modifiers]() e.g. `inline function` for inline hits. 

```ts
function add(a: i32, b: i32) -> i32
{
    return a + b;
}
```

### Objects

Objects are complex data. They can be declared as both types and data.

#### Structs

Structs are declared with the `struct` keyword. Structs cannot have any behavior and all members are accessible from different scopes.

```c
struct Point[T] /* declaration */
{
    var x: T = {};
    var y: T = {};
};


const p: Point[i32] = Point[i32](); /* as data */
```

#### Classes

Classes support methods and visibility modifiers. Inheritance and other object oriented features
are depedent on the Ash runtime.

```cs
class Vector[T]
{
    private var data: T[] = {};
    private var size: u32 = {};
    
    public operator() -> void /* this is the constructor  */
    {
        data = null;
        size = 0;
    }
    
    public function push(value: T) -> void 
    {
        /* implementation detail goes here */
    }
    
    public function get_size() -> u32 
    {
        return size;
    }
}
```

### Generics

Ash uses square brackets for templates to avoid ambiguities when reading or parsing. Generics are determined during compile-time.
It is independent of the language runtim

```ts
struct TreeNode[T] 
{
    var value: T = {};
    var left: share[TreeNode[T]] = {};
    var right: share[TreeNode[T]] = {};
}

function make_pair[T, U](first: T, second: U) -> Pair[T, U] 
{
    return Pair[T, U] {
        .first = first,
        .second = second
    };
}
```

#### Clause

Generics can have constraits using the `where` clause.

```ts
function to_array[U]() -> U[] 
    where is_convertible[T, U]() /* && is_... */
{
    /* implementation detail */
}
```

Currently, users do not have any way to declare their own clauses.

### Type Casting

Type casting exists to provide a way to be expressive when converting type from one to another.
There are many type casting methods.

| Method                  | Description                                                                                                                           |
| ----------------------- | ------------------------------------------------------------------------------------------------------------------------------------- |
| cast[T]                 | Determined by the compiler                                                                                                            |
| truncate_cast[T]        | Indicates that the cast will reduce the precision. This can be done only on numberic types.                                           |
| reinterpret_cast[T]     | Turns a type to another without any conversion rule. This cast should be used carefully as it can change alignment of data e.g. SIMD. |
| static_cast[T]          | Explicitly converts a value from one data type to another.                                                                            |


When casting, it is important to remember the conversion rule. Ash's conversion rule is the same as C & C++'s.

### Modules & Imports

Imports symbols from different files with `import`

```ts
import 'stdlib';  /* import the standard library module */
import 'path/to/module';  /* import a custom module */
```

### Error Handling

When you know a function can error, an optional should be returned.

```ts
function divide(a: i32, b: i32) -> i32?
{
    if (b == 0) 
        return null;
    return a / b;
}

var result = divide(10, 2);
if (result)
    return std::println(result);

std::error("division by error");
```

If you need concise error handling then you can declare an enum and return them based on 
error instead of using optionals.

### Standard Library

Ash includes a standard library with core functionality.

- `io`: Input, output operations
- `memory`: Memory management utilities
- `containers`: Data structures
- `algorithm`: Algorithms such as binary search, quick sort etc
- `string`: String manipulation functions
- `math`: Methematical functions
- `sys`: System-level/OS operations
- `concurrent`: Concurrent operation support

Please note that **all** standard library that are OS-dependent are depended on the runtime. 
See [here]() for the specifications of each std module.

