// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test hydro common

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <antares/study/study.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

// =================
// The fixture
// =================
struct CommonFixture
{
    std::filesystem::path path;
    StudyLoadOptions options;
    StudyVersion version = StudyVersion::latest();

    CommonFixture():
        tmp(fs::temp_directory_path()),
        hydroIni(tmp / "hydro.ini"),
        study(std::make_unique<Study>())
    {
        east = new Area("east"); // freed by ~AreaList
        study->areas.add(east);
        west = new Area("west"); // freed by ~AreaList
        study->areas.add(west);
    }

    bool load()
    {
        return east->hydro.LoadIniFile(*study, tmp) && west->hydro.LoadIniFile(*study, tmp);
    }

    ~CommonFixture()
    {
        fs::remove(hydroIni);
    }

    void writeValidFile();
    void writeInvalidFile();
    void writeIni(const std::string& contents);

public:
    Area* east;
    Area* west;

private:
    fs::path tmp;
    fs::path hydroIni;
    std::unique_ptr<Study> study;
};

void CommonFixture::writeValidFile()
{
    std::ofstream outfile(hydroIni);
    outfile <<
      R"([overflow spilled cost difference]
east = 1.00000
west = 2.31000

[reservoir]
east = true)";
}

void CommonFixture::writeInvalidFile()
{
    std::ofstream outfile(hydroIni);
    outfile <<
      R"([overflow spilled cost difference]
east = 1.00000
west = 2.31000
wrongarea = 1.414

[reservoir]
east = true)";
}

void CommonFixture::writeIni(const std::string& contents)
{
    std::ofstream outfile(hydroIni);
    outfile << contents;
}

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(test_read_valid_file, CommonFixture)
{
    writeValidFile();
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.overflowSpilledCostDifference, 1.0);
    BOOST_CHECK_EQUAL(west->hydro.overflowSpilledCostDifference, 2.31000);
}

BOOST_FIXTURE_TEST_CASE(test_read_invalid_file, CommonFixture)
{
    writeInvalidFile();
    BOOST_CHECK(!load());
}

// One case per hydro.ini section read by PartHydro::LoadIniFile.
// Each value differs from the PartHydro() constructor default so that removing
// the corresponding `ini.find(...)` block in container.cpp makes the case fail.

