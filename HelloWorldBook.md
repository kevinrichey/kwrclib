% Hello, World
% by Kevin Richey

# To Do

- Isolate test cases
- Test fixture setup & teardown
- Test case auto-discovery script
- Dynamic array
- Strings
- Linked list
- Hash table/associative array
- Auto release pool / memory tracker

# Coding Style

## C Naming Conventions

Universal Code
: Types, functions, etc that are commonly used across code.
: Stand-along and universal functions may use lower_snake_case().
: Universal functions are not part of a module and used frequently throughout the code.

Modules
: A collection of types and functions that work together. Usually (not always) in a single translation unit.
: Modules are named in UpperCamelCase and used as a namespace prefix in the collection of types and functions.
: Functions in a module are named as ModuleName_FunctionName().
: Enums in a module are named as ModuleName_EnumName. 
: Enum members are ModuleName_EnumName_MemberName.


- Variables, parameters, struct & union members:  lower_snake_case
- Structs & Unions:    UpperCamelCase
- Struct & Union Functions:   UpperCamelCase()
- Enum tag/typedef:   EnumName
- Enum members:       EnumName_ValueName
- Preprocessor symbols: UPPER_SNAKE_CASE
- Preprocessor macros:  MACRO()
- Global Constants:   MODULE_CONSTANT_NAME                                               


### Common Meanings & Abbreviations

- Init:    Initialize object to a known state.
- Make:    Create object, init from params, return by value.
- New:     Create object on heap, init, return pointer.
- Dispose: Destructor, dispose of object.
- Length:  number of elements in a sequence (array, string, list, etc).
- Size:    number of bytes in an object, as in sizeof().
- Cap:     max number of elements a sequence may contain.

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


## Preprocessor

- Symbols without value
- Constants with a single literal value
- Shortcut symbol evaluates into an expression
- Expression macro
- Statement macro

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

Typedef Structs:
: `typedef struct NAME { ... } NAME;`

Initialize empty struct objects:
: `struct_type var_name = { 0 };`

Prefer Designated Initializers for structs and arrays:
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

