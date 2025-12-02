include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# ====================================
# Tests on Reserves Functions
# ====================================
add_boost_test(test_study_reserves
  SRC test_study_reserves.cpp
  LIBS Antares::study
  test_utils_unit)

add_boost_test(test_symmetries
  SRC test_symmetries.cpp
  LIBS Antares::study
  test_utils_unit)
