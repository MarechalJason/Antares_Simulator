// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test survey results"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/variable/categories.h"
#include "antares/solver/variable/surveyresults.h"
#include "antares/writer/in_memory_writer.h"

using namespace Antares::Solver::Variable;

namespace
{

struct StudyFixture
{
    StudyFixture():
        study(std::make_unique<Data::Study>())
    {
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->initializeRuntimeInfos();
    }

    std::unique_ptr<Data::Study> study;
};

struct StudyFixtureWithArea
{
    StudyFixtureWithArea():
        study(std::make_unique<Data::Study>())
    {
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        auto* area = study->areaAdd("area1");
        area->index = 0;
        study->initializeRuntimeInfos();
    }

    std::unique_ptr<Data::Study> study;
};

struct StudyFixtureWithTwoAreas
{
    StudyFixtureWithTwoAreas():
        study(std::make_unique<Data::Study>())
    {
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        auto* area1 = study->areaAdd("area1");
        area1->index = 0;
        auto* area2 = study->areaAdd("area2");
        area2->index = 1;
        study->initializeRuntimeInfos();
    }

    std::unique_ptr<Data::Study> study;
};

} // namespace

// ---------------------------------------------------------------------------
// saveToFile
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(surveyresults)

BOOST_FIXTURE_TEST_CASE(survey_result_hourly, StudyFixture)
{
    Benchmarking::DurationCollector durationCollector;
    Solver::InMemoryWriter writer(durationCollector);

    unsigned int nbVariables = 2;
    SurveyResults survey(*study, nbVariables, "out", writer);

    survey.data.columnIndex = nbVariables;
    survey.captions[0][0] = "hello";
    survey.captions[1][0] = "unit";
    survey.captions[2][0] = "N/A";
    survey.captions[0][1] = "world";
    survey.captions[1][1] = "unit2";
    survey.captions[2][1] = "N/A";

    for (unsigned int h = 0; h < HOURS_PER_YEAR; ++h)
    {
        survey.values[0][h] = h + 1;
        survey.values[1][h] = h * h + 3;
    }

    const std::string filename = "hourly.txt";
    survey.data.filename = filename;
    survey.saveToFile(Category::DataLevel::setOfAreas,
                      Category::FileLevel::va,
                      Category::Precision::hourly);

    BOOST_REQUIRE(writer.getMap().contains(filename));
    const auto& content = writer.getMap().at(filename);
    // Header
    BOOST_CHECK_NE(content.find("system	hourly				hello	world\n"), std::string::npos);
    // Some values
    BOOST_CHECK_NE(content.find("	9	01	JAN	08:00	9	67\n"), std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

// ---------------------------------------------------------------------------
// exportDigestAllYears
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(digest)

BOOST_FIXTURE_TEST_CASE(header_contains_digest_label_and_counts_section, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto & caption : survey.captions)
        caption[0] = "V";

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tdigest\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\tVARIABLES\tAREAS\tLINKS\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(header_counts_3_variables_0_areas_0_links, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 3u, "out", writer);
    survey.data.columnIndex = 3;
    for (auto & caption : survey.captions)
        caption[0] = caption[1] = caption[2] = "x";

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\t3\t0\t0\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(three_caption_rows_are_written_for_each_column, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 2u, "out", writer);
    survey.data.columnIndex = 2;
    survey.captions[0][0] = "LOAD";  survey.captions[0][1] = "WIND";
    survey.captions[1][0] = "MW";    survey.captions[1][1] = "MW";
    survey.captions[2][0] = "EXP";   survey.captions[2][1] = "std";

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\t\tLOAD\tWIND\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t\tMW\tMW\n"),     std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t\tEXP\tstd\n"),   std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(row_with_zero_value_writes_tab_zero, StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto & caption : survey.captions)
        caption[0] = "V";

    survey.data.rowCaptions.emplace_back("area1");
    survey.values[0][0] = 0.0;

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tarea1\t0\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(row_with_nonzero_value_writes_formatted_value, StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto & caption : survey.captions)
        caption[0] = "V";
    survey.precision[0] = "%.0f";

    survey.data.rowCaptions.push_back("area1");
    survey.values[0][0] = 123.0;

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tarea1\t123\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(non_applicable_column_writes_NA, StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto & caption : survey.captions)
        caption[0] = "V";

    survey.data.rowCaptions.emplace_back("area1");
    survey.values[0][0] = 99.0;
    survey.digestNonApplicableStatus[0][0] = true;

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tarea1\tN/A\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(multiple_areas_write_one_row_each, StudyFixtureWithTwoAreas)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto & caption : survey.captions)
        caption[0] = "V";
    survey.precision[0] = "%.0f";

    survey.data.rowCaptions.emplace_back("area1");
    survey.data.rowCaptions.emplace_back("area2");
    survey.values[0][0] = 10.0;
    survey.values[0][1] = 20.0;

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tarea1\t10\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\tarea2\t20\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(mixed_zero_nonzero_na_columns_in_same_row, StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 3u, "out", writer);
    survey.data.columnIndex = 3;
    for (auto & caption : survey.captions)
    {
        caption[0] = "A";
        caption[1] = "B";
        caption[2] = "C";
    }
    survey.precision[0] = "%.0f";
    survey.precision[1] = "%.0f";
    survey.precision[2] = "%.0f";

    survey.data.rowCaptions.emplace_back("area1");
    survey.values[0][0] = 0.0;
    survey.values[1][0] = 42.0;
    survey.values[2][0] = 9.0;
    survey.digestNonApplicableStatus[0][2] = true; // column C = N/A

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tarea1\t0\t42\tN/A\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(header_area_count_matches_row_captions_size, StudyFixtureWithTwoAreas)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto & caption : survey.captions)
        caption[0] = "V";

    survey.data.rowCaptions.emplace_back("area1");
    survey.data.rowCaptions.emplace_back("area2");

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    // \t<columnIndex>\t<rowCaptions.size()>\t<links>
    BOOST_CHECK_NE(buffer.find("\t1\t2\t0\n"), std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

// ---------------------------------------------------------------------------
// resetValuesAtLine
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(reset_values)

BOOST_FIXTURE_TEST_CASE(zeroes_all_columns_at_given_line, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    const unsigned int nbVariables = 3;
    SurveyResults survey(*study, nbVariables, "out", writer);
    for (unsigned int col = 0; col < nbVariables; ++col)
        survey.values[col][5] = 100.0 + col;

    survey.resetValuesAtLine(5);

    for (unsigned int col = 0; col < nbVariables; ++col)
        BOOST_CHECK_EQUAL(survey.values[col][5], 0.0);
}

BOOST_FIXTURE_TEST_CASE(does_not_affect_other_lines, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 2u, "out", writer);
    survey.values[0][3] = 42.0;
    survey.values[1][3] = 84.0;
    survey.values[0][7] = 11.0;
    survey.values[1][7] = 22.0;

    survey.resetValuesAtLine(3);

    BOOST_CHECK_EQUAL(survey.values[0][3], 0.0);
    BOOST_CHECK_EQUAL(survey.values[1][3], 0.0);
    BOOST_CHECK_EQUAL(survey.values[0][7], 11.0);
    BOOST_CHECK_EQUAL(survey.values[1][7], 22.0);
}

BOOST_FIXTURE_TEST_CASE(zeroes_first_line, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 2u, "out", writer);
    survey.values[0][0] = 7.0;
    survey.values[1][0] = 8.0;

    survey.resetValuesAtLine(0);

    BOOST_CHECK_EQUAL(survey.values[0][0], 0.0);
    BOOST_CHECK_EQUAL(survey.values[1][0], 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
