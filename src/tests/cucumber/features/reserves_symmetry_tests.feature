Feature: reserves symmetry tests

  Scenario: Thermal_cluster_symmetry_test_1
    Given the study path is "short-tests/symmetry_test_1"
    When I run antares simulator with "xpress" as solver
    Then the simulation takes less than 60 seconds
    And the simulation succeeds
    And in area "FRANCE", during year 1, for cluster "therm" and reserve "res_1_up", reserve participation power is always equal to 40 MWh
    And in area "FRANCE", during year 1, for cluster "therm" and reserve "res_1_down", reserve participation power is always equal to 20 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_up", reserve unsupplied power is always equal to 10 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_down", reserve unsupplied power is always equal to 30 MWh
    And in area "FRANCE", during year 1, hourly production of "therm" is always equal to 80 MWh
    And in area "FRANCE", unsupplied energy on "2 JAN 09:00" of year 1 is of 40 MW

  Scenario: ST_symmetry_test_2
    Given the study path is "short-tests/symmetry_test_2"
    When I run antares simulator with "xpress" as solver
    Then the simulation takes less than 60 seconds
    And the simulation succeeds
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "res_1_up", reserve participation power is always equal to 44 MWh
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "res_1_down", reserve participation power is always equal to 36 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_up", reserve unsupplied power is always equal to 6 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_down", reserve unsupplied power is always equal to 14 MWh
    And in area "FRANCE", unsupplied energy on "2 JAN 09:00" of year 1 is of 100 MW

  Scenario: LT_symmetry_test_3
    Given the study path is "short-tests/symmetry_test_3"
    When I run antares simulator with "xpress" as solver
    Then the simulation takes less than 60 seconds
    And the simulation succeeds
    And in area "FRANCE", during year 1, for cluster "LongTermStorage" and reserve "res_1_up", reserve participation power is always equal to 44 MWh
    And in area "FRANCE", during year 1, for cluster "LongTermStorage" and reserve "res_1_down", reserve participation power is always equal to 36 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_up", reserve unsupplied power is always equal to 6 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_down", reserve unsupplied power is always equal to 14 MWh
    And in area "FRANCE", unsupplied energy on "2 JAN 09:00" of year 1 is of 100 MW