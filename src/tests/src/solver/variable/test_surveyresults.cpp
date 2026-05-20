// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test survey results"

#include "surveyresults_test_helpers.h"

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
    BOOST_CHECK_NE(content.find("system\thourly\t\t\t\thello\tworld\n"), std::string::npos);
    // Some values
    BOOST_CHECK_NE(content.find("\t9\t01\tJAN\t08:00\t9\t67\n"), std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
