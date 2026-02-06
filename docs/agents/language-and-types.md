# Language and Types

- Standard: C++20 strictly required
- Encoding: UTF-8
- Exceptions: define custom exceptions derived from std::exception
- Typedef: use using instead of typedef
- Smart pointers: prefer std::unique_ptr, std::shared_ptr
- Enum classes preferred over C-style enums
- Avoid Hungarian notation
- Avoid global/static variables (threading issues)
- Prefer C++20 types over Boost counterparts
- Avoid Boost libraries requiring linking
