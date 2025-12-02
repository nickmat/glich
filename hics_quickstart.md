# HICS-Quickstart

Welcome to the quickstart guide for **hics**—a flexible, powerful calendar and date-parsing toolkit. This guide introduces the central concepts step by step, showing you how to build and extend your own schemes, grammars, formats, and lexicons.

---

> **Note on Field Types:**  
> In hics, fields are divided into three classes:
> - **Fixed fields**: The fields supplied by the base calendar (e.g., year, month, day in the Gregorian calendar).  
>   Previously referred to as "base fields" or "default fields".
> - **Calculated fields**: Additional fields whose values are computed from the fixed fields, used for derived information, era names, etc.
> - **Optional fields**: Fields not needed to define a unique date, but which can be included for formatting or display—such as the day of the week.

---

## Range Shortcut: Working with Imprecise Dates

Historical records often record dates imprecisely—just the month and year, or even just the year.  
**hics** makes it easy to handle these cases: when you input a date string lacking a day, the conversion functions return a **date range** that covers all matching days.

### How it works

- If you provide just "Nov 2025", you’re referring to every day in November 2025.
    - `@date.g:dmy("Nov 2025")` will give the **date range** from the first to the last day of November 2025:  
      `2460981..2461010`
    - Conversely, `@text.g:dmy(2460981..2461010)` will return `"Nov 2025"`—the most compact form for that full-month range.

- Supplying only a year behaves similarly:
    - `@date.g:dmy("2025")` yields `2460677..2461041`, the date range for all days in 2025.
    - `@text.g:dmy(2460677..2461041)` outputs `"2025"`.

### Format-dependent

Not all formats will collapse a range back to a short string.  
For example:
- `@text.g:wdmy(2460677..2461041)` results in  
  `"Wed 1 Jan 2025..Wed 31 Dec 2025"`  
  since this format includes specific day-of-week and day-of-month components and can't be meaningfully abbreviated.

### Why is this useful?

- You can **faithfully represent historical imprecision**—where a record only mentions a month/year, hics keeps track of all possible days.
- Range-aware conversions allow flexible, automated reasoning and comparisons.

**Summary of the shortcut:**
- Input a less-specific date → get a date range.
- Output a date range that exactly matches a month or year → get an abbreviated label.

---

## Creating Your First Custom Scheme

The real power of hics is in extending the calendar system yourself.  
With the `scheme` statement, you can define your own calendar schemes—tailored to your needs or domain.

