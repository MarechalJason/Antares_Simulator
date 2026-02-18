# AGENTS.md

Antares Simulator is an open-source power system simulator.

## Essentials

Package manager: vcpkg (C++ dependencies).
Antares Simulator: open-source power system planning tool for long-term adequacy and operational studies.

## Build & Test

**Configure and build** (from `src/` directory):
```bash
cmake -B ../build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build ../build-debug -j$(nproc)
cmake --build ../build-release -j$(nproc)

# Build specific target (e.g., solver)
cmake --build ../build-debug --target antares-solver

# Run all tests using ctest
cmake --build ../build-debug --target antares-solver  # Specific target
```

**Run tests:**
```bash
cd build-debug && ctest --output-on-failure
ctest -R "test_name" --output-on-failure              # Specific test

# Python pytest (from src/tests/run-study-tests)
python -m pytest -m json --antares-simu-path=<path_to_solver>
```

## More guidance

- Code style and formatting: docs/agents/code-style.md
- Language and types: docs/agents/language-and-types.md
- Error handling and logging: docs/agents/error-handling-logging.md
- Best practices: docs/agents/best-practices.md
- Git workflow: docs/agents/git-workflow.md
- Key directories: docs/agents/key-directories.md
- Pull request guidelines: docs/agents/pull-requests.md
- Access control (do not read `src/ui`): docs/agents/access-control.md
**Format code:**
```bash
cd src && ./format-code.sh
```

## Detailed Guidelines

- **[Code Style & Formatting](.github/agents/code-style.md)** - Naming conventions, indentation, includes
- **[C++ Guidelines](.github/agents/cpp-guidelines.md)** - Language standards, types, best practices
- **[Error Handling & Logging](.github/agents/error-handling.md)** - Exception handling, log levels
- **[Git Workflow](.github/agents/git-workflow.md)** - Branch naming, PR requirements
- **[Project Structure](.github/agents/project-structure.md)** - Key directories

See also: [Contributing Guide](docs/developer-guide/6-Contributing.md)
