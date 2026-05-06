include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-intermediate
    SRC test_intermediate.cpp dummyVariable.h
  LIBS antares-solver-variable)

add_boost_test(test-surveyresults
  SRC
    test_surveyresults.cpp
    test_digest_export.cpp
    test_digest_with_variables.cpp
    test_reset_values.cpp
    surveyresults_test_helpers.h
  LIBS
  antares-solver-variable
  result_writer)

add_boost_test(test-residual-load
  SRC test_residual_load.cpp
  LIBS
  antares-solver-variable)

add_boost_test(test-migrated-variables-metadata
  SRC test_migrated_variables_metadata.cpp
  LIBS
  antares-solver-variable)

add_boost_test(test-dynamic-aggregation
    SRC test_dynamic.cpp
    LIBS
    antares-solver-variable
    in-memory-study)

add_boost_test(test-dynamic-economy-traits
    SRC test_dynamic_economy_traits.cpp
    LIBS
    antares-solver-variable
    in-memory-study)

add_boost_test(test-setofareas-reports
    SRC test_setofareas_reports.cpp
    LIBS
    antares-solver-variable
    result_writer)
