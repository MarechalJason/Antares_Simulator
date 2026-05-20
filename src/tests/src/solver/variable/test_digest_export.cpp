// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "surveyresults_test_helpers.h"

BOOST_AUTO_TEST_SUITE(digest)

BOOST_FIXTURE_TEST_CASE(header_contains_digest_label_and_counts_section, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto& caption: survey.captions)
    {
        caption[0] = "V";
    }

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tdigest\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\tVARIABLES\tAREAS\tLINKS\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(header_contains_digest_label_and_counts_section_with_area,
                        StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto& caption: survey.captions)
    {
        caption[0] = "V";
    }
    survey.data.rowCaptions.emplace_back("area1");

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\tdigest\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\tVARIABLES\tAREAS\tLINKS\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t1\t1\t0\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(header_counts_3_variables_0_areas_0_links, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 3u, "out", writer);
    survey.data.columnIndex = 3;
    for (auto& caption: survey.captions)
    {
        caption[0] = caption[1] = caption[2] = "x";
    }

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\t3\t0\t0\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(header_counts_3_variables_1_area_0_links, StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 3u, "out", writer);
    survey.data.columnIndex = 3;
    for (auto& caption: survey.captions)
    {
        caption[0] = caption[1] = caption[2] = "x";
    }
    survey.data.rowCaptions.emplace_back("area1");

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\t3\t1\t0\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(three_caption_rows_are_written_for_each_column, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 2u, "out", writer);
    survey.data.columnIndex = 2;
    survey.captions[0][0] = "LOAD";
    survey.captions[0][1] = "WIND";
    survey.captions[1][0] = "MW";
    survey.captions[1][1] = "MW";
    survey.captions[2][0] = "EXP";
    survey.captions[2][1] = "std";

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\t\tLOAD\tWIND\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t\tMW\tMW\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t\tEXP\tstd\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(three_caption_rows_are_written_for_each_column_with_area,
                        StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 2u, "out", writer);
    survey.data.columnIndex = 2;
    survey.captions[0][0] = "LOAD";
    survey.captions[0][1] = "WIND";
    survey.captions[1][0] = "MW";
    survey.captions[1][1] = "MW";
    survey.captions[2][0] = "EXP";
    survey.captions[2][1] = "std";
    survey.data.rowCaptions.emplace_back("area1");

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\t\tLOAD\tWIND\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t\tMW\tMW\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t\tEXP\tstd\n"), std::string::npos);
    BOOST_CHECK_NE(buffer.find("\t2\t1\t0\n"), std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(row_with_zero_value_writes_tab_zero, StudyFixtureWithArea)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    SurveyResults survey(*study, 1u, "out", writer);
    survey.data.columnIndex = 1;
    for (auto& caption: survey.captions)
    {
        caption[0] = "V";
    }

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
    for (auto& caption: survey.captions)
    {
        caption[0] = "V";
    }
    survey.precision[0] = "%.0f";

    survey.data.rowCaptions.emplace_back("area1");
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
    for (auto& caption: survey.captions)
    {
        caption[0] = "V";
    }

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
    for (auto& caption: survey.captions)
    {
        caption[0] = "V";
    }
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
    for (auto& caption: survey.captions)
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
    for (auto& caption: survey.captions)
    {
        caption[0] = "V";
    }

    survey.data.rowCaptions.emplace_back("area1");
    survey.data.rowCaptions.emplace_back("area2");

    std::string buffer;
    survey.exportDigestAllYears(buffer);

    BOOST_CHECK_NE(buffer.find("\t1\t2\t0\n"), std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
