# Best Practices

- Mark non-derived classes as final
- Always check warnings on clang++, g++, MSVC
- Write unit tests covering new functionality (see examples in src/tests/libs/)
- Test on both Ubuntu and Windows platforms
- Update documentation when modifying documented features
- Use const and constexpr where appropriate
- Avoid one-line control flow statements for easier debugging
- Use // TODO(DeveloperName): need to fix it for incomplete work
- Avoid Yuni framework for new code (legacy)
