# Code Style and Formatting

## Auto-formatting

```bash
cd src && ./format-code.sh
clang-format -i -style=file:../.clang-format file.cpp file.hpp
```

Reference clang-format version: 18.1.3

## Critical Formatting Rules

- 4-space indentation, no tabs
- 100 character line limit
- Allman brace style (braces on new lines)
- Braces always required for if/for/while/switch (even single-line)
- Space after keywords: `if (condition)`, `for (int i = 0; ...)`
- Space after semicolon in for loops
- Spaces between binary operators: `x = y * y + z * z`
- No spaces in parentheses, square brackets, or angles
- Left-aligned pointers/references: `const string& s`, `int* ptr`
- `#pragma once` for header guards
- `constexpr` for compile-time constants

## Naming Conventions

- Files: CamelCase.cpp, CamelCase.h (new files)
- Functions: lowerCamelCase() - `isPrime(unsigned int n)`
- Classes: CamelCase - `ComplexClass`
- Constants: kCamelCase - `constexpr int kBufferSize = 255`
- Enums: enum class with capital values - `enum class Color { RED, GREEN };`
- Namespaces: lowercase - `namespace lower_case {}`
- Macros: UPPER_CASE (avoid when possible)

## Include Ordering

1. Current directory header: "current_file.h"
2. Local includes (sorted by dependency: indexer, coding, base)
3. "defines.h"
4. C++ standard library headers: <vector>, <string>, etc.
5. Boost headers: <boost/...>
6. Yuni headers: <yuni/...> (avoid for new code)
7. Antares headers: "antares/..." or <antares/...>

Use blank lines between groups. Use #include "" for local headers, #include <> for system headers.