Official reference: [scheme statement documentation](https://nickmat.github.io/glich/website/dev/man/hics/scheme.htm)

### The Shortest Possible Scheme Definition

Let’s define a new scheme named `G`, built directly on the rules of the existing Gregorian calendar:

```glich
scheme G { base gregorian; }
```

- This creates a new calendar scheme called `G`.
- The new scheme inherits all Gregorian rules (leap years, month lengths, year boundaries, etc.).
- The scheme includes three **fixed fields**: `year`, `month`, and `day`.

### About the `base` Sub-statement

- The **base** describes the underlying calendar logic—these are called **base calendars**.
- The list of base calendars (`gregorian`, `jdn`, `julian`, etc.) will grow over time and support more cultures and conventions.
- Depending on the chosen base, you can add specific **keywords** to further customize behavior.
    - For instance, **`base jdn`** is just a raw count of days and is the same as hics’ internal day representation.
    - With `base gregorian`, the new calendar follows modern civil rules for year, month, day.

### Calendar Epochs

A **calendar’s epoch** is the first day of the calendar scheme—the point to which its day numbering and all conversions are anchored.

The epoch is crucial for defining new schemes, aligning the system with established scientific or historical conventions, or creating custom calendars.  
Typically, all days are counted from (and include) this starting epoch.

#### Example: Modified Julian Day (MJD)

The **Modified Julian Day (MJD)** system is a variation of the Julian Day Number in astronomy:

- **Epoch:** MJD’s epoch is defined such that **MJD 0 corresponds to JDN 2400001**.
- In hics, you can define a scheme where the "first day" (day number 1) matches **JDN 2400002**.

##### MJD Scheme Definition Example

```glich
scheme MJD {
    name "Modified Julian Day";
    base jdn epoch:2400002;
}
```

- Here, the `epoch:2400002` keyword ensures that **MJD day 1 = JDN 2400002**.
- In this scheme, **MJD 1** is the starting point, and all other days are numbered consecutively from there.

### Default Formats

When you create a scheme, two **default formats** are always provided:
- **`u`**  ("unit" format): Labeled fields, e.g., `2025year 11month 26day`
- **`def`** ("default" format): Space-separated, e.g., `2025 11 26`

These formats work with any scheme out-of-the-box.

**Examples:**

```glich
@text.G:u(2461006)         // outputs: "2025year 11month 26day"
@text.G:def(2461006)       // outputs: "2025 11 26"
```

Where `2461006` is the date number (Julian Day Number) for 26 November 2025.

### Quick Recap

- `scheme G { base gregorian; }` creates a Gregorian-based scheme you can use and extend.
- The `base` describes which date rules your new scheme follows.
- Many base calendars will be supported over time, allowing significant flexibility.
- For simple day-counting schemes, you can set the base to `jdn` and specify your own epoch.
- You can use @date, @text, and @scheme with signatures like `G:u`, `G:def`, or your custom scheme codes.

---

### Adding a Name: The `name` Sub-statement

You can also add a human-readable name to your scheme using the **name** sub-statement.  
This is helpful for documentation and makes it easier to generate or browse lists of available schemes.

```glich
scheme J { name "My Julian Test"; base julian; }
```

- Here, the scheme code is **J**
- `name "My Julian Test"` gives it a descriptive label, which will appear in listings, documentation, and UI.
- The scheme is otherwise based on the Julian calendar rules.

The name you provide is used by hics to:
- Display user-friendly names in lists of installed or available schemes.
- Improve error messages, documentation, and code-completion displays.

**Tip:** Even if you only use a scheme in scripts, giving it a name helps keep things organized when your collection grows.

---

## Defining Formats: The `grammar` Statement

Official documentation: [grammar statement](https://nickmat.github.io/glich/website/dev/man/hics/grammar.htm)

While every scheme has basic formats by default, the main—and safest—way to define new or custom formats is with the **grammar** statement.

### What is a Grammar?

A **grammar** defines how dates are parsed from or written to text.  
- It allows schemes to support new input/output styles.
- Grammars can be reused across schemes.
- Only the working features are covered here (this is an evolving system).

### How It Works

1. **Define a Grammar**  
   You must first define the grammar, specifying its field structure.  
   For example:
   ```glich
   grammar J {
       name "Test";
       fields year month day;
   }
   ```
   - The `fields` sub-statement is **required** and must exactly match the **fixed fields** expected by the base calendar scheme.  
     (This is a sanity check—if the fields do not match, results can be unpredictable!)

2. **Link the Grammar to Your Scheme**  
   In the scheme definition, link the grammar using the `grammar` sub-statement:
   ```glich
   scheme J {
       name "Test Julian";
       base julian;
       grammar J;
   }
   ```
   - Now, the scheme `J` uses the grammar `J` for parsing and formatting.
   - You can give your scheme a human-readable name as well.

### Why Is This Important?

- If you use the wrong field names or order, your scheme may fail or behave unpredictably.
- hics checks the list of fixed fields at link-time as a safeguard.
- Grammars make it safer and easier to share and maintain date formats across multiple schemes.

### Inheritance and Extension

While grammars can support inheritance and more advanced parsing features, this quickstart covers only the working, basic parts:
- Define a grammar with the correct fixed fields matching your base scheme.
- Link it directly in your scheme’s definition.

---

## Aliases: Making Formats Concise

By default, the **unit** format (`u`) uses the full field names for both input and output, e.g., `"2025year 11month 26day"`.  
This can be verbose and less convenient for frequent data entry or reading.

### Defining Aliases

You can define aliases to provide short forms for each field name:

```glich
alias unit { d day; m month; y year; }
```

- Here, `d` is mapped to `day`, `m` to `month`, and `y` to `year`.
- After defining this alias, you can use the short forms in any context where the unit (`u`) format is used.

### Example Use

Now, input and output with the `u` format can be much more compact:

```glich
@text.G:u(2461006)         // outputs: "2025y 11m 26d"
@date.G:u("2025y 11m 26d") // parses the short form correctly
```

> **Note:**  
> When using the unit (`u`) format:
> - **Output**: Fields always appear in the fixed order defined for the scheme (e.g., year, month, day).
> - **Input**: You can supply the fields in any order!  
>   For example, `@date.J:u("11m 26d 2025y")` parses just as well as `"2025y 11m 26d"`.

### Why Use Aliases?

- **Shorter, more efficient data entry.**
- Clear, unambiguous, and user-friendly strings.
- Especially valuable when working with many date values in scripts or data files.

---

## Defining Formats: `format` and the Grammar Statement

In hics, **formats** (such as `def`, `pretty`, etc.) are actually defined as part of a grammar—not directly on the scheme.  
The `format` and `alias` sub-statements are placed inside a `grammar` statement.

- Every scheme must be associated with a grammar (either an explicitly named one, or an anonymous grammar created automatically).
- When you use the `grammar` sub-statement in your `scheme`, your custom grammar and formats apply.
- If you do **not** specify a grammar, hics creates an anonymous grammar for you, which generates the default formats (`def`, `u`) based on your scheme’s fixed fields.

### Example: Defining Formats in a Grammar

```glich
grammar MyGrammar {
    fields year month day;
    format def "{year} |{month} |{day}";
    format pretty "{day}/{month}/{year}";
    alias unit { d day; m month; y year; }
}
scheme EX {
    base gregorian;
    grammar MyGrammar;
}
```

- The `grammar` statement defines:
    - `fields` (what **fixed fields** a date object contains)
    - Several named formats with the `format` sub-statement (using text rules)
    - An alias for unit-format field names

- The `scheme` simply links to this grammar.

### Default/Anonymous Grammar

If you create a scheme like this:

```glich
scheme G { base gregorian; }
```

- No grammar is specified, so hics will auto-create an anonymous grammar.
- This grammar supplies the default unit (`u`) and default (`def`) formats for the scheme’s fixed fields.

### Format Types

Most formats you’ll need can be described with text rules using the `format` sub-statement inside a grammar:

- **`format def "{year} |{month} |{day}";`**  
  Use template-like patterns for concise, readable output.
- **Unit and alias** are also defined in the grammar and used automatically.

There are also special built-in grammars/types for ISO 8601 (`iso`) and other cases.

---

### The `format` Sub-statement: Sections and the `|` Character

The `format` sub-statement inside a grammar lets you describe exactly how a date should appear as text.  
**A key feature:** The format string is broken into **sections** using the `|` (vertical bar) character.

#### Why are sections important?

- When you use a date **range shortcut**—as with `"Nov 2025"` or `"2025"`—some fields aren’t shown (e.g., day or month).
- hics removes the relevant **entire section** from the format string if that field isn’t present in the range, including all formatting characters belonging to that part.
- This ensures there are no stray slashes, spaces, or punctuation in the output:  
  For example, `"2025 11 26"`, `"Nov 2025"`, and `"2025"` all appear as clean strings, not `"2025 11"` or `"2025 "`.

#### Example: A Three-Part Format

```glich
format def "{year} |{month} |{day}";
```

**If all fields are present:**  
- `@text.EX:def(2461006)` outputs `"2025 11 26"`

**If only year and month:**  
- `@text.EX:def(2460981..2461010)` (all dates in Nov 2025) outputs `"2025 11"`

**If only year:**  
- `@text.EX:def(2460677..2461041)` (all dates in 2025) outputs `"2025"`

#### Practical Example

Suppose you want a format like `"26/11/2025"` (day, month, year):

```glich
format slash "{day}/|{month}/|{year}";
```

This allows range shortcuts to remove whole sections as needed:
- Range for Nov 2025: outputs `"11/2025"`
- Range for year: outputs `"2025"`
- Single day: `"26/11/2025"`

#### Rule of Thumb

- Each element separated by `|` is **atomic**: if any field in a section is omitted (as by a range), the entire piece is suppressed from output.

---

### Using Custom Separators with the Long Form `format` Statement

By default, hics only recognizes `:` and `,` as separator characters for parsing and output.  
If you want to use different separators (like `/`), you must use the **long form** of the `format` statement and declare them explicitly.

#### Basic vs. Long Form

- **Short form:**  
  ```glich
  format def "{year} |{month} |{day}";
  ```
  - Works with the default separators (`:`, `,`).
  - You can output fields separated by spaces or colons, but input with `/` will not parse as expected.

- **Long form:**  
  ```glich
  format slash {
      separators "/,";
      inout "{day}/|{month}/|{year}";
  }
  ```
  - `separators "/,";` declares `/` and `,` as valid separators for both parsing and output.
  - `inout` gives the template for both directions (input and output).
  - Now, both `"26/11/2025"` and `"26,11,2025"` will be parsed, and output will consistently use `/`.

#### Example Usage

```glich
grammar MyGrammar {
    fields year month day;
    format def "{year} |{month} |{day}";
    format slash {
        separators "/,";
        inout "{day}/|{month}/|{year}";
    }
}
scheme EX {
    base gregorian;
    grammar MyGrammar;
}
```

- Allows both `@date.EX:slash("26/11/2025")` and `@date.EX:slash("26,11,2025")` as valid inputs.
- Ensures `@text.EX:slash(2461006)` outputs `"26/11/2025"`.

#### Range-aware Output

With this configuration:
- A **month range** (`Nov 2025`): `@text.EX:slash(2460981..2461010)` outputs `"11/2025"`
- A **year range**: `@text.EX:slash(2460677..2461041)` outputs `"2025"`
- Single day: `"26/11/2025"`

---

## Formatting Using Lexicons

In hics, you can combine lexicons with your formats to include named values—like month names or day names—directly in your output strings.  
This is especially useful for creating human-readable or localized date strings.

### Example: Using a Month Lexicon in a Format

Suppose you have defined this month lexicon:

```glich
lexicon M {
    name "Month names (with pseudo)";
    fieldname month;
    tokens {
        1, January, Jan;
        2, February, Feb;
        // ...
        12, December, Dec;
    }
    pseudo Month, Mon;
}
```

You can reference this lexicon in your format by providing its code and additional selectors in the placeholder:

```glich
format dmy "{day} |{month:M} |{year}";
```

- `{month:M}`  outputs the **first entry** in the tokens list for the corresponding value (typically the full name: "November").
- `{month:M:a}` outputs the **second entry** (abbreviation: "Nov").

#### Case Modifiers

You can further control the capitalization with an additional suffix:
- `:l` → lowercase
- `:u` → uppercase

These can be chained, e.g.:
- `{month:M::u}` outputs "NOVEMBER"
- `{month:M:a:l}` outputs "nov"

#### Padding and Presentation

- `{month::lp:02}` Left pads the month number with zeros to 2 digits (e.g., "09").
- `{day::lp:02}`  Equivalent padding for the day.

#### Summary Table

| Placeholder        | Output Example      | Description                                         |
|--------------------|--------------------|-----------------------------------------------------|
| `{month}`          | `11`               | Numeric month (default)                             |
| `{month::lp:02}`   | `03`               | Zero-padded numeric month, 2 digits                 |
| `{month:M}`        | `November`         | Full month name (tokens entry 1)                    |
| `{month:M:a}`      | `Nov`              | Abbreviated name (tokens entry 2, "a" for abbrev)   |
| `{month:M:a:l}`    | `nov`              | Abbreviation, forced lowercase                      |
| `{month:M::u}`     | `NOVEMBER`         | Full month name, forced uppercase                   |

You can combine these options as needed for full control in any format string.

---

## Attaching Lexicons for Parsing: The `lexicons` Sub-statement

When working with lexicons, you not only control output, but also parsing—accepting natural language date elements as input.

The `lexicons` sub-statement in your grammar lets you list all the lexicons that should be considered when parsing text.  
When hics encounters a word or string in place of a number, it searches through all listed lexicons to find a match for that field's value.

### Example: Declaring Multiple Lexicons in a Grammar

Suppose you've defined several lexicons for month names, weekdays, etc.:

```glich
lexicon M { ... }   // months
lexicon W { ... }   // weekdays
```

You attach them to your grammar like so:

```glich
grammar MyGrammar {
    fields year month day;
    lexicons M, W;
    ...
}
```

- With `lexicons M, W;`, hics will check both the month (M) and weekday (W) lexicons during parsing.
- When the parser encounters text (like "Nov" or "Monday"), it attempts to match each token in all active lexicons, in the order listed.

### How It Works in Parsing

- If a string matches a token in any of your active lexicons for a particular field, it's converted to the proper value automatically.
- Especially important for supporting multiple languages, various abbreviations, or alternative historical names.
- The `lexicons` sub-statement ensures you don't miss any acceptable form in input.

---

## Handling Names and Words: The `lexicon` Statement

Dates aren't always just numbers—humans use month names, day names, and other natural language strings in date expressions.  
hics tackles this with the **lexicon** statement, letting you map numbers to strings and vice versa for any scheme field.

Official documentation: [lexicon statement](https://nickmat.github.io/glich/website/dev/man/hics/lexicon.htm)

### What is a Lexicon?

A **lexicon** is a dictionary telling hics how to interpret and output words for fields like months, weekdays, eras, and more.  
You provide:
- A name (for documentation and referencing)
- Which field ("month", "wday", etc.) this lexicon serves
- A list of token groups: each line lists the canonical value, then possible text tokens (full names, abbreviations, etc.)

### Lexicon Token Lines: Structure and Conventions

Each line in the `tokens` block of a `lexicon` defines all the acceptable and display values for a single field value. The format is:

```
canonical_value, full_default_name [, abbreviated_name [, alternative1 [, alternative2, ... ] ] ];
```

- **canonical_value**: The underlying value in your scheme (usually a number, or a canonical string/enum).
- **full_default_name**: The preferred or “main” string for that value, always listed first.  
  - This is used for display/output by default.
  - If it contains spaces or unusual characters, wrap it in quotes (`"..."`).
  - If it’s a plain word (A-Z, initial cap, no spaces/specials), quotes are optional.
- **abbreviated_name**: A short or common abbreviation.  
  - Can also be quoted if needed, but often a single token.
- **alternatives**: Any further alternative strings that should be accepted in parsing (covering misspellings, variant spellings, etc.).  
  - All are parsed as this value.

**Example with spaces, quoting, and alternatives:**

```glich
lexicon FRCOMP {
    name "French complementary days";
    fieldname cday;
    tokens {
        1, "Fête de la Vertu",   "Ver";
        2, "Fête du Génie",      "Gen";
        // ...
        5, "Fête de la Récompense", "Rec", "Récompense", "Recompense";
        // (sometimes 6, in a leap year)
    }
}
```

Here, the fieldname is `cday` for the "complementary days" at the end of the French Revolutionary calendar year.  
These days don't fit the 30-day month pattern and often require special care in formatting and grammar definition.

*Note: You generally don't have to worry about these cases unless you are working with specialized calendars, but hics provides the flexibility to handle even these historical formatting challenges.*

---

## Pseudo Dates: Showing Example Output Formats

When describing or documenting date formats, it's useful to include an example string that shows the expected appearance—like `"dd Mon yyyy"` for `"26 Nov 2025"` or `"Month dd, yyyy"` for `"November 26, 2025"`.  
In hics, we call these **pseudo dates**. They serve as clear, human-oriented illustrations of how date components are rendered in a given format.

### How Pseudo Dates Work

- **Pseudo dates don't represent an actual date value.**
- Instead, they act as labeled templates, showing the pattern for output.
- This is especially important for documentation, UI hints, or dynamically describing what a format string means.

#### Example: Abbreviated and Full Month Names

Suppose you have a month lexicon and want to show in documentation that both full ("Month") and abbreviated ("Mon") forms are supported.  
You can declare this directly in the lexicon:

```glich
lexicon M {
    name "Month names (with pseudo)";
    fieldname month;
    tokens {
        1, January, Jan;
        2, February, Feb;
        // ...
        12, December, Dec;
    }
    pseudo Month, Mon;
}
```

- **pseudo Month, Mon;**  
  - `Month` is the label for the full month name (e.g., "November"),
  - `Mon` is the label for the abbreviated form (e.g., "Nov").

#### Pseudo Aliases in Grammar

You can also specify how to show number placeholders for output examples using the `alias pseudo` sub-statement in your grammar:

```glich
grammar MyGrammar {
    fields day month year;
    alias pseudo { dd day; mm month; yyyy year; }
}
```

- This tells hics (and any tooling around it) to generate, for example, the string `"dd mm yyyy"` to illustrate the expected numeric ordering and widths, instead of just showing a sample with real numbers.

### Why are Pseudo Dates Useful?

- They automatically generate **format examples** for humans—making documentation and UI design easier.
- They clarify the expected rendering of each format your calendar supports.
- Output generation is usually precise; input can be much more flexible (names, abbreviations, numbers in any order, etc.).

---

## Note on Text Input: The Importance of Field Order

While hics is very flexible with text input—accepting numbers, names, abbreviations, and even labels/aliases—**the order of fields in an unlabeled input string is determined by the format string**, not by the grammar's field order.

For example:
- If your format is `"{day} {month} {year}"`, then the string `"2 3 2025"` will be interpreted as **2 March 2025**.
- If your format is `"{month} {day} {year}"`, then `"2 3 2025"` will mean **February 3, 2025**.

> **Important:**  
> The order of field names in the grammar’s `fields` sub-statement is always from largest to smallest unit (e.g. `year month day`).  
> This does **not** dictate how input is parsed—**the format string does.**

**Best Practice:**  
- Always ensure that your text input matches the field order in the format string for unlabelled/default formats.
- In ambiguous cases (e.g. is `"2 3 2025"` day-month-year or month-day-year?), use labeled/unit formats or document your expected input with pseudo dates.

**Tip:**  
- Labeled/unit formats (e.g., `"2025y 3m 2d"`) or unique separators in custom formats can help avoid confusion in data entry.

---

## Advanced Format Specifiers: Padding, Ordinals, Roman Numerals, and More

hics format placeholders support a wide range of modifiers for output customization—including number formatting, ordinal suffixes, and Roman numerals. These enhance how dates are presented in both plain text and rich output.

### Padding Numbers

You can use the `::lp:N` modifier (left-pad to N places) for numeric fields.  
Optionally, specify the padding character.

- `{day::lp:02}` → Day padded to 2 digits: `"07"`, `"26"`

### Roman Numerals

You can output numbers as roman numerals using the `::rn` modifier.  
Further, use `:l` or `:u` for lowercase or uppercase.

- `{month::rn:l}` → Month as lowercase roman numerals: `"xi"` for November
- `{month::rn:u}` → Month as uppercase: `"XI"`

**Example:**

```glich
format full { output "{day}.{month::rn:l}.{year}"; }
```
Outputs `"26.xi.2025"` for November 26, 2025.

> **Note:** Roman numeral formatting is **write-only**—it can only be used for output, not for parsing input.

### Ordinal Suffixes

Two additional specifiers provide ordinal suffixes for numbers:

- `::os` appends the ordinal suffix to a number:
    - `{day::os}` → `"26th"`, `"1st"`, `"3rd"`
- `::oa` outputs **only** the ordinal suffix for the number:
    - `{day::oa}` →  `"th"`, `"st"`, `"rd"`

This is especially useful for HTML or rich-text output.

**Examples:**

```glich
format html {
    output "{day}<sup>{day::oa}</sup> day of {month:M} in the year {year}";
}
```

Outputs:

```
26<sup>th</sup> day of November in the year 2025
```

> **Note:** Ordinal suffixes and Roman numeral output are **write-only**: you can display them but cannot use them for input parsing yet.

---

#### Summary Table: Advanced Format Modifiers

| Specifier          | Output Example        | Description                           | Readable as input? |
|--------------------|----------------------|---------------------------------------|--------------------|
| `{month::lp:02}`   | `03`                 | Zero-padded numeric                   | Yes               |
| `{month::rn:l}`    | `xi`                 | Roman numerals (lowercase)            | No                |
| `{day::os}`        | `26th`               | Ordinal with suffix                   | No                |
| `{day::oa}`        | `th`                 | Ordinal suffix only                   | No                |

---

**Summary:**  
- Use padding, case, ordinal, and roman numeral specifiers for fine-grained formatting.
- These features expand hics’ utility for documentation, output, and publishing—though some advanced options (like ordinals and roman numerals) are output only.

---

## Note on Separator Choice and ISO Formats

A practical point: using a hyphen `-` as a separator is common (e.g., `2025-11-26`), but it is potentially ambiguous in hics, because negative values (such as negative years, i.e., BCE/BC) are also allowed. This can lead to confusion between field separators and negative signs.

### Example of Potential Ambiguity

- `-0044-03-15` could mean March 15th, 44 BCE (negative year), or be misinterpreted if not parsed carefully.
- Unfortunately, the hyphen is the standard separator in many contexts, including ISO 8601.

hics handles this by not making the hyphen a default separator for regular text formats.  
**If you want to use a standard like ISO 8601, use the built-in rules instead of a hand-written format string.**

### Defining ISO 8601 Formats in hics

The ISO 8601 standard is implemented as a special rule:

```glich
format iso {
    rules iso8601;
}
```

This enables full ISO 8601 conformance and handles negatives, extended years, and strict formatting.

#### Adding ISO Modifiers

You can add switches to adjust the behavior.  
For example, to force the sign (`+` or `-`) always to appear on the year, use:

```glich
format s_iso {
    rules iso8601 sign;
}
```

- This will output `+2025-11-26` for AD dates and `-0044-03-15` for BCE.

You can explore additional modifiers in the hics documentation for fine-tuning padding, extended year support, week dates, and so forth.

---

## Note on ISO 8601 and Year Zero

A special detail of the ISO 8601 date standard is its treatment of the year zero.  
Unlike historical AD/BC or CE/BCE systems—which typically do **not** include a year zero—ISO 8601 defines the proleptic Gregorian calendar such that:

- **Year `0000`** corresponds to **1 BCE**
- **Year `-0001`** corresponds to **2 BCE**
- ...and so on

There is **no "AD"/"BC"** (or "CE"/"BCE") distinction in ISO 8601 notation.  
Instead, years are signed integers, and the year zero is meaningful and necessary for unambiguous date computation.

### Practical Implication

- When you output or parse ISO 8601 dates in hics with `rules iso8601`, you are using the **ISO proleptic Gregorian year numbering**.
- This may not match traditional historical labels.
    - Example: `0000-01-01` in ISO 8601 is January 1, 1 BCE.
    - `-0001-01-01` is January 1, 2 BCE.

**Summary:**  
- Do not describe ISO-format strings as “AD” or “BCE/BC”.
- Use “ISO year numbering” or “proleptic Gregorian” for clarity in these contexts.

---

## Year Zero and Negative Years in hics Base Calendars

One important design choice in hics is that **all base calendars use a year zero and support negative years (proleptic years) for computational simplicity and consistency.**

- Year zero is always present.
- Negative years follow the astronomical numbering system:
  - Year 0 = 1 BCE
  - Year -1 = 2 BCE
  - Year 1 = 1 CE
  - ...and so forth.

### Compatibility with Traditional Eras (BCE/CE)

If you want a system that outputs eras (such as "BCE"/"CE" or "BC"/"AD"), you can define a custom scheme with calculated fields or use the built-in scheme provided for this purpose.  
For example:

- The hics library includes a prebuilt `jce` scheme ("Julian with Common Era"), which outputs explicit BCE/CE eras.
- This scheme requires **additional calculated fields** and grammar support.

**Note:**  
This quickstart doesn’t cover calculated fields, but be aware that you can extend hics to support traditional historical era notations and logic on top of its base proleptic year system.

---

## Problem: BCE/CE Representation in the Julian Calendar (JCE)

When working with historical calendars, particularly the Julian calendar, negative year numbering is the norm for dates before year 1.  
Astronomical and proleptic calendars often use year 0 for 1 BCE, year -1 for 2 BCE, and so on.  
However, for most historical and user-facing purposes, people expect BC/BCE ("Before Common Era") and AD/CE ("Common Era") style labeling, without a year zero.

### What makes this tricky?

- **Astronomical year numbering:**  
  - Year ... -2, -1, 0, 1, 2 ... corresponds to  
    ... 3 BCE, 2 BCE, 1 BCE, 1 CE, 2 CE ...
- **Common historical notation:**  
  - There is no year zero; 1 BCE is followed immediately by 1 CE.

### Typical user expectation

| Astronomical Year | BCE/CE Label | User expects |
|-------------------|--------------|--------------|
| ... -2            | 3 BCE        | "3 BCE"      |
| -1                | 2 BCE        | "2 BCE"      |
| 0                 | 1 BCE        | "1 BCE"      |
| 1                 | 1 CE         | "1 CE"       |
| 2                 | 2 CE         | "2 CE"       |

### Why is this a problem?

- Without custom formatting, hics (or any system based on fixed fields from the base calendar) will simply output the astronomical year number—which is historically odd for BCE dates.
- Users expect "44 BCE", not "-43", for the Ides of March (March 15, 44 BCE).

### Solution: Calculated Fields

By introducing **calculated fields** (see below), we can:
- Output the year as users expect, using `ceyear` for BCE/CE notation (not `cyear`, which in hics is reserved for the civil year, discussed separately)
- Add an explicit BCE or CE era label (`ce`)
- Hide the underlying astronomical numbering in user displays

The [standard JCE scheme in the hics library](https://nickmat.github.io/glich/website/dev/man/defs/jce.htm) implements full BCE/CE date representation for the Julian calendar.  
We’ll show a simplified version next.

---

## Implementing Calculated Fields for BCE/CE Output

To achieve user-friendly BCE/CE year labeling in the Julian calendar, we introduce two **calculated fields** via the grammar:

- `ce` – The era field. Values:  
  - `0` for BCE  
  - `1` for CE
- `ceyear` – The proleptic year in BCE/CE form; a strictly positive, nonzero year.

### Declaring Calculated Fields

In the grammar statement, declare the calculated fields immediately after the fixed fields:

```glich
grammar JCE {
    fields year month day ce ceyear;
    calculated ce ceyear;
    rank ce ceyear month day;
    lexicons CE, M;
    format def "{day} {month:M} {ceyear} {ce:CE}";         // 15 March 44 Before Common Era
    format yearonly "{ceyear} {ce:CE:a}";                  // 44 BCE
    format full "{day} {month:M:a}, {ceyear} {ce:CE}";     // 15 Mar, 44 Before Common Era
    function calculate {
        ce = @if( year < 1, 0, 1 );
        ceyear = @if( year < 1, -year + 1, year );
        result = this;
    }
    function fixed {
        year = @if( ce = 1, ceyear, -ceyear + 1 );
        result = this;
    }
}
```

- The order in `fields` is significant:  
  **Fixed fields** (such as `year`, `month`, `day`) are supplied automatically by the `base` statement in the scheme (e.g., `base julian;`).  
  The calculated fields (`ce`, `ceyear`) follow the fixed fields in the grammar’s field list.
- The `rank` statement is required so shortcut formats work correctly when calculated fields are present.

### Usage Example in a Scheme

**Note:** The grammar must be defined **before** the scheme, since the scheme references it.

```glich
scheme JCE {
    name "Julian with Common Era";
    base julian;
    grammar JCE;
}
```

---

### Lexicon for Era (`ce`)

We provide both the full and abbreviated forms for BCE and CE:

```glich
lexicon CE {
    name "Common Era";
    fieldname ce;
    tokens {
        0, "Before Common Era", BCE;
        1, "Common Era", CE;
    }
}
```
- The first string for each value is the full name (e.g., "Before Common Era")
- The second string is the abbreviation ("BCE")

### Lexicon for Months (Shared)

```glich
lexicon M {
    name "Month Names";
    fieldname month;
    tokens {
        1, January, Jan;
        2, February, Feb;
        // ...through...
        12, December, Dec;
    }
}
```

---

### Usage Examples

- For -43-03-15 (March 15, 44 BCE):

    - `@text.JCE:def(-43,3,15)` → `15 March 44 Before Common Era`
    - `@text.JCE:yearonly(-43,3,15)` → `44 BCE`

- For 2025-11-26:

    - `@text.JCE:def(2025,11,26)` → `26 November 2025 Common Era`
    - `@text.JCE:yearonly(2025,11,26)` → `2025 CE`

---

**Summary:**
- Always define a grammar before its referencing scheme.
- Use the same name for grammar and scheme when they match one-to-one (as in "JCE" here).
- Provide full and abbreviated values in the era lexicon, accessed by standard format selectors.
- Formats use calculated fields (`ceyear`, `ce`) and shared fixed fields as usual.

---

## Civil Year vs. Historical Year and Dual Dating

Another important calendar variation is where the start of the year differs from January 1.  
This is known as the **civil year** (begins on a locally defined date) versus the **historical year** (begins on January 1).

### The Case of England Before 1752

- **Historical Year**: Year count increases on **1st January** (modern usage).
- **Civil Year**: Year count increases on **25th March** ("Lady Day"), which was the legal new year in England until 1752.

#### Practical Effect

- From 1 Jan to 24 Mar, the historical year and the civil year were **off by one**.
- From 25 Mar to 31 Dec, both year numbers matched.
- **Key problem:** Many dates in historical documents appear with the "wrong" year if you do not account for which system was used.

#### Example

- On **14 Feb "historical year" 1748**, the **civil year** is still 1747.
- Proper dual dating:  
  - Written as `"14 Feb 1747/8"`
  - Civil year in full (`1747`), a slash `/`, then the minimal different digits of the historical year (`8` for 1748).

  If the years are the same, just give the year:
  - `"25 Mar 1748"` (year starts align)

#### Importance

- The difference (the **year delta**) can be 0 or -1/+1, depending on the date and the calendar.
- This **dual dating** practice was developed to reduce confusion at the time and is still vital for interpreting historical records.

### Implementation Note

- In hics, you can create schemes that track both the civil year and the historical year as **calculated fields** and then use a format rule to output dual dating as `"dd M yyyy/yy"` as appropriate.

#### Example Rendering

| Date (Gregorian)         | Civil Year | Historical Year | Dual Dating   |
|--------------------------|------------|-----------------|--------------|
| 24 Feb 1747 (Old Style)  | 1747       | 1748            | 24 Feb 1747/8 |
| 25 Mar 1748              | 1748       | 1748            | 25 Mar 1748   |

---

**Summary:**  
- Many historical records use a civil year that starts on dates other than January 1.
- Always clarify whether you are reading civil or historical year in a document.
- Dual dating (e.g. "14 Feb 1747/8") is the unambiguous historical solution and is supported in hics via calculated fields and format strings.

---

## Implementing Dual Dating with the Civil Year (`cyear`)

hics provides a convenient way to handle civil years—years that begin on dates other than January 1—using a calculated field called `cyear`.  
The fixed field `year` always represents the historical (Jan 1-based) year; `cyear` is the civil year (starting at the defined new year).

### How to Implement

#### 1. Grammar: Add `cyear` as a Calculated Field

```glich
grammar OLDSTYLE {
    fields year month day cyear;
    calculated cyear;
    rank cyear month day;
    use epoch;
    // Add formats and lexicons as needed...
}
```
- `cyear` is the civil year field to be calculated.
- `use epoch;` tells hics to generate the necessary `calculate` and `fixed` functions for you (no need to code manually).
- `rank` is set for shortcut/abbreviation logic.

#### 2. Scheme: Define When the Civil Year Starts

```glich
scheme OLDSTYLE {
    name "Julian (England, Old Style Civil Year)";
    base julian;
    grammar OLDSTYLE;
    epoch 1721417;                   // or epoch @date.j:dmy("25 Mar 0");
}
```
- `epoch` sets the start of the *first* civil year (the civil new year).  
  For England, Old Style:  
  `epoch 1721417;` is JDN for 25 March, year 0 (Julian).

#### 3. Usage

- The grammar and scheme above allow hics to manage the difference between civil and historical years automatically.
- Input and output can now support dual dating, e.g.,  
  `"14 Feb 1747/8"`  
  `"25 Mar 1748"`

#### 4. Formatting for Dual Dating

You can use a custom format string and a helper function or pseudo date to produce abbreviated dual dating as needed.  
For example, you might design a format that outputs `"14 Feb 1747/8"` where the civil year (`cyear`) and historical year (`year`) differ.

---

## Note on Shortcut Ranges with Civil Year (Dual Dating)

When the year boundary is not January 1—such as with the civil year starting on March 25 (Old Style in England)—date ranges become more complex.  
**Shortcut ranges are NOT available** for these schemes.

### Why are shortcut ranges not supported?

- In these calendars, a given month (e.g., "March 1748") might be split between two years:
    - Early March belongs to the previous civil year, later March to the next.
- As a result, it is **ambiguous** which year to use for month- or year-level shortcut abbreviations.

**Example:**
- "March 1748"—Does that mean the part of March in civil year 1747, or in civil year 1748?

**Consequence:**  
- hics disables shortcut range abbreviations (month-only, year-only) for civil year schemes to avoid confusion and incorrect representations.

**Summary:**  
- When year boundaries differ from January 1, only fully specified (day-level) dates are safely and unambiguously formatted.
- For historical accuracy and user clarity, always use explicit full dates and dual dating (e.g., `14 Feb 1747/8`), not abbreviated ranges.

---

## Hybrid Calendars: Example – The Astronomical Year Calendar (AY)

A **hybrid calendar** in hics is a scheme that automatically switches between two or more base calendars, using each one for its correct historical (or logical) period.  
This feature is especially useful for representing whole eras or astronomical conventions where a numbering system changes at a clear cutover.

### The Astronomical Year Calendar (AY)

The **Astronomical Year calendar** (see [AY scheme documentation](https://nickmat.github.io/glich/website/dev/man/defs/ay.htm)) is a hybrid that provides:

- **Julian calendar** (with astronomical year numbering, including year zero) for all dates before the Gregorian reform.
- **Gregorian calendar** (astronomical year numbering, continuing year zero rules) for dates from the Gregorian reform onward.

This allows astronomers and historians to unambiguously reference any date in a consistent proleptic year system, while correctly encoding the historical switch from Julian to Gregorian.

### How It Works in hics

You define a **hybrid scheme** specifying where the transition takes place.

#### Example Hybrid Calendar Definition

```glich
scheme AY {
    name "Astronomical Year calendar";
    base hybrid {
        // Julian calendar with astronomical year numbering up to Gregorian cutover
        julian (astro) through 2299160;
        // Gregorian calendar with astronomical year numbering from cutover onward
        gregorian (astro) from 2299161;
    }
    grammar AY;
}
```

- `(astro)` signals astronomical year numbering—years 0, -1, etc., are allowed according to the astronomical system (not traditional BCE/CE).
- The cutover occurs at JDN 2299160/2299161, which is 4 October 1582 (Julian) / 15 October 1582 (Gregorian), the official Gregorian calendar reform start.

### Shortcut Ranges and the Astronomical Year Calendar

- Unlike civil/julian hybrids where months/years can split, the AY’s cutover is a single, clean day boundary.
- **Shortcut ranges** (e.g., "44" meaning the entire year -43 or "2025" meaning 2025 CE) are fully supported.
  - So, `@date.AY:u("44")` returns all days in astronomical year 44.
  - `@text.AY:u(-43,3,15)` outputs `-43 3 15` or formatted per your formats.

### Why Hybrid Schemes Are Useful

- Allow a single scheme to be accurate across eras that otherwise would require separate calendars or complex manual logic.
- Enable accurate conversion and output for astronomical, historical, or legal timekeeping needs across the Julian/Gregorian boundary.
- Maintain correct leap years, day counts, and field numbering logic at all times.

---

## Hybrid Scheme Script Requirements: The "scheme" Field and Internal Structure

When constructing a hybrid calendar scheme in hics, there are a few key script requirements to ensure robust parsing and precision, especially as hybrid models become more advanced.

### The `scheme` Field

- **New field:** All hybrid schemes require a field named `scheme`.
    - The `scheme` field indicates *which calendar regime* (e.g., Julian or Gregorian) is currently active for a given date.
    - It is placed **first** in the `fields` list, as it possesses the highest granularity for range/shortcut purposes.
- While `scheme` is critical for calculations, it is usually **redundant** for output (since humans don't need to see which regime applies for most ordinary dates), so it is rarely included in the primary format strings.

#### Example for Astronomical Year Calendar (AY):

```glich
fields scheme year month day;
rank year month day;
```
- `scheme` is first in `fields` (per requirements), but **not** included in `rank`, so normal date shortcuts function as expected.
- In format strings, skip `{scheme}` unless you want to explicitly display the regime for debugging or technical purposes.

#### Notes:

- On **input**, including the scheme name is **optional** (provided input is as text).
    - This allows transparent composition from user strings like `"1582 10 15"` or `"gregorian 1582 10 15"`.
    - You must define a **lexicon** for `scheme`, mapping integer codes (`0`, `1`, ...) to scheme names (e.g., `Julian`, `Gregorian`).
- The `fields` statement for the hybrid is both a check for field sets (sanity enforcement) and will be required for more complex hybrids.

### Hybrid `base` Script Structure

The core "base" assignment for a hybrid scheme is structured as:

```glich
base hybrid {
    fields scheme year month day;
    scheme 0 { base julian; }      // local scheme "0", Julian rules
    change 2299161;                // JDN at which to switch
    scheme 1 { base gregorian; }   // local scheme "1", Gregorian rules
}
```
- **schemes** are numbered (`0`, `1`, …) automatically, but you may assign codes for clarity.
- Each `scheme { ... }` block defines the local settings for that segment of the calendar.

#### Example Lexicon for `scheme` Field

```glich
lexicon CAL {
    name "Calendar Regime";
    fieldname scheme;
    tokens {
        0, Julian;
        1, Gregorian;
    }
}
```

**Full Example: Astronomical Year Hybrid**

```glich
lexicon CAL {
    name "Calendar Regime";
    fieldname scheme;
    tokens {
        0, Julian;
        1, Gregorian;
    }
}

grammar AY {
    fields scheme year month day;
    lexicons CAL, M;
    rank year month day;
    format def "{year} {month} {day}";
    // Additional formats as needed...
}

scheme AY {
    name "Astronomical Year calendar (hybrid)";
    base hybrid {
        fields scheme year month day;
        scheme 0 { base julian (astro); }
        change 2299161;
        scheme 1 { base gregorian (astro); }
    }
    grammar AY;
}
```

- The grammar's `fields` matches what the hybrid declares.
- The format string typically omits the `scheme` field.
- For user input, `"Julian 44 3 15"`, `"Gregorian 1582 10 15"`, or simply `"44 3 15"` will be interpreted correctly if within range.
- `scheme` field can be hidden or shown in debugging, auditing, or when technical transparency is needed.

---

## Hybrid Calendars: Regnal and Era-Based Calendars (Coming Soon)

While most hybrid calendars in hics switch between different civil or astronomical calendars at fixed dates, there are also calendars whose years are based on the accession or reign of a monarch, official, or even an era.

### What is a Regnal or Era-Based Calendar?

- In a **regnal calendar**, each "year one" begins on the date of accession of a king, queen, emperor, pope, official, etc.  
  Examples: English regnal years (“1 Elizabeth I”), Japanese era names (“Reiwa 1”), papal reigns, or dynastic chronologies.
- Years and even months may reset with each reign.
- Some calendars use combinations of regnal and civil/era systems (e.g., East Asian lunisolar calendars with era switches at the emperor's accession).

### Why is this a Hybrid Problem?

- These calendars are even more complex hybrids:  
  Not only might the base calendar (Julian/Gregorian/lunisolar/etc.) change, but the *anchor date* for "year 1" changes whenever a new monarch or era begins.
- Transitions do not happen at a fixed point worldwide but are tied to specific political or historical events.
- Handling overlapping, interrupted, or short-lived reigns adds further complexity.

### Status in hics

Support for **regnal and era-based calendars** is **in development**.  
- Features will include:
    - Defining reign/era periods and the corresponding anchor dates.
    - Automatic calculation of regnal year/era fields.
    - Formatting functions for standard notation, e.g., "3 Charles III", "2 Reiwa".

If you have specific historical requirements or are planning to work with regnal chronologies, keep an eye on future hics releases and documentation for formal support!

---

**Summary:**  
- Regnal/era calendars are a complex form of hybrid calendar, with variable "year 1" anchor points and repeated year numbers per reign or era.
- hics aims to provide clean support for these systems for historians, genealogists, and anyone working with such chronology.
- Full regnal/era functionality is in the works!

---

## How hics Parses Date Strings: Elements, Boundaries, and Mixed Input

One of the powerful, and sometimes surprising, features of hics is its ability to accept a wide range of date string formats.  
However, its parsing approach is a little unusual and has specific rules.

### Parsing Rules for Date Elements

- An input string is parsed into **date elements** corresponding to each field.
- Elements can be runs of digits (numbers) or runs of text (words/names).
- **Text elements must be separated by a separation character** (normally a comma `,`); a space alone is **not** a separator between two text elements.
- **Transitions between digit and non-digit** (and vice versa) **start a new element**, even without a separator.

#### Separation Rules

1. **Separator Characters:** By default, `:` or `,` are always element separators.
   - These allow you to explicitly separate elements, especially for cases with consecutive text elements.
2. **Digit-Text Transitions:** Whenever the parser sees a transition from digit to non-digit (or vice versa), a new element begins, even with no separators or spaces.
   - Example: `1dec2025` → `1`, `dec`, `2025`
3. **Spaces:**
    - **If both sides of a space are text:** The space is **not** a separator; it will be included in the text element.
      - Example: `Rabī‘ al-Awwal` is parsed as a single text element.
    - **If at least one side of the space is a digit:** The space **is** a separator.
      - Example: `1 12 2025` → `1`, `12`, `2025`
      - Example: `15 Rabī‘ al-Awwal 1445` → `15`, `Rabī‘ al-Awwal`, `1445`
4. **Multiple Text Elements:** To separate adjacent text fields (e.g., an era and a month), a separation character (e.g. comma) **must** be used; a space will not separate two text elements.

#### Examples

| Input string               | Parsed as                                                   |
|----------------------------|------------------------------------------------------------|
| `1dec2025`                | `1`, `dec`, `2025`                                         |
| `1 12 2025`               | `1`, `12`, `2025`                                          |
| `19Feb03`                 | `19`, `Feb`, `03`                                          |
| `23/2/1923`               | `23`, `2`, `1923`                                          |
| `15 Rabī‘ al-Awwal 1445`  | `15`, `Rabī‘ al-Awwal`, `1445`                             |
| `julian, dauphin, 1 dec`  | `julian`, `dauphin`, `1`, `dec`                            |
| `julian  dauphin  1 dec`  | `julian dauphin`, `1`, `dec`                               |

*Note: When in doubt, use a comma or other defined separator to ensure unambiguous parsing of multiple text elements!*

### Advantages

- **Flexible**: Handles terse/concise inputs and natural language forms.
- **Localized**: Supports names and Unicode from many languages;
- **Reliable**: Robust distinction between numbers and names, minimizes parsing errors.

---

**Summary:**  
- hics splits text into elements at explicit separators or when digit/non-digit groups change.
- Two adjacent text elements **must** be separated by a separator character (e.g., comma), not just spaces.
- A space is **not** a separator if both sides are text; it is included in the text.
- Use explicit separators for complex calendars (multiple names/eras) to avoid ambiguity.

---

## Advanced: Date Phrases and Set Operations

hics supports **date phrases**, letting you combine date expressions—including multiple calendar schemes—using **set operators**.  
This allows you to express unions, intersections, exclusions, and differences between periods in a single phrase, returning an ordered list (RList) that can be displayed or further manipulated.

### What Is a Date Phrase?

A **date phrase** is a textual expression that combines date values and date ranges using one or more set operators.  
- Each value must be a **valid date or shortcut range** (e.g., `"2025"`, `"Feb 2025"`, `"julian, 1 Jan 1600"`).
- Values may be grouped with parentheses `( ... )` for clarity and precedence.

### The `@phrase` Function

- **Syntax:** `@phrase(<text phrase string>)`
- Evaluates a phrase into an RList (list of date ranges).

#### Supported Set Operators

| Operator    | Meaning                 | Example usage                                |
|-------------|-------------------------|----------------------------------------------|
| `..`        | Range (inclusive)       | `1 Mar 2025..5 Mar 2025`                     |
| `!`         | Exclusion (NOT)         | `2025 && !30 Nov 2025`                       |
| &#124;      | Union (OR)              | `2024 &#124; 2025`                           |
| `&&`        | Intersection (AND)      | `2025 && Wed`                                |
| `^^`        | Symmetric difference    | `2022 ^^ 2023`                               |
| `\\`        | Set subtraction (A\B)   | `2025 \\ Jan 2025`                           |

> **Note:** The set subtraction operator is **two backslashes** (`\\`).

#### Notes

- **Parentheses** `( ... )` are supported for grouping and controlling precedence.
- All values between operators must be well-formed **dates or shortcut ranges** in valid hics syntax (see earlier sections).
- New (experimental) operators may appear in future releases.

### Scheme and Format Qualification

To use nond-default schemes in a phrase, prefix the entry with `scheme[:format]#`, for example:

```glich
@phrase("2025 | j:dmy# 2025")
```
- Combines the Gregorian year 2025 and Julian year 2025.

#### Example Usages

```glich
// Range and union
@text.g:dmy(@phrase("1 Jan 2025..10 Jan 2025 | 15 Jan 2025"))

// Exclusion
@text.g:dmy(@phrase("2025 && !30 Nov 2025"))

// Scheme qualification
@text.g:dmy(@phrase("2025 | j:dmy# 2025"))

// Set subtraction
@text.g:dmy(@phrase("2025 \\ Jan 2025"))

// Parentheses for grouping
@text.g:dmy(@phrase("(2025 | 2026) && !10 Jan 2025"))
```

### Limitations (for now)

- Only valid date values or shortcut ranges (not arbitrary words) are accepted.
- More advanced operators beyond those listed are in development/testing.

---

**Summary:**  
- Date phrases let you combine and manipulate lists or ranges of dates using familiar set operators, with parentheses for grouping.
- The set subtraction operator is two backslashes: `\\`.
- Each value must be a valid date expression.
- You can mix schemes/format using qualification; see examples above.

---

## Historical Calendars and Their Periods of Validity

When working with historical calendar schemes in hics, it is important to understand the distinction between the **period of actual use** (historical validity) and the mathematical extension (**proleptic system**) provided by the calendar algorithm.

### Period of Use Versus Proleptic Extension

- Each historical calendar was used over a specific span of years—outside of this range, its written rules may become ambiguous or self-contradictory.
- **hics** calendar schemes are typically implemented so that the rules "work" mathematically for any date (including outside the real period of use), but:
  - Outside the true period of validity, computed results may not reflect any real historic usage.
  - Results are consistent and defined by the algorithm, but may lack historical or legal meaning.

### Example: The French Republican Calendar

- The French Republican calendar’s specification states:
    - The year begins on the **autumnal equinox** (the day of the autumnal equinox in Paris),
    - A leap year should occur every four years.
- **However:**  
  Both rules cannot be true for all time. The actual autumnal equinox drifts with respect to the Gregorian calendar, so a fixed arithmetic leap rule and a purely astronomical equinox rule will eventually diverge.
- **During its actual usage (Revolutionary France):**
    - The produced year boundaries and leap years did, in practice, align with both the equinox and the “every four years” scheme, so the rules matched historical documents.
    - There is strong evidence that starting the year on the actual autumnal equinox was intentional and regarded as important by the designers.
    - Some purely arithmetic (non-astronomical) leap year rules were proposed but, as far as is known, **not implemented during the calendar's active use**.
- **In hics:**  
  The base French Republican calendar follows the **autumnal equinox as year start** as the foundation, to match historical intent.  
  Alternative purely arithmetic versions may be added later as separate base calendars or options.

- **After/before this period:**  
  If you use the calendar algorithm beyond the years of the Revolution, it will still produce a result, but that date may be a mathematical artifact rather than a date anyone recognized at the time.

### Practical Guidance

- The algorithms in hics are designed for consistency and generality.
- For real historical research, always note the **range of actual usage** for each calendar scheme.
- Explicitly document or code your schemes' periods of validity if historical fidelity is important.
- For periods outside the span of real use, treat the calendar output as a **proleptic extension**—mathematically convenient, but not historically precise.
- Alternative scheme definitions (for proposed or hypothetical reforms) can be added as needed to hics.

---

**Summary:**
- hics supports any date input, but only guarantees historical/semantic accuracy within the period the calendar was in use.
- The French Republican calendar uses the autumnal equinox as year start in hics, matching historical practice—though arithmetic alternatives can be implemented in future.
- Mathematical results outside this period are well-defined, but may lack real-world meaning.
- When working with any historical calendar, always check whether calculated results are meaningful for your period and region of interest.

---