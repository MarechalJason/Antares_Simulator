Feature: reserves tests

@fast @short
# Lot 1 : Intégration de la participation du thermique allumé à des réserves à la hausse et baisse
  Scenario: lot_1_disabled
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/lot_1_disabled"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 100 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And the annual system cost is 6.7872e+07

@fast @short
# Lot 1
  Scenario: lot_1_simple_up
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/lot_1_simple_up"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 80 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_1", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_1", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_1", reserve participation power is always equal to 0 MWh
    And the annual system cost is 7.04995e+07

@fast @short
# Lot 1
  Scenario: lot_1_simple_down
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/lot_1_simple_down"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 100 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_1", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_1", reserve participation power is always equal to 0 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_1", reserve participation power is always equal to 0 MWh
    And the annual system cost is 7.08994e+07

@fast @short
# Lot 1
  Scenario: lot_1_simple_up_and_down
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/lot_1_simple_up_and_down"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, hourly production of "thermal_all_cheap" is always equal to 80 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_prod" is always equal to 0 MWh
    And in area "AREA", during year 1, hourly production of "thermal_expensive_res_part" is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_up", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_up", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_up", reserve participation power is always equal to 0 MWh
    And in area "AREA", during year 1, for cluster "thermal_all_cheap" and reserve "Res_down", reserve participation power is always equal to 20 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_prod" and reserve "Res_down", reserve participation power is always equal to 0 MWh
    And in area "AREA", during year 1, for cluster "thermal_expensive_res_part" and reserve "Res_down", reserve participation power is always equal to 0 MWh
    And the annual system cost is 7.35269e+07

@fast @short
# Lot 2 : Intégration de la participation du thermique éteint et des stockage CT et LT
  Scenario: ST_4_off_cluster_participation
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_4_reserves"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", total reserve participation power is 0 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", participation of off units to the reserve is always equal to 40 MWh
    # OV. Cost = 20 *50+1*40+500*50+3000*10 euros (Prod cluster + Surcoûts réserves + défaillance EOD + défaillance réserves)
    And in area "AREA", overall cost on "1 JAN 06:00" of year 1 is of 56040 Euro 
    And the annual system cost is 9.41472e+06

@fast @short
# Lot 2
  Scenario: ST_5_off_cluster_participation_multiple_res
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_5_reserves"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", total reserve participation power is 0 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_2", total reserve participation power is 0 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_1", participation of off units to the reserve is always equal to 30 MWh
    And in area "AREA", during year 1, for cluster "thermal1" and reserve "Res_2", participation of off units to the reserve is always equal to 20 MWh
    And in area "AREA", during year 1, for reserve "Res_1", reserve unsupplied power is always equal to 10 MWh
    And in area "AREA", during year 1, for reserve "Res_2", reserve unsupplied power is always equal to 0 MWh
    And in area "AREA", overall cost on "1 JAN 06:00" of year 1 is of 80050 Euro 
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 100 MW
    And the annual system cost is 1.34484e+07

@fast @short
# Lot 3 : intégration des contraintes de stock en puissance et en énergie pour les stocks CT et LT
  Scenario: ST_1UP_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test5
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test6
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test7
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test8
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3
  Scenario: ST_1UP_reserves_test9
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh


@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
  	When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
	  When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test5
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test6
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test7
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test8
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3
  Scenario: ST_1DOWN_reserves_test9
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_1_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_down.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_1_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
	  And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh
	
@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_2_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 50 MWh
	
@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_3_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 50 MWh
	
@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_4_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", the sum over two hours of reserve participation power is always equal to 100 MWh
	
@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test5
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_5_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 0 MWh

@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test6
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_6_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 0 MWh

@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test7
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_7_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test8
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_8_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh

@fast @short
# Lot 3
  Scenario: ST_2UP_reserves_test9
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_2_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_9_double_up.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 50 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", total reserve participation power is 25200 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_2", reserve participation power is always equal to 150 MWh

@fast @short
# Lot 3
  Scenario: ST_3UP_unavailable_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_1.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33400 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And the annual system cost is 3.521e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3
  Scenario: ST_3UP_unavailable_reserves_test2
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_2.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 32800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 3.572e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3
  Scenario: ST_3UP_unavailable_reserves_test3
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_3.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 32800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 3.572e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3
  Scenario: ST_3UP_unavailable_reserves_test4
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/ST_3_reserves"
    When I replace the "input/reserves/area/reserves.ini" file with "../reserves_ini_files/reserves_unavailable_prod_4.ini"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 6.686e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
# Lot 3
  Scenario: LT_1_up_down_reserves_test1
    Given the solver study path is "Antares_Simulator_Tests_NR/reserves_tests/LT_1_reserves"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 20 seconds
    And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_1", total reserve participation power is 1008 MWh
	And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_2", total reserve participation power is 1680 MWh
    And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 6 MWh
	And in area "AREA", during year 1, for cluster "Hydro" and reserve "Res_2", on "1 JAN 06:00", reserve participation power is 10 MWh
    And the annual system cost is 2.15084e+06
