Feature: reserves symmetry tests

  Scenario: Thermal_cluster_symmetry_test_1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/symmetry_test_1"
    When I run antares simulator 
    Then the simulation takes less than 60 seconds
    And the simulation succeeds
    And in area "FRANCE", during year 1, for cluster "therm" and reserve "res_1_up", reserve participation power is always equal to 40 MWh
    And in area "FRANCE", during year 1, for cluster "therm" and reserve "res_1_down", reserve participation power is always equal to 20 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_up", reserve unsupplied power is always equal to 10 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_down", reserve unsupplied power is always equal to 30 MWh
    And in area "FRANCE", during year 1, hourly production of "therm" is always equal to 60 MWh
    And in area "FRANCE", unsupplied energy on "2 JAN 09:00" of year 1 is of 40 MW

  Scenario: ST_symmetry_test_2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/symmetry_test_2"
    When I run antares simulator
    Then the simulation takes less than 60 seconds
    And the simulation succeeds
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "res_1_up", reserve participation power is always equal to 44 MWh
    And in area "FRANCE", during year 1, for cluster "st1" and reserve "res_1_down", reserve participation power is always equal to 36 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_up", reserve unsupplied power is always equal to 6 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_down", reserve unsupplied power is always equal to 14 MWh
    And in area "FRANCE", unsupplied energy on "2 JAN 09:00" of year 1 is of 100 MW
	And in area "FRANCE", on "2 JAN 09:00" of year 1, storage injection for cluster "st1" is of 24 MW
	And in area "FRANCE", on "2 JAN 09:00" of year 1, storage withdrawal for cluster "st1" is of 24 MW

  Scenario: LT_symmetry_test_3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/symmetry_test_3"
    When I run antares simulator
    Then the simulation takes less than 60 seconds
    And the simulation succeeds
    And in area "FRANCE", during year 1, for cluster "LongTermStorage" and reserve "res_1_up", reserve participation power is always equal to 44 MWh
    And in area "FRANCE", during year 1, for cluster "LongTermStorage" and reserve "res_1_down", reserve participation power is always equal to 36 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_up", reserve unsupplied power is always equal to 6 MWh
    And in area "FRANCE", during year 1, for reserve "res_1_down", reserve unsupplied power is always equal to 14 MWh
    And in area "FRANCE", unsupplied energy on "2 JAN 09:00" of year 1 is of 44 MW
    And in area "FRANCE", on "2 JAN 09:00" of year 1, hydro storage injection is of 80 MWh
	And in area "FRANCE", on "2 JAN 09:00" of year 1, hydro storage pumping is of 24 MWh
