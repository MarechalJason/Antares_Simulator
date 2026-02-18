# Error Handling and Logging

```cpp
// Logging levels - Use Antares::logs
Antares::logs.fatal()   // DON'T USE (use error instead)
Antares::logs.error()   // Errors causing simulation to stop
Antares::logs.warning() // Non-critical issues, simulation continues
Antares::logs.info()    // Information for users
Antares::logs.debug()   // Debugging info for developers
```

Define custom exceptions derived from std::exception. Do not use logs.fatal(); use logs.error() instead.
