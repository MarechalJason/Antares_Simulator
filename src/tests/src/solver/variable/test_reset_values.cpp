// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "surveyresults_test_helpers.h"

BOOST_AUTO_TEST_SUITE(reset_values)

BOOST_FIXTURE_TEST_CASE(zeroes_all_columns_at_given_line, StudyFixture)
{
    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);

    const unsigned int nbVariables = 3;
    SurveyResults survey(*study, nbVariables, "out", writer);
    for (unsigned int col = 0; col < nbVariables; ++col)
    {
        survey.values[col][5] = 100.0 + col;
    }

    survey.resetValuesAtLine(5);

    for (unsigned int col = 0; col < nbVariables; ++col)
    {
        BOOST_CHECK_EQUAL(survey.values[col][5], 0.0);
    }
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
