// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test survey results"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/variable.h"
#include "antares/solver/variable/area.h"
#include "antares/solver/variable/categories.h"
#include "antares/solver/variable/container.h"
#include "antares/solver/variable/tuple_variable_list.h"
#include "antares/solver/variable/economy/avail-dispatchable-generation.h"
#include "antares/solver/variable/economy/dynamic_multi_column_base.h"
#include "antares/solver/variable/economy/residual.h"
#include "antares/solver/variable/surveyresults.h"
#include "antares/writer/in_memory_writer.h"

using namespace Antares::Solver::Variable;

namespace
{

std::unique_ptr<Data::Study> makeStudyWithAreas(unsigned int areaCount)
{
    auto study = std::make_unique<Data::Study>();
    study->parameters.simulationDays.first = 0;
    study->parameters.simulationDays.end = 7;
    for (unsigned int i = 0; i < areaCount; ++i)
    {
        auto* area = study->areaAdd("area" + std::to_string(i + 1));
        area->index = i;
    }
    study->initializeRuntimeInfos();
    return study;
}

struct StudyFixture
{
    StudyFixture():
        study(makeStudyWithAreas(0))
    {
    }

    std::unique_ptr<Data::Study> study;
};

struct StudyFixtureWithArea
{
    StudyFixtureWithArea():
        study(makeStudyWithAreas(1))
    {
    }

    std::unique_ptr<Data::Study> study;
};

struct StudyFixtureWithTwoAreas
{
    StudyFixtureWithTwoAreas():
        study(makeStudyWithAreas(2))
    {
    }

    std::unique_ptr<Data::Study> study;
};

using ResidualDigestVariables = Container::List<Areas<Economy::ResidualLoad>>;



struct OneColumnDynamicDigestTraits
{
    static std::string Caption()
    {
        return "DYN ONE COL";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dynamic variable with one digest column";
    }

    using ResultsProfile = StandardResults<>;
    static constexpr uint8_t decimal = 0;

    static std::vector<Economy::ColumnDescriptor> buildColumnDescriptors(Data::Area*)
    {
        return {{"DYN_COL_1", "MWh"}};
    }

    static void setHourlyValue(Economy::VCardDynamicMultiColumn<
                                 OneColumnDynamicDigestTraits>::IntermediateValuesBaseType& pValues,
                               State& state,
                               unsigned int,
                               const std::vector<Economy::ColumnDescriptor>&)
    {
        pValues[0][state.hourInTheYear] += state.problemeHebdo
                                             ->ConsommationsAbattues[state.hourInTheWeek]
                                             .ConsommationAbattueDuPays[state.area->index];
    }
};

using OneColumnDynamicDigestVariable = Economy::DynamicMultiColumnBase<
  OneColumnDynamicDigestTraits>;
using OneColumnDynamicDigestVariables = Container::List<Areas<OneColumnDynamicDigestVariable>>;

struct TwoColumnDynamicDigestTraits
{
    static std::string Caption()
    {
        return "DYN TWO COL";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dynamic variable with two digest columns";
    }

    using ResultsProfile = StandardResults<>;
    static constexpr uint8_t decimal = 0;

    static std::vector<Economy::ColumnDescriptor> buildColumnDescriptors(Data::Area*)
    {
        return {{"DYN_COL_1", "MWh"}, {"DYN_COL_2", "MWh"}};
    }

