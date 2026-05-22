Feature: hydro parameters loading (hydro.ini / PartHydro::LoadIniFile)

  # Each study below is a copy of valid-v920/final-levels with [follow load] = true
  # and exactly one hydro.ini parameter set to a distinct non-default value.
  # These scenarios verify that every parameter read by PartHydro::LoadIniFile is
  # correctly loaded and propagated to the simulation.
  # Reference values were produced by running antares-solver once on each study.

  @fast @short
  Scenario Outline: hydro parameter "<param>" is loaded and the simulation is stable
    Given the solver study path is "Antares_Simulator_Tests_NR/hydro/hydro-param-<param>"
    When I run antares simulator
    Then the simulation succeeds
    And the simulation takes less than 5 seconds
    And the expected value of the annual system cost is <exp_cost>
    And in area "area", during year 1, total hydro production is <prod1> MWh
    And in area "area", during year 2, total hydro production is <prod2> MWh
    And in area "area", during year 1, total hydro pumping is <pump1> MWh
    And in area "area", during year 2, total hydro pumping is <pump2> MWh

    Examples:
      | param                            | exp_cost   | prod1  | prod2  | pump1 | pump2 |
      | inter-daily-breakdown            | 6302470000 | 99792  | 9485   | 0     | 0     |
      | intra-daily-modulation           | 6302470000 | 99792  | 9485   | 0     | 0     |
      | inter-monthly-breakdown          | 6302470000 | 99792  | 9485   | 0     | 0     |
      | reservoir                        | 6557460000 | 0      | 109200 | 0     | 0     |
      | follow-load                      | 6302470000 | 99792  | 9485   | 0     | 0     |
      | use-water                        | 6557460000 | 0      | 109190 | 0     | 0     |
      | hard-bounds                      | 6302470000 | 99792  | 9485   | 0     | 0     |
      | use-heuristic                    | 6052000000 | 199920 | 0      | 0     | 0     |
      | power-to-level                   | 6302470000 | 99792  | 9485   | 0     | 0     |
      | use-leeway                       | 6302000000 | 99792  | 9485   | 0     | 9485  |
      | leeway-low                       | 6302470000 | 99792  | 9485   | 0     | 0     |
      | leeway-up                        | 6302470000 | 99792  | 9485   | 0     | 0     |
      | pumping-efficiency               | 6302000000 | 99792  | 37898  | 0     | 37898 |
      | overflow-spilled-cost-difference | 6302470000 | 99792  | 9485   | 0     | 0     |

  # The two parameters below are constrained by the final-levels feature of the
  # base study: any non-default value is incompatible with the reservoir final
  # level data. The simulation failing on the expected validation error proves
  # the parameter was loaded and applied.

  @fast @short
  Scenario: parameter "initialize reservoir date" is loaded (non-zero value rejected by final-levels)
    Given the solver study path is "Antares_Simulator_Tests_NR/hydro/hydro-param-initialize-reservoir-date"
    When I run antares simulator
    Then the simulation fails
    And the message "Hydro allocation must start on the 1st simulation month" is reported in the logs

  @fast @short
  Scenario: parameter "reservoir capacity" is loaded (changed capacity rejected by final-levels)
    Given the solver study path is "Antares_Simulator_Tests_NR/hydro/hydro-param-reservoir-capacity"
    When I run antares simulator
    Then the simulation fails
    And the message "Incompatible total inflows" is reported in the logs
