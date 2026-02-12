cmake_minimum_required(VERSION 3.14)

# Test for C++23 features support
add_executable(test-cpp23-features
        test-cpp23-features.cpp
)

target_link_libraries(test-cpp23-features
        PRIVATE
        Boost::unit_test_framework
)

# Ensure C++23 is enabled
set_target_properties(test-cpp23-features PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
)

#Print compiler version
message(STATUS "Testing C++23 features with compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# Add test to CTest
add_test(NAME cpp23-features COMMAND test-cpp23-features)

# Add labels for categorization
set_tests_properties(cpp23-features PROPERTIES
        LABELS "cpp23;compatibility;unit"
)

