# C++23 Features Compatibility Tests

## Overview

This directory contains unit tests to verify that the Antares Simulator project fully supports C++23 standard features.
These tests ensure that the compiler and build system are correctly configured for modern C++ development.

## Test Coverage

### 1. std::expected

- **Purpose**: Modern error handling without exceptions
- **Tests**:
    - Success and error cases
    - `value_or()` for default values
    - `and_then()` for monadic composition
    - `or_else()` for error recovery
    - `transform()` for value transformation

**Example**:

```cpp
std::expected<FileContent, ErrorCode> readFile(std::string_view filename);

auto result = readFile("test.txt");
if (result.has_value()) {
    // Process successful read
}
```

### 2. C++23 Ranges Enhancements

- **ranges::to**: Convert ranges to containers directly
- **views::zip**: Combine multiple ranges element-wise
- **views::chunk**: Split ranges into fixed-size chunks
- **views::slide**: Create sliding windows over ranges

**Example**:

```cpp
auto evens = numbers 
    | views::filter([](int n) { return n % 2 == 0; })
    | ranges::to<std::vector>();
```

If the standard library does not provide `std::ranges::to` (missing `__cpp_lib_ranges_to`), the tests fall back to a
local `toVector()` helper that collects the range manually. This keeps the tests compiling while still exercising
`views::filter`, `views::chunk`, and `views::slide`.

### 3. String Enhancements

- **string::contains()**: Check substring presence
- **string_view::contains()**: Efficient substring checking

**Example**:

```cpp
std::string text = "Antares Simulator";
if (text.contains("Antares")) {
    // Found!
}
```

### 4. Optional Monadic Operations

- **optional::and_then()**: Chain optional operations
- **optional::transform()**: Transform contained value
- **optional::or_else()**: Provide fallback for empty optionals

**Example**:

```cpp
std::optional<int> value = 42;
auto doubled = value.transform([](int v) { return v * 2; });
```

### 5. Constexpr Enhancements

- Extended constexpr support for more complex compile-time computations
- Static assertions for compile-time validation

## Building and Running

### Build the test:

```bash
cd src
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --target test-cpp23-features
```

### Run the test:

```bash
./build/tests/src/libs/antares/cpp23/test-cpp23-features
```

### Run via CTest:

```bash
cd build
ctest -R cpp23-features -V
```

## Compiler Requirements

To successfully compile and run these tests, you need:

- **GCC 12+** (recommended GCC 13 or later for full C++23 support)
- **Clang 15+** (recommended Clang 16 or later)
- **MSVC 2022** (version 17.4 or later)

### Checking Your Compiler

```bash
# For GCC
g++ --version

# For Clang
clang++ --version

# Test C++23 support
echo '#include <expected>' | g++ -std=c++23 -x c++ -c - 2>&1
```

## Integration with CI/CD

These tests are automatically run as part of the unit test suite. They are tagged with:

- `cpp23`: For C++23 specific tests
- `compatibility`: For compiler/standard compatibility tests
- `unit`: Standard unit test category

## Troubleshooting

### Error: `expected: No such file or directory`

**Solution**: Your compiler doesn't support C++23. Upgrade to:

- GCC 13+
- Clang 16+
- MSVC 2022 17.4+

### Error: `ranges::to` is not a member of `std::ranges`

**Solution**: Some C++23 features require very recent compilers. Check your standard library version.

### Build succeeds but tests fail

**Solution**: Check that `CMAKE_CXX_STANDARD` is set to 23 in your CMakeLists.txt.

## Why C++23?

### Benefits for Antares Simulator:

1. **Better Error Handling**: `std::expected` provides clear error propagation without exceptions overhead
2. **Cleaner Code**: Ranges and views make data processing pipelines more readable
3. **Performance**: Many C++23 features enable better compiler optimizations
4. **Safety**: Stronger type checking and compile-time validation
5. **Productivity**: Less boilerplate code with monadic operations

## Future Enhancements

Additional C++23 features to test in future iterations:

- `std::print` and `std::println` (when compiler support is widespread)
- `std::mdspan` for multidimensional arrays
- Deducing `this` for explicit object parameters
- `if consteval` for compile-time context checking
- Extended floating-point types

## References

- [C++23 on cppreference.com](https://en.cppreference.com/w/cpp/23)
- [GCC C++23 Support](https://gcc.gnu.org/projects/cxx-status.html#cxx23)
- [Clang C++23 Status](https://clang.llvm.org/cxx_status.html#cxx23)
- [MSVC C++23 Features](https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance)

## Contributing

When adding new C++23 features to the codebase:

1. Add corresponding tests to this file
2. Document the feature usage in the test
3. Update this README with the new feature
4. Verify tests pass on all supported compilers

---

**Maintainer**: Antares Simulator Development Team  
**Last Updated**: 2026-02-12
