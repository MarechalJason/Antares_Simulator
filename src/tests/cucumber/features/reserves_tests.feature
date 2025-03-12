Feature: reserves tests

@fast @short
  Scenario: ST_1UP_reserves_test1
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh


@fast @short
  Scenario: ST_1UP_reserves_test2
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test3
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test4
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1UP_reserves_test5
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test6
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test7
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test8
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1UP_reserves_test9
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test1
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh


@fast @short
  Scenario: ST_1DOWN_reserves_test2
    Given the study path is "short-tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test3
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test4
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test5
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test6
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test7
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test8
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test9
    Given the study path is "short-tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_down.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_2UP_reserves_test1
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
	  And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh
	
@fast @short
  Scenario: ST_2UP_reserves_test2
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
	  And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 50 MWh
	
@fast @short
  Scenario: ST_2UP_reserves_test3
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
	  And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 50 MWh
	
@fast @short
  Scenario: ST_2UP_reserves_test4
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
	  And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", the sum over two hours of reserve participation power is always equal to 100 MWh
	
@fast @short
  Scenario: ST_2UP_reserves_test5
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 0 MWh

@fast @short
  Scenario: ST_2UP_reserves_test6
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 0 MWh

@fast @short
  Scenario: ST_2UP_reserves_test7
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_2UP_reserves_test8
    Given the study path is "short-tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
	And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh

@fast @short
  Scenario: ST_2UP_reserves_test9
    Given the study path is "short-tests/ST_2_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_double_up.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
	And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh

@fast @short
  Scenario: ST_3UP_unavailable_reserves_test1
    Given the study path is "short-tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_1.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33400 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And the annual system cost is 3.521e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
  Scenario: ST_3UP_unavailable_reserves_test2
    Given the study path is "short-tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_2.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 32800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 3.572e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
  Scenario: ST_3UP_unavailable_reserves_test3
    Given the study path is "short-tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_3.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 32800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 3.572e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
  Scenario: ST_3UP_unavailable_reserves_test4
    Given the study path is "short-tests/ST_3_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_4.ini"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 6.686e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours