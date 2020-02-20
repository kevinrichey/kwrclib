% Hello, World
% by Kevin Richey

# To Do

- Isolate test cases
- Test fixture setup & teardown
- Test case auto-discovery script

# Coding Style

## Scoping & Namespaces

- Organization
- Product
- Library
- Module
- Component
- Object

## C Naming Conventions

- Variables, parameters, struct/union members:  lower_snake_case
- Struct & Union tags/typedefs:    Module_StructName, Module_UnionName
- Functions:   Module_Action(), Struct_Action()
- Enum tag/typedef:   Module_EnumName
- Enum members:       Module_EnumName_ValueName
- Preprocessor symbols: Module_SYMBOL
- Preprocessor macros:  Module_MACRO()
- Global Constants:   MODULE_CONSTANT_NAME                                               


### Common Meanings

- init:    constructor, initialize object from params.
- dispose: destructor, dispose of object.
- make:    create object, init from params, return by value.
- new:     create object on heap & return pointer.
- length:  number of elements in a sequence (array, string, list, etc).
- size:    number of bytes in an object.
- cap:     max number of elements a sequence may contain.


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

Types of preprocessor things

- Symbols without value
- Constant symbol with a single literal value
- Short cut symbol evaluates into an expression
- Expression macro
- Statement macro

## Formatting

indentation
code blocks & braces
control statements (if, for, do, while, switch, etc)

## C Guildelines & Tips

Preprocessor Guidelines
: - Wrap statement macros in `do{ ... }while(0)`.
: - Wrap macro parameters in ()
: - Ensure variables declared within macros have unique names. Eg. use a prefix.
: - Use `_Bool` inside macros so user doesn't need to include stdbool.h.

Typedef Structs
: `typedef struct NAME { ... } name;`

Initialize empty struct objects
: `struct_type var_name = { 0 };`

Prefer Designated Initializers for structs
: `struct_type var_name = { .m1 = x, .m2 = y };`
: If the struct members are rearranged, the designated initializer still works regardless of order.

Goto Guidelines
: - `goto` is not evil.
: - Use `goto` to exit early from compound statement blocks. Target label shall be in outer scope below the block.
: - Jump to clean-up code for early exit from a function. Label for the clean-up code should be named "finally:".
: - Avoid jumping to labels above the goto or in inaccesible scopes.
: - Prefer using `break` from loops.

Special Enum Members
: - First: equal to first enum in range (0)
: - Last:  equal to last enum in range
: - End:   equal to last + 1

Use enum values to name designated array Initializers
:   `type_name  array_name[] = { [enum_a] = value, [enum_b] = value };`
: So initializers do not depend on position.

- [X Macros](https://www.drdobbs.com/the-new-c-x-macros/184401387)
- [Duff's Device](http://www.catb.org/~esr/jargon/html/D/Duffs-device.html)
- [Overloading Functions in C](http://locklessinc.com/articles/overloading/)

# Program Instrumentation

- Condition:  do trace if true
- Report:     print/log info
- Handle:     do something (ignore, return code, throw, abort, etc).

## Instrumentation Components

- Category enum & strings
- Category filter & function
- Category handler
- Volume Level
- Volume filter function
- Debug info macro
- Printer function
- Logger function
- Custom format & variable output
- Open log file
- Backtrace stack
- Breakpoint marker
- Error state variable
- Return codes
- longjmp exceptions

## Instrumentation Types

monitor, measure, audit, scan, recorder, 
Vital, pulse, breath, beat, heart, health, fitness
alert, alarm, diagnose

T
E
A
P
O
D

Monitoring & measuring performance, diagnosing errors, tracing.

- Code [Tracing]
- [Debugging]
- [Error Handling]
- [Assertions]
- [Profiling]
- [Unit Testing]
- Event Logging - print/log

## Instrumentation Categories

- Fatal
- Error
- Warning
- Notice
- Info
- Debug
- Watch
- Startup
- Shutdown
- Begin
- End
- Progress
- Timer
- Input
- Output
- Assert
- Precondition
- Postcondition
- Invariant
- Check
- Failure
- Memory
- Memory usage & tracking
- Sample
- Counter
- Profile
- Event logging
- Auditing

## Volume Level

Each event is given a volume level, which should be based on the frequency of of the event.
The level is written to the trace/log. A throttle filter turns events on/off.

- 0 = always (can't run it off)
- max = never run

Functions

- set/get volume level

## Reporting

- Print to stdout
- Print to stderr
- Print to log file

Details

- Date & time
- Type/Category
- Volume level
- Debug info (source file, line, function)
- Message (static const string)
- Other data

Message Format

    $file:$###: $category ($LL) message in $function, $other-data

Functions

- format event string
- print event
- log event

## Handling

A handler is a user-supplied function that is called in response to the event.
The general flow works like this:

in-line code (macro) checks the condition and calls master handler function.
Master handler calls user-supplied handler (or default if user hasn't).


- Ignore
- Print (to console)
- [Logging] to file
- Show (in user interface)
- Cache (in memory backtrace, for output later, requires a storage object)
- Pause (stop and prompt user)
- Debug (stop at breakpoint)
- Terminate
- Set error state
- Return error code
- Raise signal
- Throw [Exceptions] / (longjmp)

Functions

- Set handler
- Call master handler
- Call user handler

## Filters

- Volume level throttle
- High volume cache
- Type on/off
- Category on/off
- Individual event on/off

## Back Trace 

- Cache high volume traces in memory
- Output all pending traces 

## Logging

Log format

- Time/date stamp
- Cateogry
- Level
- Message
- Other Context (function, local vars)

File management options

- Path name of log file.
- Log rotation (time, size, run)
- Backup  (copy to another folder)
- Archive (zip and move)
- Cleanup (delete old logs by age, count)

## Tracing

- Handle:  cache, ignore
- Reporting: print, log, silent

Trace format

- Debug info
- Category
- Level
- Message
- Other Context (function, local vars)

## Debugging

- Handle: ignore, pause, debug

## Profiling

- Reporting: silent
- Handling:  ignore, cache

Format

- Source location
- Clock time (high-res)
- Category?
- Level?
- Context (function, begin/end, entry/exit)

## Assertions

Detect defects, programming bugs, logic errors, broken contracts.
A condition which must always be true. A false assertion is a logical program error or defect. 

- Condition: boolean expression which should always be true, false is failure.
- Reporting: print, log, silent
- Handling:  terminate, ignore, pause, debug

Format

- Debug Info
- Category
- Level
- Message or conditional expression
- Other data (optional, eg. function, variables)

## Error Handling

An error is an incorrect result or failure. 
Software errors are frequently broken down into different categories: logic errors, syntax errors, runtime errors.  
Runtime errors are the result of the programming being unable to complete its task due to unforeseen circumstances beyond its control.
An organized method of handling errors is fundamental to our programming approach.

- Flexible - Configure different handling methods
- Extensible - add new handling methods without modifying the library
- Decoupled

Error Handling Process

- Detection
- Collect [Error Info]
- Report to calling code
- Inform user, give options
- Trace/log to programmer

- Return Code
- Error State

# Unit Testing

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

