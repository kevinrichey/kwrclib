% Hello, World
% by Kevin Richey

# Coding Style

## C Naming Conventions

Module
: Data types, structures, functions that work together. 
: Modules are named in UpperCamelCase and used as a namespace prefix in the collection of types and functions.
: Functions in a module are named as ModuleName_FunctionName().

### Common Meanings & Abbreviations

- New:     Create object on heap, init, return pointer.
- Dispose: Destructor, dispose of object.

### Matching Verb Pairs

- init, dispose
- start, stop
- begin, end
- put, get
- set, unset
- add, remove
- create, destroy
- enable, disable
- insert, delete
- load, save
- push, pop
- first, last


## Formatting

- Indentation: 4 spaces (not tabs).
- Code blocks & braces
- Control statements (if, for, do, while, switch, etc)

## C Guildelines & Tips

Preprocessor Guidelines
: - Wrap statement macros in `do{ ... }while(0)`.
: - Wrap macro parameters in ()
: - Ensure variables declared within macros have unique names. Eg. use a prefix.
: - Use `_Bool` inside macros so user doesn't need to include stdbool.h.
: - [X Macros](https://www.drdobbs.com/the-new-c-x-macros/184401387)

Structs:
: `typedef struct NAME { ... } NAME;`

Initialize empty struct objects:
: `struct_type var_name = { 0 };`

Prefer designated initializers for structs and arrays:
: `struct_type var_name = { .m1 = x, .m2 = y };`
: If the struct members are rearranged, the designated initializer still works regardless of order.

Goto Guidelines
: - `goto` is not evil.
: - Use `goto` to exit early from compound statement blocks. Target label shall be in outer scope below the block.
: - Jump to clean-up code for early exit from a function. Label for the clean-up code should be named "finally:".
: - Avoid jumping to labels above the goto or in inaccesible scopes.
: - Prefer using `break` from loops.

Special Enum Members
: - First: equal to first enum in range (usually 0).
: - Last:  equal to last enum in range.
: - End:   equal to last + 1.
: - Count: number of enums, usually equal to End-First, can be used to allocate an array.

Use enum values to name designated array Initializers
:   `type_name  array_name[] = { [enum_a] = value, [enum_b] = value };`
: So initializers do not depend on position.

- [Duff's Device](http://www.catb.org/~esr/jargon/html/D/Duffs-device.html)
- [Overloading Functions in C](http://locklessinc.com/articles/overloading/)

# Program Instrumentation

Requirements

- Flexible - Configure different handling methods
- Decoupled
- Device-independent - output to screen/console, file, UI, network, etc.

Detection
Reporting
Handling

Extensibility

- custom reporting channels
- custom handers
- custom categories
- custom filters/throttles

# Resource Management 

acquire: Request the resource. This may fail if the resource is unavailable.
dispose: Return resource to the computer.
is_empty: Return true if the handle does not contain a resource. 
raw_data: Access the raw underlying resource. Requires !empty()
release: Nullify the handle and return the raw resource handle. Requires !empty()
reset: Dispose of the current resource (if exists) and assign new raw resource.  If parameter is null/empty, the object is left empty.
copy: Deep copy, replace resource with a duplicate of the parameter. If this resource !empty() then dispose it. This operation may not be applicable to all resources. 
swap: Exchange the resource with another resource handle.
move: Take the resource from another handle and leave that handle empty. Previous resource is disposed.

# Outline

- Data Structures
  - sequence: enumerated, ordered collection of objects.
  - array:  finite, fixed length sequence of data
  - string: finite sequence of characters
  - stream: infinite/unbounded sequence
  - list:   finite, dynamic sequence
  - vector
  - range
  - tuple

- Data structure operations
  - length: number of elements in a squence
  - size:   storage consumption (bytes) of sequence
  - capacity:  maxiumum length of sequence
  - first:     first element in sequence
  - last:      last element in sequence
  - end:       end of sequence after last element
  
- Files
  - Text Input & Parsing
  - Structured data
  - Tabular data
  - Markup

# Terminology

Runtime errors
: Any error that occurs at runtime. 

Syntax errors
: An error in the program code that prevents the computer from executing the instructions. 
: This can occur at compile-time (for compiled languages) or runtime (for interpreted languages). 

