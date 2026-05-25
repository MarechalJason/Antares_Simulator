Feature: hydro parameters loading (hydro.ini / PartHydro::LoadIniFile)

  # Each study below is a copy of valid-v920/final-levels with [follow load] = true,
  # a monthly-stepped load profile, and a single 500 MW / 50 €/MWh thermal cluster.
  # Exactly one hydro.ini parameter is set to a distinct non-default value per study.
  # Reference values were produced by running antares-solver once on each study.
  #
  # Note: parameters that participate only in soft hydro heuristics (the four
  # breakdown / modulation parameters, follow-load, hard-bounds, leeway-low/up,
  # power-to-level, overflow-spilled-cost-difference) do not change the simulation
  # output in this study setup because the final-levels feature pins start/end
  # reservoir levels and the optimizer has enough slack to ignore the soft targets.
  # The dedicated unit test in test-hydro-common.cpp covers loader regressions for
  # *every* section; the scenarios below verify end-to-end propagation where it
  # is observable.

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
      | param                            | exp_cost  | prod1  | prod2  | pump1 | pump2 |
      | inter-daily-breakdown            | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | intra-daily-modulation           | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | inter-monthly-breakdown          | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | reservoir                        | 106830000 | 0      | 109200 | 0     | 0     |
      | follow-load                      | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | use-water                        | 106827000 | 0      | 114196 | 0     | 4881  |
      | hard-bounds                      | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | use-heuristic                    | 100830000 | 236100 | 155026 | 36100 | 5850  |
      | power-to-level                   | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | use-leeway                       | 106823000 | 100001 | 9488   | 0     | 0     |
      | leeway-low                       | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | leeway-up                        | 106823000 | 138751 | 45158  | 38750 | 35670 |
      | pumping-efficiency               | 106823000 | 100001 | 9488   | 0     | 0     |
      | overflow-spilled-cost-difference | 106823000 | 138751 | 45158  | 38750 | 35670 |

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