    static void setHourlyValue(Economy::VCardDynamicMultiColumn<
                                 TwoColumnDynamicDigestTraits>::IntermediateValuesBaseType& pValues,
                               State& state,
                               unsigned int,
                               const std::vector<Economy::ColumnDescriptor>&)
    {
        double value = state.problemeHebdo->ConsommationsAbattues[state.hourInTheWeek]
                         .ConsommationAbattueDuPays[state.area->index];

        pValues[0][state.hourInTheYear] += value;
        pValues[1][state.hourInTheYear] += value * 2.0;
    }
};

using TwoColumnDynamicDigestVariable = Economy::DynamicMultiColumnBase<
  TwoColumnDynamicDigestTraits>;
using TwoColumnDynamicDigestVariables = Container::List<Areas<TwoColumnDynamicDigestVariable>>;
using TwoStaticOneDynamicTwoColumnsPerAreaVariables = Container::TupleVariableList<
  Economy::AvailableDispatchGen,
  Economy::ResidualLoad,
  TwoColumnDynamicDigestVariable>;
using TwoStaticOneDynamicTwoColumnsDigestVariables
  = Container::List<Areas<TwoStaticOneDynamicTwoColumnsPerAreaVariables>>;

std::unique_ptr<Data::Study> makeStudyWithVariable(std::string_view caption,
                                                   unsigned int maxColumns,
                                                   unsigned int areaCount = 1)
{
    auto study = std::make_unique<Data::Study>();

    study->parameters.simulationDays.first = 0;
    study->parameters.simulationDays.end = 7;
    study->parameters.nbYears = 1;
    study->maxNbYearsInParallel = 1;
    study->parameters.userPlaylist = false;
    study->parameters.resetPlaylist(study->parameters.nbYears);
    study->parameters.resetYearsWeigth();

    for (unsigned int i = 0; i < areaCount; ++i)
    {
        auto* area = study->areaAdd("area" + std::to_string(i + 1));
        area->index = i;
    }
    study->initializeRuntimeInfos();

    Data::VariablePrintInfo varInfo(Category::FileLevel::va, Category::DataLevel::area);
    varInfo.setMaxColumns(maxColumns);
    study->parameters.variablesPrintInfo.add(std::string(caption), varInfo);

    Data::VariablePrintInfo flowLinInfo(Category::FileLevel::va, Category::DataLevel::link);
    flowLinInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW LIN.", flowLinInfo);

    Data::VariablePrintInfo flowQuadInfo(Category::FileLevel::va, Category::DataLevel::link);
    flowQuadInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW QUAD.", flowQuadInfo);

    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    study->parameters.variablesPrintInfo.prepareForSimulation(false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW LIN.", false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW QUAD.", false);
    study->parameters.variablesPrintInfo.computeMaxColumnsCountInReports(study->setsOfAreas);

    return study;
}

inline std::unique_ptr<Data::Study> makeStudyForResidualDigest()
{
    return makeStudyWithVariable("RES LOAD", 4);
}

inline std::unique_ptr<Data::Study> makeStudyForResidualDigestWithTwoAreas()
{
    return makeStudyWithVariable("RES LOAD", 4, 2);
}

inline std::unique_ptr<Data::Study> makeStudyForOneColumnDynamicDigestWithTwoAreas()
{
    return makeStudyWithVariable(OneColumnDynamicDigestTraits::Caption(),
                                 OneColumnDynamicDigestTraits::ResultsProfile::count,
                                 2);
}

inline std::unique_ptr<Data::Study> makeStudyForTwoColumnDynamicDigestWithTwoAreas()
{
    return makeStudyWithVariable(TwoColumnDynamicDigestTraits::Caption(),
                                 2 * TwoColumnDynamicDigestTraits::ResultsProfile::count,
                                 2);
}

PROBLEME_HEBDO makeWeeklyResidualProblem(double value, unsigned int nAreas = 1)
{
    PROBLEME_HEBDO weeklyProblem;
    weeklyProblem.NombreDePays = nAreas;
    weeklyProblem.NombreDePasDeTemps = Antares::Constants::nbHoursInAWeek;
    weeklyProblem.ResultatsHoraires.resize(nAreas);
    weeklyProblem.ConsommationsAbattues.resize(Antares::Constants::nbHoursInAWeek);

    for (unsigned int h = 0; h < Antares::Constants::nbHoursInAWeek; ++h)
    {
        weeklyProblem.ConsommationsAbattues[h].ConsommationAbattueDuPays.resize(nAreas);
        for (unsigned int a = 0; a < nAreas; ++a)
        {
            weeklyProblem.ConsommationsAbattues[h].ConsommationAbattueDuPays[a] = value;
        }
    }

    return weeklyProblem;
}

inline std::unique_ptr<Data::Study> makeStudyForOneColumnDynamicDigest()
{
    return makeStudyWithVariable(OneColumnDynamicDigestTraits::Caption(),
                                 OneColumnDynamicDigestTraits::ResultsProfile::count);
}

inline std::unique_ptr<Data::Study> makeStudyForTwoColumnDynamicDigest()
{
    return makeStudyWithVariable(TwoColumnDynamicDigestTraits::Caption(),
                                 2 * TwoColumnDynamicDigestTraits::ResultsProfile::count);
}

std::unique_ptr<Data::Study> makeStudyForMixedResidualAndOneColumnDynamicDigest(
  unsigned int areaCount = 1)
{
    auto study = std::make_unique<Data::Study>();

    study->parameters.simulationDays.first = 0;
    study->parameters.simulationDays.end = 7;
    study->parameters.nbYears = 1;
    study->maxNbYearsInParallel = 1;
    study->parameters.userPlaylist = false;
    study->parameters.resetPlaylist(study->parameters.nbYears);
    study->parameters.resetYearsWeigth();

    for (unsigned int i = 0; i < areaCount; ++i)
    {
        auto* area = study->areaAdd("area" + std::to_string(i + 1));
        area->index = i;
    }
    study->initializeRuntimeInfos();

    Data::VariablePrintInfo residualInfo(Category::FileLevel::va, Category::DataLevel::area);
    residualInfo.setMaxColumns(4u);
    study->parameters.variablesPrintInfo.add("RES LOAD", residualInfo);

    Data::VariablePrintInfo dynamicInfo(Category::FileLevel::va, Category::DataLevel::area);
    dynamicInfo.setMaxColumns(OneColumnDynamicDigestTraits::ResultsProfile::count);
    study->parameters.variablesPrintInfo.add(OneColumnDynamicDigestTraits::Caption(), dynamicInfo);

    Data::VariablePrintInfo flowLinInfo(Category::FileLevel::va, Category::DataLevel::link);
    flowLinInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW LIN.", flowLinInfo);

    Data::VariablePrintInfo flowQuadInfo(Category::FileLevel::va, Category::DataLevel::link);
    flowQuadInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW QUAD.", flowQuadInfo);

    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    study->parameters.variablesPrintInfo.prepareForSimulation(false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW LIN.", false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW QUAD.", false);
    study->parameters.variablesPrintInfo.computeMaxColumnsCountInReports(study->setsOfAreas);

    return study;
}

std::unique_ptr<Data::Study> makeStudyForTwoStaticAndTwoColumnDynamicDigest(unsigned int areaCount = 1)
{
    auto study = std::make_unique<Data::Study>();

    study->parameters.simulationDays.first = 0;
    study->parameters.simulationDays.end = 7;
    study->parameters.nbYears = 1;
    study->maxNbYearsInParallel = 1;
    study->parameters.userPlaylist = false;
    study->parameters.resetPlaylist(study->parameters.nbYears);
    study->parameters.resetYearsWeigth();

    for (unsigned int i = 0; i < areaCount; ++i)
    {
        auto* area = study->areaAdd("area" + std::to_string(i + 1));
        area->index = i;
    }
    study->initializeRuntimeInfos();

    Data::VariablePrintInfo availableDispatchInfo(Category::FileLevel::va, Category::DataLevel::area);
    availableDispatchInfo.setMaxColumns(4u);
    study->parameters.variablesPrintInfo.add("AVL DTG", availableDispatchInfo);

    Data::VariablePrintInfo residualInfo(Category::FileLevel::va, Category::DataLevel::area);
    residualInfo.setMaxColumns(4u);
    study->parameters.variablesPrintInfo.add("RES LOAD", residualInfo);

    Data::VariablePrintInfo dynamicInfo(Category::FileLevel::va, Category::DataLevel::area);
    dynamicInfo.setMaxColumns(2u * TwoColumnDynamicDigestTraits::ResultsProfile::count);
    study->parameters.variablesPrintInfo.add(TwoColumnDynamicDigestTraits::Caption(), dynamicInfo);

    Data::VariablePrintInfo flowLinInfo(Category::FileLevel::va, Category::DataLevel::link);
    flowLinInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW LIN.", flowLinInfo);

    Data::VariablePrintInfo flowQuadInfo(Category::FileLevel::va, Category::DataLevel::link);
    flowQuadInfo.enablePrint(false);
    study->parameters.variablesPrintInfo.add("FLOW QUAD.", flowQuadInfo);

    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    study->parameters.variablesPrintInfo.prepareForSimulation(false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW LIN.", false);
    study->parameters.variablesPrintInfo.setPrintStatus("FLOW QUAD.", false);
    study->parameters.variablesPrintInfo.computeMaxColumnsCountInReports(study->setsOfAreas);

    return study;
}

template<typename VariablesT>
std::string runSimulationAndExportDigest(Data::Study& study,
                                         VariablesT& variables,
                                         double weeklyValue)
{
    variables.initializeFromStudy(study);
    variables.simulationBegin();

    State state(study);
    state.year = 0;
    state.numSpace = 0;
    state.startANewYear();

    PROBLEME_HEBDO weeklyProblem = makeWeeklyResidualProblem(weeklyValue, study.areas.size());
    state.problemeHebdo = &weeklyProblem;

    variables.yearBegin(0, 0);

    for (unsigned int h = 0; h < HOURS_PER_YEAR; ++h)
    {
        state.hourInTheYear = h;
        state.hourInTheWeek = h % Antares::Constants::nbHoursInAWeek;
        variables.hourForEachArea(state, 0);
    }

    variables.yearEndBuild(state, 0, 0);
    variables.yearEnd(0, 0);
    variables.computeSummary(0, 0);
    variables.simulationEnd();

    Benchmarking::DurationCollector dc;
    Solver::InMemoryWriter writer(dc);
    variables.exportSurveyResults(true, "out", 0, writer);

    const auto& files = writer.getMap();
    const auto fileIt = files.find("out/grid/digest.txt");
    BOOST_REQUIRE(fileIt != files.end());
    return fileIt->second;
}

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
    BOOST_CHECK_NE(content.find("system	hourly				hello	world\n"),
                   std::string::npos);
    // Some values
    BOOST_CHECK_NE(content.find("	9	01	JAN	08:00	9	67\n"),
                   std::string::npos);
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

    // \t<columnIndex>\t<rowCaptions.size()>\t<links>
    BOOST_CHECK_NE(buffer.find("\t1\t2\t0\n"), std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

// ---------------------------------------------------------------------------
// digest export from variables tree
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(digest_with_variables)

BOOST_AUTO_TEST_CASE(exports_digest_from_residual_load_variable_tree)
{
    auto study = makeStudyForResidualDigest();
    ResidualDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_residual_load_variable_tree_with_two_areas)
{
    auto study = makeStudyForResidualDigestWithTwoAreas();
    ResidualDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea2\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_one_column)
{
    auto study = makeStudyForOneColumnDynamicDigest();
    OneColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_one_column_with_two_areas)
{
    auto study = makeStudyForOneColumnDynamicDigestWithTwoAreas();
    OneColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea2\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_for_static_and_dynamic_one_column_variables)
{
    auto study = makeStudyForMixedResidualAndOneColumnDynamicDigest();
    ResidualDigestVariables residualVariables;
    const auto& residualDigest = runSimulationAndExportDigest(*study, residualVariables, 5.0);
    OneColumnDynamicDigestVariables dynamicVariables;
    const auto& dynamicDigest = runSimulationAndExportDigest(*study, dynamicVariables, 5.0);

    BOOST_CHECK_NE(residualDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\tarea1\t840\n"), std::string::npos);

    BOOST_CHECK_NE(dynamicDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\tarea1\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_for_static_and_dynamic_one_column_variables_with_two_areas)
{
    auto study = makeStudyForMixedResidualAndOneColumnDynamicDigest(2);
    ResidualDigestVariables residualVariables;
    const auto& residualDigest = runSimulationAndExportDigest(*study, residualVariables, 5.0);
    OneColumnDynamicDigestVariables dynamicVariables;
    const auto& dynamicDigest = runSimulationAndExportDigest(*study, dynamicVariables, 5.0);

    BOOST_CHECK_NE(residualDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\tarea2\t840\n"), std::string::npos);

    BOOST_CHECK_NE(dynamicDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\tarea2\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_two_columns)
{
    auto study = makeStudyForTwoColumnDynamicDigest();
    TwoColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t2\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\tDYN_COL_2\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\t1680\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_two_columns_with_two_areas)
{
    auto study = makeStudyForTwoColumnDynamicDigestWithTwoAreas();
    TwoColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t2\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\tDYN_COL_2\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\t1680\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea2\t840\t1680\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_two_static_and_one_dynamic_two_columns_variables)
{
    auto study = makeStudyForTwoStaticAndTwoColumnDynamicDigest();
    TwoStaticOneDynamicTwoColumnsDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t4\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tAVL DTG\tRES LOAD\tDYN_COL_1\tDYN_COL_2\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\tMWh\tMWh\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\tEXP\tEXP\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t0\t840\t840\t1680\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(digest_column_order_with_multiple_variables)
{
    auto study = makeStudyForTwoStaticAndTwoColumnDynamicDigest();
    TwoStaticOneDynamicTwoColumnsDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    auto avlDtgPos = digest.find("\tAVL DTG\t");
    auto resLoadPos = digest.find("\tRES LOAD\t");
    auto dynCol1Pos = digest.find("\tDYN_COL_1\t");

    BOOST_REQUIRE(avlDtgPos != std::string::npos);
    BOOST_REQUIRE(resLoadPos != std::string::npos);
    BOOST_REQUIRE(dynCol1Pos != std::string::npos);

    BOOST_CHECK(avlDtgPos < resLoadPos);
    BOOST_CHECK(resLoadPos < dynCol1Pos);
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
