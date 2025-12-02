# Glich QUICKSTART

## Types Overview

### Number
- Represents a 64-bit signed integer.
- All literal whole numbers (e.g., `42`, `-3`) are Numbers by default.
- Used for general-purpose integer math.
- When a **Field** is expected (such as a Range), a Number is automatically converted if possible; otherwise, an error is triggered.
- Range: −9,223,372,036,854,775,808 to 9,223,372,036,854,775,807.

**Example:**
```glich
let year = 2025;
let answer = 42;
```

### Field

- Integer type, less than 32 bits, with reserved values for special meanings.
- Used for array indices, range bounds, etc.
- To write a field literal, use the `f` suffix (e.g., `7f`).
- **Core special values:**
    - **Infinity:** Use the keyword `infinity` for positive infinity; prefix with `-` for negative infinity (`infinity`, `+infinity`, `-infinity`).
    - **Not-a-number (unknown):** `?`
- Both `+` and `-` can prefix `infinity`.
- For concepts such as “before a value,” write `-infinity..1950`.
- **Calendar extension:** If installed, defines the constants:
    - `past` as `-infinity` and `future` as `+infinity` for clearer temporal intervals (e.g., `past..1950`).

**Examples:**
```glich
let idx = 7f;            // Field value 7
let top = +infinity;     // Positive infinity field value
let bottom = -infinity;  // Negative infinity field value
let unknown = ?;         // Not-a-number (unknown) field value
// With hics calendar extension:
let long_ago = past;     // Same as -infinity
let far_future = future; // Same as +infinity
```

### Range
- Two **field** values marking a start and end, written as `x..y`.
- Numbers in a range are automatically converted to fields when possible.
- Ranges are always stored so that `start ≤ end`, regardless of the order given; so `7..4` is equivalent to `4..7`.
- A range with `start = end` represents a single field value.
- **Field**, **Range**, and **RList** can be promoted or demoted automatically as required by context.

**Example:**
```glich
let r = 2f..7f;          // Range from 2 to 7
let from_ints = 3..6;    // Numbers auto-convert to fields
let single = 8f..8f;     // Single value, equivalent to 8f field
let backwards = 7..4;    // Auto-stored as 4..7
write r;          // Output: 2..7
write single;     // Output: 8
write backwards;  // Output: 4..7
```

### RList
- A list of zero or more Ranges (intervals), separated using the `|` operator.
- May represent the empty set using the `empty` keyword.
- Used for set/interval operations (union, intersection, etc.), and can hold joined or disjoint ranges.
- Promotes and demotes with Range and Field as required.

**Example:**
```glich
let s = 1..3 | 7..9;      // RList: 1..3 and 7..9
let none = empty;         // The empty set
write s;                  // Output: 1..3 | 7..9
write none;               // Output: empty
```

### Float
- Double-precision (64-bit) floating point value.
- Used for fractional numbers and decimal precision.
- Special Float values:
    - **Infinity:** Use the keyword `inf` (`+inf`, `-inf`)
    - **Not-a-number:** `nan`
- Both `+` and `-` can prefix `inf` or `nan` (though sign for nan is rarely used).

**Examples:**
```glich
let pi = 3.14159;
let f1 = +inf;    // Positive infinity
let f2 = -inf;    // Negative infinity
let impossible = nan;  // Not-a-number float value
```

### String
- A **String** in glich can contain any sequence of UTF-8 characters, including literal newlines.
- Strings can contain any Unicode character.
- Literal strings are written between double quotes (`"`).
- To include a double-quote character inside a string, use two double quotes in a row (`""`).

**Example:**
```glich
write "He said ""Hi""";   // Output: He said "Hi"
write "First line
Second line";            // Output: First line
//         Second line
```
- There are no single-quoted strings; only double quotes are used for literals.

### Boolean
- Logical values: `true`, `false`.
**Example:**
```glich
let is_valid = true;
write is_valid;   // Output: true
```

