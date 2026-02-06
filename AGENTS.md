# AGENTS.md - Guidelines for Coding Agents

Antares Simulator is an open-source power system simulator.

## Essentials

Package manager: vcpkg (C++ dependencies).

```bash
# Configure build (from src/ directory)
cmake -B ../build-debug -DCMAKE_BUILD_TYPE=Debug
cmake -B ../build-release -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build ../build-debug -j$(nproc)
cmake --build ../build-release -j$(nproc)

# Build specific target (e.g., solver)
cmake --build ../build-debug --target antares-solver

# Run all tests using ctest
cd build-debug && ctest --output-on-failure
cd build-debug && ctest -R "test_name" --output-on-failure

# Run specific Boost.Test binary (from build-debug directory)
./tests/unit_tests_antares/optimization_options_test

# Run Python pytest tests
cd src/tests/run-study-tests
python -m pytest -m json --antares-simu-path=<path_to_solver>
python -m pytest test_file.py::test_function_name

# Run single test via ctest with regex
ctest -R "OptimizationOptionsTests" --output-on-failure -V
```

## More guidance

- Code style and formatting: docs/agents/code-style.md
- Language and types: docs/agents/language-and-types.md
- Error handling and logging: docs/agents/error-handling-logging.md
- Best practices: docs/agents/best-practices.md
- Git workflow: docs/agents/git-workflow.md
- Key directories: docs/agents/key-directories.md
- Pull request guidelines: docs/agents/pull-requests.md
