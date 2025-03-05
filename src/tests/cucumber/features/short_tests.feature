Feature: short tests


  @fast @short
  Scenario: 001 One node - passive
    Given the study path is "short-tests/001 One node - passive"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 0

  @fast @short
  Scenario: 002 Thermal fleet - Base
    Given the study path is "short-tests/002 Thermal fleet - Base"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.729e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW

  @fast @short
  Scenario: 003 Thermal fleet - Must-run
    Given the study path is "short-tests/003 Thermal fleet - Must-run"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.751e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    And in area "AREA", during year 1, hourly production of "non-dispatchable semi base" is always equal to 300 MWh

  @fast @short
  Scenario: 004 Thermal fleet - Partial must-run
    Given the study path is "short-tests/004 Thermal fleet - Partial must-run"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.751e+7
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    And in area "AREA", during year 1, hourly production of "semi base" is always greater than 300 MWh

  @fast @short
  Scenario: 005 Thermal fleet - Minimum stable power and min up down times
    Given the study path is "short-tests/005 Thermal fleet - Minimum stable power and min up down times"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.75816e+07
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 006 Thermal fleet - Extra costs
    # Like previous test, but with extra non-proportional (NP) costs
    # NP costs = 1756400 ; OP costs = 2.75816e+07 (like test 5) => Total cost = 2.9338e+07
    Given the study path is "short-tests/006 Thermal fleet - Extra costs"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.9338e+07
    And in area "AREA", during year 1, total non-proportional cost is 1756400
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 007 Thermal fleet - Fast unit commitment
    # This example is the first of a set of two that are comparing the two unit-commitment modes of Antares.
    # Fast mode
    # => overall cost is not great, there are a lot of startups, and min up & down time are considered equal
    Given the study path is "short-tests/007 Thermal fleet - Fast unit commitment"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.98912e+07
    And in area "AREA", during year 1, total non-proportional cost is 1861400
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 12 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 008 Thermal fleet - Accurate unit commitment
    # Like previous test, but with unit commitment
    # => overall cost is better, there are less startups, and min up & down time are not equal
    Given the study path is "short-tests/008 Thermal fleet - Accurate unit commitment"
    When I run antares simulator
    Then the simulation takes less than 5 seconds
    And the simulation succeeds
    And the annual system cost is 2.97339e+07
    And in area "AREA", during year 1, total non-proportional cost is 1680900
    And in area "AREA", the units of "base" produce between 400 and 900 MWh hourly
    And in area "AREA", the units of "semi base" produce between 100 and 300 MWh hourly
    And in area "AREA", the units of "peak" produce between 10 and 100 MWh hourly
    And in area "AREA", during year 1, loss of load lasts 1 hours
    And in area "AREA", unsupplied energy on "2 JAN 09:00" of year 1 is of 52 MW
    # Ideally, we would also check min up & down durations in this test. But is not possible, since clusters defined
    # in this test have a unitcount > 1
    # TODO : create similar tests with unitcount = 1, and implement the following steps:
    # And in area "AREA", unit "base" respects a minimum up duration of 24 hours, and a minimum down duration of 24 hours
    # And in area "AREA", unit "semi base" respects a minimum up duration of 6 hours, and a minimum down duration of 12 hours
    # And in area "AREA", unit "peak" respects a minimum up duration of 2 hours, and a minimum down duration of 2 hours

  @fast @short
  Scenario: 021 Four areas - DC law
    Given the study path is "short-tests/021 Four areas - DC law"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And the annual system cost is
      | EXP       | STD       | MIN       | MAX       |
      | 7.972e+10 | 2.258e+10 | 5.613e+10 | 1.082e+11 |

@fast @short
  Scenario: ST_1UP_reserves_test1
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_1_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh


@fast @short
  Scenario: ST_1UP_reserves_test2
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_2_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test3
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_3_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test4
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_4_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1UP_reserves_test5
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_5_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test6
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_6_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test7
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_7_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1UP_reserves_test8
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_8_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1UP_reserves_test9
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_9_up"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test1
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_1_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh


@fast @short
  Scenario: ST_1DOWN_reserves_test2
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_2_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test3
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_3_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test4
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_4_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16800 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test5
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_5_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test6
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_6_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test7
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_7_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 8400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", the sum over two hours of reserve participation power is always equal to 100 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test8
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_8_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh

@fast @short
  Scenario: ST_1DOWN_reserves_test9
    Given the study path is "short-tests/ST_1_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_9_down"
    When I run antares simulator
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33600 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", reserve participation power is always equal to 200 MWh


@fast @short
  Scenario: ST_3UP_unavailable_reserves_test1
    Given the study path is "short-tests/ST_3_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_unavailable_prod_1"
    When I run antares simulator with "coin" as solver
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 33400 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And the annual system cost is 3.521e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours

@fast @short
  Scenario: ST_3UP_unavailable_reserves_test2
    Given the study path is "short-tests/ST_3_reserves"
    When I configure the reserve.ini file in area "AREA" for test "reserves_unavailable_prod_2"
    When I run antares simulator with "coin" as solver
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
    When I configure the reserve.ini file in area "AREA" for test "reserves_unavailable_prod_3"
    When I run antares simulator with "coin" as solver
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
    When I configure the reserve.ini file in area "AREA" for test "reserves_unavailable_prod_4"
    When I run antares simulator with "coin" as solver
    Then the simulation takes less than 20 seconds
    And the simulation succeeds
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", total reserve participation power is 16400 MWh
    And in area "AREA", during year 1, for cluster "st1" and reserve "Res_1", on "1 JAN 06:00", reserve participation power is 0 MWh
    And in area "AREA", battery level on "1 JAN 06:00" of year 1 is 0 MWh
    And in area "AREA", battery level on "1 JAN 10:00" of year 1 is 200 MWh
    And the annual system cost is 6.686e+06
    And in area "AREA", during year 1, loss of load lasts 3 hours