### Object
- Specialized general-purpose structured type, always associated with a code (short identifier).
- Object **code** distinguishes object types and must be as short as practical.
- Must be explicitly defined before use.
- Each instance's fields and behaviors are specified in its definition.
- See [Objects & Definitions](#objects--definitions) for more.

### Blob
- Arbitrary binary data (Binary Large OBject), useful for file contents or buffers.

### Error
- Represents error states or exceptional conditions.
- Often returned by commands or via the `@error()` built-in function.

### Null
- Represents "no value" or absence of data.

```glich
let val = null;
if val = null write "No value"; endif
```
---

## Operators

Glich provides operators for manipulating numbers, fields, floats, booleans, strings, ranges, and RLists.

### Arithmetic Operators

| Operator / Keyword | Description         | Example         | Result      |
|--------------------|---------------------|-----------------|-------------|
| `+`                | Addition            | `2 + 3`         | `5`         |
| `-`                | Subtraction         | `5 - 1`         | `4`         |
| `*`                | Multiplication      | `4 * 2`         | `8`         |
| `/`                | Division (float result) | `5 / 2`     | `2.5`       |
| `div`              | Integer division    | `5 div 2`       | `2`         |
| `mod`              | Modulo/remainder    | `8 mod 3`       | `2`         |
| `-x`               | Negation            | `-7`            | `-7`        |

### Assignment Operators

| Operator  | Example         | Equivalent To      |
|-----------|-----------------|--------------------|
| `=`       | `n = 1`         | set value          |
| `+=`      | `n += 2`        | `n = n + 2`        |
| `-=`      | `n -= 1`        | `n = n - 1`        |
| `*=`      | `x *= 10`       | `x = x * 10`       |
| `/=`      | `x /= 2`        | `x = x / 2`        |

### Comparison Operators

| Operator | Description         | Example      | Result (Boolean) |
|----------|---------------------|--------------|------------------|
| `=`      | Equality            | `x = y`      | true if equal    |
| `<>`     | Not equal           | `x <> 0`     | true if not same |
| `<`      | Less than           | `a < b`      | true/false       |
| `<=`     | Less than or equal  | `a <= b`     | true/false       |
| `>`      | Greater than        | `a > b`      | true/false       |
| `>=`     | Greater or equal    | `a >= 0`     | true/false       |

### Logical Operators

Only for Boolean values (`true`, `false`). Using with any other type is an error.

| Operator | Description | Example            | Result         |
|----------|-------------|--------------------|----------------|
| `and`    | Logical AND | `a and b`          | true/false     |
| `or`     | Logical OR  | `x or y`           | true/false     |
| `not`    | Logical NOT | `not done`         | true/false     |

### String Operators

| Operator | Description         | Example                    | Result            |
|----------|---------------------|----------------------------|-------------------|
| `+`      | Concatenate strings | `"Hi " + "there"`          | `"Hi there"`      |
|          | (numbers/other types converted to string as needed) | `"Answer: " + 42` | `"Answer: 42"`    |

### Range and Set Operators

- Set operations always operate on RLists; fields and ranges are promoted automatically.

| Operator | Description                           | Example syntax            |
|----------|---------------------------------------|--------------------------|
| `..`     | Range between two fields (inclusive)  | `1..5`                   |
| `|`      | Union (OR) of ranges/RLists           | `1..3 &#124; 8..9`       |
| `!`      | Set complement                        | `!0`                     |
| `&&`     | Intersection (AND)                    | `1..10 && 5..15`         |
| `^^`     | Symmetric difference (XOR)            | `1..5 ^^ 4..8`           |
| `\\`     | Relative complement (A minus B)       | `1..10 \ 3..6`           |

**Typical results:**
```glich
let all_but_zero = !0;          // -infinity..-1 | 1..infinity
let both = 1..10 && 7..20;      // 7..10
let xor = 1..5 ^^ 4..8;         // 1..3 | 6..8
let minus = 2..9 \ 5..6;        // 2..4 | 7..9
let united = 1..3 | 8..9;       // 1..3 | 8..9
let combined = (2 | 3..5) | 7;  // 2..5 | 7
```
- For complex set results, see example output in code blocks above.

*For details and operator precedence, see the  
[Glich Expressions and Operators manual](https://nickmat.github.io/glich/website/dev/man/script/expr.htm).*

---

## Names

- A **name** (identifier) is used for variables, constants, object codes, fields, and more.
- Allowed characters: alphanumerics (`A-Z`, `a-z`, `0-9`), underscore (`_`), colon (`:`).
- Cannot start with a number.
  - Valid: `field1`, `iso_date`, `main:calendar`
  - Invalid: `3number`
- In certain contexts, a name may be used as a string value.

---

## Functions and Commands

### The `function` Statement

```
function identifier.qualifier(arg1 = default, ...) {
  // function body
}
```
- `identifier`: The function's primary name.
- `qualifier`: Optional dot-separated name (available as string `qualifier`).
- Arguments may have defaults; if omitted and no default is given, the value is `null`.
- Function body can only see: `qualifier`, arguments, global variables, and `result`.
- `result` is special: its value becomes the function’s result, and is initially `null`.

**Examples:**
```glich
function sum.x(a = 1, b = 2) {
  result = x + " with: " + (a + b);
}
write @sum.foo(5, 10);     // Outputs: foo with: 15
write @sum.foo(, 10);      // Outputs: foo with: 11
write @sum.foo;            // Outputs: foo with: 3
```
Here, `identifier` is `sum`, `qualifier` is `foo`, the arguments are `a` and `b`.

---

### The `command` Statement

- Syntax is identical to `function`.
- Used to define custom statement-like logic for use with the `call` statement.
- Error values returned by commands are specially handled; if the result is an `error`, the `call` fails.
- In practice, most logic can be handled by functions, and command is mainly for custom statement-like integration or special error flow.

**Example:**
```glich
command simon(says) { write "Simon Says: " + @quote(says) nl; }
call simon("Jump");
// Output: Simon Says: "Jump"
```

#### Purpose of `command`

- The `command` statement in glich is designed to allow for custom user-defined statements.
- Unlike functions (which are used as part of expressions), commands are invoked as standalone statements via the `call` keyword.
- In typical scripting, there may be little need for custom commands; most logic can be handled with functions.
- Commands are mainly useful when you want to define a new statement-like block with special error-handling or when integrating tightly with scripting flow.

---

### Function and Command Invocation

- **Function Calls:**  
  Always use the `@` operator **before the function name**.  
  `@sum`, `@double(7)`, etc.
  - Without `@`, the name refers to a variable, NOT a function.
  - Syntax is flexible: qualifiers, arguments, and parentheses may all be omitted if not needed.
  - If all arguments are omitted, brackets may be left out entirely (`@sum`).

- **Examples:**
```glich
function sum.x(a = 1, b = 2) {
  result = x + " with: " + (a + b);
}
@sum.foo(7, 8)             // x = "foo", a = 7, b = 8
@sum.x(5)                  // x = "x", a = 5, b = 2
@sum( , 10)                // x = "", a = 1, b = 10
@sum                       // Same as @sum(), x = "", a = 1, b = 2
```
- **If an argument is omitted and has no default, its value is `null`.**

- **Command Calls:**  
  Only called using the `call` statement (never with `@`):
```glich
command simon(says) { write "Simon Says: " + @quote(says) nl; }
call simon("Jump");    // Output: Simon Says: "Jump"
```

---

## Conditional Execution (`if` statement) and `end;`

You can control flow using `if`, `elseif`, and `else` blocks:

```
if condition
  statements
[elseif condition
  statements]
...
[else
  statements]
endif
```

- You may include any number of `elseif` blocks and at most one `else` block.
- The block following the first true condition is executed; the rest are skipped.
- `endif` ends the entire conditional block.
- Use `end;` to immediately exit from the current function, command, or script.

**Example with `if`/`elseif`/`else`:**

```glich
let x = ...;  // some number
if x < 0
  write "negative" nl;
elseif x = 0
  write "zero" nl;
else
  write "positive" nl;
endif
```

**Example in a function:**

```glich
function classify(x) {
  if x < 0
    result = "negative";
  elseif x = 0
    result = "zero";
  else
    result = "positive";
  endif
}
```
Usage:
```glich
write @classify(-3);  // Output: negative
write @classify(0);   // Output: zero
write @classify(8);   // Output: positive
```

You can also use `end;` to stop execution immediately when a condition is met:

```glich
if x < 0 or y < 0
  result = @error("Inputs must be non-negative.");
  end;
endif
```
---

## Built-in Functions

Glich provides several built-in functions available anywhere in your code using the `@` operator. These cover common tasks and shortcuts.

### Examples

- **@quote(value)**
  Ensures the output is always a legal double-quoted string literal, even if the value is numeric, null, or another type.
  - `@quote(123)` outputs `"123"`
  - `@quote("hello")` outputs `"hello"`
  - Nested and complex: `write @quote("Say " + @quote("No"));` outputs `"Say ""No"""`

- **@if(condition, is_true, is_false)**
  Returns `is_true` if `condition` is true, otherwise `is_false`.
  This is equivalent to:
  ```glich
  function if(cond, t, f) {
    if cond
      result = t;
    else
      result = f;
    endif
  }
  ```
  Example: `write @if(2 > 1, "yes", "no"); // Output: yes`

- **@type(value)**
  Returns the name of the value’s type as a string.
  Example: `write @type(1.0); // Output: float`

> Built-in functions always require the `@` operator and are used like ordinary user-defined functions in expressions.

**Full Reference:**
For a complete list of built-in functions, see:
[https://nickmat.github.io/glich/website/dev/man/script/bi-functions.htm](https://nickmat.github.io/glich/website/dev/man/script/bi-functions.htm)

---

## Objects & Definitions

- Define with the `object` statement using a code and fields:
```glich
object pair { values first second; }
```
- Create with curly braces:
```glich
let p = {pair "one", 1};
```
- Access fields with names or by index:
```glich
write p[first] " is " p[second];  // Output: one is 1
write p[0] " is " p[1];           // Output: one is 1
```
- Objects must be defined before use.
- **Object code** is a special short name identifying the object type.

### Built-in Generalized Array Object

- There is a built-in object with code `:` defined as `object : {};`
- Used as a generalized array; order is preserved and values may be of any type.
```glich
let array = {: 1, 1 + 1, "three"};
write array; // Output: {: 1, 2, "three"}
```

---

## Object Functions

- Objects can have functions as part of their definition.
- Define as part of the object block:
```glich
object pair {
  values first second;
  function sum { result = first + second; }
}
let p = {pair 3, 5};
write p@sum;        // Output: 8
```

**Key Points:**
- **Calling:** Call object functions with the `@` operator and function name: `objectInstance@functionName`
- **Context:** In the body, object fields are directly accessible by name.
- **Scope:** Only object fields, the function’s own parameters, and `result` are visible (not global variables).
- **Result:** Set the function’s result via assignment to the special `result` variable.
- **No Inheritance:** Only functions defined directly on the object may be called this way.
- **Parameter Defaults:** Defaults for parameters are supported as for regular functions.
- **No Qualifier:** Only `objectInstance@functionName` (no qualifier allowed).
- **Copy Semantics:** The functions operate on a **copy** of the object, so the original object is unchanged when the function runs.
- A variable **`this`** is available inside the function body, representing the local copy.
    - You can assign to `result = this;` to return the updated object (though there are some limitations; this behavior may need refinement in future versions).
    - You can reference or mutate unnamed fields with an index, e.g., `this[0] = 10;`.

**Example with parameters and defaults:**
```glich
object counter {
  values value;
  function add(x = 1) { result = value + x; }
}
let c = {counter 10};
write c@add(5);       // Output: 15
write c@add();        // Output: 11
```

**Example returning an updated object:**
```glich
object pair {
  values first second;
  function set_first(new_first) {
    this[0] = new_first;
    result = this;  // Return an updated copy (original remains unchanged)
  }
}
let p = {pair "a", "b"};
let q = p@set_first("z");
write p[first];   // Output: a
write q[first];   // Output: z
```

---

## Built-in Object Functions

Some functions are available to **all objects** in glich, regardless of their definition. These are called built-in object functions and are called with the `@` operator just like user-defined object functions.

**Example:**
```glich
write {: 10, 20, 30}@size;      // Output: 3
write {: 10, 20}@obj:name;      // Output: ":"
```

Common built-in object functions include:
- **@size** — returns the number of elements/fields in the object.
- **@obj:name** — returns the object code string.
- **@obj:list** — returns the values as a standard list/array.
- **@mask** — returns a new object with only selected fields.

For a complete list and details, see:
[https://nickmat.github.io/glich/website/dev/man/script/bi-objects.htm](https://nickmat.github.io/glich/website/dev/man/script/bi-objects.htm)

---

## Returning Error Values

- Use the built-in conversion function `@error("message")` to create/return errors.
- Example:
```glich
function must_be_positive(x) {
  if x <= 0
    result = @error("Value must be positive.");
    end;
  endif
  result = x;
}
```
- Returned error values propagate—for `command`, they'll cause the `call` to fail.

---

## Looping: The `do .. loop` Statement

Use the `do .. loop` construct to repeat a block of statements.

**Basic Syntax:**
```glich
do
  // repeated statements
loop
```

### Exiting a Loop

There are **three ways** to exit a loop:

- `while condition;` — Exits the loop if the condition is **false**. If the condition is true, looping continues. Typically placed at the start of the loop body.
- `until condition;` — Exits the loop if the condition is **true**. If the condition is false, looping continues. Usually placed at the end of the loop body.
- `end;` — Immediately exits the enclosing function, command, or script, so also ends the loop.

### Examples

**Example: Using `while` at the beginning:**
```glich
let n = 0;
do
  while n < 5;
  write n nl;
  n += 1;
loop
// Output: 0 1 2 3 4
```

**Example: Using `until` at the end:**
```glich
let n = 0;
do
  write n nl;
  n += 1;
  until n = 5;
loop
// Output: 0 1 2 3 4
```

**Example: Using `end;` to exit:**
```glich
let n = 0;
do
  write n nl;
  n += 1;
  if n = 5
    end;
  endif
loop
// Output: 0 1 2 3 4
```

- You can use assignment operators like `+=`, `-=`, etc., for counters and other variables in the loop.

---

## Error Handling

Glich has two distinct error handling methods:

### 1. Expression Errors

- Errors that occur in expressions (such as division by zero or a function returning an error value) "ripple" through the entire expression and become its result.
- These do **not** stop the script; execution continues.
- The error message indicates the line number of the statement that produced the error (such as `let` or assignment—not necessarily the output statement).

**Example:**
```glich
let x = (1/0) + 10;
write x;
// Output: Error (1): Division by zero.
```
Here, the error is reported on the `let x = ...` statement.

### 2. Statement Syntax Errors

- Errors that occur at the statement level (such as calling an unknown command).
- These errors are immediately reported to the error output stream (with a line number), and the script stops executing.
- Scripting ceases at the point of the error.

**Example:**
```glich
call unknown_command;
// Output: Error (1): Command "unknown_command" not found.
```

---

## The `mark` Statement

When functions, objects, and variables are loaded into memory by a script, they remain resident even after the script finishes.
If you run the script again without clearing these definitions, you may get errors about duplicate definitions, redefined objects, or similar issues.

The `mark` statement is used to **control script memory** and ensure a clean environment for script reruns.

**How it works:**

- Place `mark tag;` at the **very start** of your script, where `tag` is any name you choose.
- The first time the mark is seen, it is created and everything read after it is loaded as usual.
- On subsequent runs, when the script encounters the same `mark tag;`, it **removes** all objects, functions, and variables defined after the mark—giving a fresh start.

**Typical usage:**
```glich
mark myscript;
// Now define all objects, variables, functions, etc.
object score { values value; }
function add(x, y) { result = x + y; }
...
```
By always including a `mark` line at the top, you can rerun scripts freely without worrying about memory “ghosts” or redefinition errors.

For more, see the [mark statement documentation](https://nickmat.github.io/glich/website/dev/man/script/stmts/mark.htm).

---

## Input and Output

### The `write` Statement

- The `write` statement outputs data to the standard output by default.
- It is frequently used to print values for testing or results.

**Example:**
```glich
write "Hello, world!" nl;   // Output: Hello, world!
```

- You can also use the `write` statement to output to files. This requires creating a file-code with the `file` statement.
- The file-code then acts as a qualifier for the `write` statement.

**Example – Writing to a file:**
```glich
file notes write "note.txt";
write.notes "Another note text" nl;
```

**Example – Reading from a file:**
```glich
file notes read "note.txt";
write @read.notes nl;    // Output: Another note text
```

- The `@read.file-code` function reads text from a file line by line.
- Each call to `@read.file-code` returns the next line of the file.

For full details on the file statement and file-based I/O, see:
[https://nickmat.github.io/glich/website/dev/man/script/stmts/file.htm](https://nickmat.github.io/glich/website/dev/man/script/stmts/file.htm)

---

> Expression errors produce an error value in their place and allow the script to continue,  
> while statement (syntax) errors immediately halt execution and send an error message to the error output.