- [X Macros](https://www.drdobbs.com/the-new-c-x-macros/184401387)
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

## Instrumentation Types

- Unit Testing
- Tracing - Report code & debugging info to programmer.
- Debugging
- Assertions - detect and report logic errors to programmer.
- Errors - handle run-time errors, inform user and report to programmer.
- [Profiling]
- [Logging]

## Assertions

Catch and report logic errors

- Detect assertion failure (false condition)
- Report to console and/or log
- Abort handler
- Categories
  - Precondition (requires)
  - Postcondition (ensures)
  - Invariant
  - Assert
- Category on/off
- no level/throttle (always max level)

Interface

- requires(condition, [message], [values...])
- ensures(condition, [message], [values...])
- invariant(condition, [message], [values...])
- assert(condition, [message], [values...])

Requirements

- Calling code source-line, for preconditions
- Category handlers
- Function name


## Tracing

Report code & debugging info to programmer or admin. 

- Print to console, log to file
- No handler
- Volume throttle
- Category on/off
- Categories
    - Debug
    - Watch
    - Scope entry/exit
    - Operation begin/end

Interface

- trace(message, [params...])
- watch(variables, ...)
- enter_scope(scope_id)
- exit_scope(scope_id)
- begin_operation(op_id)
- end_operation(op_id)

## Errors

Detect errors, propogate up call stack, report to programmer, inform user.

- Detection (conditional checks)
- Cancel operation
- Return error code up call chain to handler
- Print or log error
- Inform user
- Categories
  - Error (cancel and return)
  - Failure (unable to complete operation)
  - Warning (inform user, option to proceed)
  - Fatal (terminate app)
  - Out of memory
  - Item not found
- Category on/off
- level throttle


## Instrumentation Categories

- Assertion
    - Precondition
    - Postcondition
    - Invariant

- Trace
    - Debug
    - Watch

- Profile
    - Timer
    - Sample
    - Count

- Debug
    - Breakpoint

- Audit

- Error
    - Fatal
    - Failure
    - Warning
    - Notice
    - Info

- Startup
- Shutdown
- Begin
- End
- Progress
- Input
- Output
- Check
- Memory
- Memory usage & tracking

## Volume Level

Each event is given a volume level, which should be based on the frequency of of the event.
The level is written to the trace/log. A throttle filter turns events on/off.

- 0 = always (can't turn off)
- max = never run


## Reporting

Format

- Time stamp
- Status/Category
- Volume level
- Source file
- Source line
- Function
- Message

Output Devices

- Console
- File
- UI
- Memory
- Network

## Handlers

A handler is a user-supplied function that is called in response to the event.
The general flow works like this:

- Ignore/do nothing
- Pause (stop and prompt user)
- Debug (stop at breakpoint)
- Terminate
- Set error state
- Return code
- Raise signal
- Throw [Exceptions] / (longjmp)
- User message

## Filters

- Volume level throttle
- High volume cache
- Type on/off
- Category on/off
- Individual event on/off

## Back Trace 

- Cache high volume traces in memory
- Output all pending traces 


## Profiling

Measure runtime performance. 

Format

- Clock time, high-res
- Source file
- Source line
- Category
- Level
- Scope (function, begin/end, entry/exit)

## Logging

- Path name of log file.
- Log rotation (time, size, run)
- Backup  (copy to another folder)
- Archive (zip and move)
- Cleanup (delete old logs by age, count)

## Instrumentation Components

Error status info
Error status format printer
Category handler registry


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

# Configuration

- Config File
- Command Line Arguments

# Text Output Formatting

Custom formatting escape codes

- Follow convention of `%spec` where 'spec' is a format specifier string.

Mechanism to map specifier names, struct members, and formatting procedures.
Static array of `struck_mem_ptr`, columns: specifier string, pointer offset to struct member, fn pointer to formatter.

    struct struct_mem_ptr;

    typedef void (*struct_mem_fn)(void* object, struct_mem_ptr* member);

    typedef struct struct_mem_ptr {
        const char* name;
        int8_t offset;
        struct_mem_fn func;
    } struct_mem_ptr;

Specifier      Type       Width   Purpose
-----------    ---------  ------  -----------------------
date           `time_t`   10      Date as YYYY-MM-DD
time           `time_t`   8       Time as HH:MM:SS
ticks          `clock_t`  10      System clock ticks
category       `char*`    10      Category name
lvl            `int`      3       Volume level
sfile          `char*`    *       Source filename
sline          `int`      6       Source line number
func           `char*`    *       Source function name as "function()"
msg            `char*`    *       Text message

# Data Structures

- Array
- Linked List
- Hash Table
- Associative Array
- Dynamically Sized Array
- Any/variant type
- Memory block manager?

## Associative Array

aka map, dictionary, hash table

Depends on

- Hash function (eg. Murmur3 32 bit)
- Linked list (for chaining)
- Dynamic Array (for hash table)


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

- Instrumentation

- Source Control
- Backups

- Process
    - SDLC
    - Requirements
    - Self-Organizing Teams
    - Progress Reporting

- Software Engineering
    - Architecture
    - Object-Oriented Programming
    - SOLID
    - Design Patterns
    - Test-Driven Development
    - Refactoring
    - Clean Code & Readability
    - You Aren't Gonna Need it (YAGNI)
    - Once and Only Once, Don't Repeat Yourself (DRY)
    - Interfaces
    - Technical Debt
    - Scripting 
    - Key Commands and Shortcuts
    - Documentation

- Tools, Compilers, Editors, Interpreters
    - Hardware, Keyboards, Screens

- Computer Science
    - CPU, memory, disk
    - Assembly
    - Memory models, stack & heap
    - Formal Data Structures
    - Algorithms
    - Compilers & Interpreters


# Terminology

Runtime errors
: Any error that occurs at runtime. 

Syntax errors
: An error in the program code that prevents the computer from executing the instructions. 
: This can occur at compile-time (for compiled languages) or runtime (for interpreted languages). 