BOOST_FIXTURE_TEST_CASE(loads_inter_daily_breakdown, CommonFixture)
{
    writeIni("[inter-daily-breakdown]\neast = 3.5\nwest = 4.25\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_CLOSE(east->hydro.interDailyBreakdown, 3.5, 1e-9);
    BOOST_CHECK_CLOSE(west->hydro.interDailyBreakdown, 4.25, 1e-9);
}

BOOST_FIXTURE_TEST_CASE(loads_intra_daily_modulation, CommonFixture)
{
    writeIni("[intra-daily-modulation]\neast = 7.25\nwest = 12.5\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_CLOSE(east->hydro.intraDailyModulation, 7.25, 1e-9);
    BOOST_CHECK_CLOSE(west->hydro.intraDailyModulation, 12.5, 1e-9);
}

BOOST_FIXTURE_TEST_CASE(loads_inter_monthly_breakdown, CommonFixture)
{
    writeIni("[inter-monthly-breakdown]\neast = 2.75\nwest = 5.5\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_CLOSE(east->hydro.intermonthlyBreakdown, 2.75, 1e-9);
    BOOST_CHECK_CLOSE(west->hydro.intermonthlyBreakdown, 5.5, 1e-9);
}

BOOST_FIXTURE_TEST_CASE(loads_reservoir_capacity, CommonFixture)
{
    writeIni("[reservoir capacity]\neast = 12345.0\nwest = 6789.5\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_CLOSE(east->hydro.reservoirCapacity, 12345.0, 1e-9);
    BOOST_CHECK_CLOSE(west->hydro.reservoirCapacity, 6789.5, 1e-9);
}

BOOST_FIXTURE_TEST_CASE(loads_follow_load, CommonFixture)
{
    // constructor default is true, so we set false to detect the loader
    writeIni("[follow load]\neast = false\nwest = false\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_EQUAL(east->hydro.followLoadModulations, false);
    BOOST_CHECK_EQUAL(west->hydro.followLoadModulations, false);
}

BOOST_FIXTURE_TEST_CASE(loads_use_water, CommonFixture)
{
    writeIni("[use water]\neast = true\nwest = true\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_EQUAL(east->hydro.useWaterValue, true);
    BOOST_CHECK_EQUAL(west->hydro.useWaterValue, true);
}

BOOST_FIXTURE_TEST_CASE(loads_hard_bounds, CommonFixture)
{
    writeIni("[hard bounds]\neast = true\nwest = true\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_EQUAL(east->hydro.hardBoundsOnRuleCurves, true);
    BOOST_CHECK_EQUAL(west->hydro.hardBoundsOnRuleCurves, true);
}

BOOST_FIXTURE_TEST_CASE(loads_use_heuristic, CommonFixture)
{
    // constructor default is true, so we set false to detect the loader
    writeIni("[use heuristic]\neast = false\nwest = false\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_EQUAL(east->hydro.useHeuristicTarget, false);
    BOOST_CHECK_EQUAL(west->hydro.useHeuristicTarget, false);
}

BOOST_FIXTURE_TEST_CASE(loads_power_to_level, CommonFixture)
{
    writeIni("[power to level]\neast = true\nwest = true\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_EQUAL(east->hydro.powerToLevel, true);
    BOOST_CHECK_EQUAL(west->hydro.powerToLevel, true);
}

BOOST_FIXTURE_TEST_CASE(loads_initialize_reservoir_date, CommonFixture)
{
    writeIni("[initialize reservoir date]\neast = 6\nwest = 9\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_EQUAL(east->hydro.initializeReservoirLevelDate, 6);
    BOOST_CHECK_EQUAL(west->hydro.initializeReservoirLevelDate, 9);
}

BOOST_FIXTURE_TEST_CASE(loads_use_leeway, CommonFixture)
{
    writeIni("[use leeway]\neast = true\nwest = true\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_EQUAL(east->hydro.useLeeway, true);
    BOOST_CHECK_EQUAL(west->hydro.useLeeway, true);
}

BOOST_FIXTURE_TEST_CASE(loads_leeway_low, CommonFixture)
{
    // constructor default is 1.0, so we set 0.4 / 0.6 to detect the loader
    writeIni("[leeway low]\neast = 0.4\nwest = 0.6\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_CLOSE(east->hydro.leewayLowerBound, 0.4, 1e-9);
    BOOST_CHECK_CLOSE(west->hydro.leewayLowerBound, 0.6, 1e-9);
}

BOOST_FIXTURE_TEST_CASE(loads_leeway_up, CommonFixture)
{
    // constructor default is 1.0, so we set values other than 1.0
    writeIni("[leeway up]\neast = 2.1\nwest = 3.3\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_CLOSE(east->hydro.leewayUpperBound, 2.1, 1e-9);
    BOOST_CHECK_CLOSE(west->hydro.leewayUpperBound, 3.3, 1e-9);
}

BOOST_FIXTURE_TEST_CASE(loads_pumping_efficiency, CommonFixture)
{
    // constructor default is 1.0, so we set values other than 1.0
    writeIni("[pumping efficiency]\neast = 0.85\nwest = 0.5\n");
    BOOST_REQUIRE(load());
    BOOST_CHECK_CLOSE(east->hydro.pumpingEfficiency, 0.85, 1e-9);
    BOOST_CHECK_CLOSE(west->hydro.pumpingEfficiency, 0.5, 1e-9);
}

BOOST_AUTO_TEST_SUITE_END()
