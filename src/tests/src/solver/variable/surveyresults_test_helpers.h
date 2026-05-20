// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/variable.h"
#include "antares/solver/variable/storage/results.h"
#include "antares/solver/variable/area.h"
#include "antares/solver/variable/categories.h"
#include "antares/solver/variable/container.h"
#include "antares/solver/variable/tuple_variable_list.h"
#include "antares/solver/variable/economy/avail-dispatchable-generation.h"
#include "antares/solver/variable/economy/dynamic_multi_column_base.h"
#include "antares/solver/variable/economy/residual.h"
#include "antares/solver/variable/economy/thermalAirPollutantEmissions.h"
#include "antares/solver/variable/economy/productionByDispatchablePlant.h"
#include "antares/solver/variable/surveyresults.h"
#include "antares/writer/in_memory_writer.h"
#include "antares/study/parts/thermal/cluster.h"

using namespace Antares::Solver::Variable;

// ---------------------------------------------------------------------------
// Cluster helper
// ---------------------------------------------------------------------------

inline void addThermalClusterToArea(Data::Area* area,
                                    const std::string& clusterName,
                                    double nominalCapacity,
                                    unsigned int unitCount,
                                    double co2EmissionsFactor)
{
    auto cluster = std::make_shared<Data::ThermalCluster>(area);
    cluster->reset();
    cluster->setName(clusterName);
    cluster->nominalCapacity = nominalCapacity;
    cluster->unitCount = unitCount;
    cluster->mustrun = true;
    cluster->emissions.factors[Data::Pollutant::CO2] = co2EmissionsFactor;
    cluster->emissions.factors[Data::Pollutant::NH3] = 0.0;
    cluster->emissions.factors[Data::Pollutant::SO2] = 0.0;
    cluster->emissions.factors[Data::Pollutant::NOX] = 0.0;
    cluster->emissions.factors[Data::Pollutant::PM2_5] = 0.0;
    cluster->emissions.factors[Data::Pollutant::PM5] = 0.0;
    cluster->emissions.factors[Data::Pollutant::PM10] = 0.0;
    cluster->emissions.factors[Data::Pollutant::NMVOC] = 0.0;
    cluster->emissions.factors[Data::Pollutant::OP1] = 0.0;
    cluster->emissions.factors[Data::Pollutant::OP2] = 0.0;
    cluster->emissions.factors[Data::Pollutant::OP3] = 0.0;
    cluster->emissions.factors[Data::Pollutant::OP4] = 0.0;
    cluster->emissions.factors[Data::Pollutant::OP5] = 0.0;

    cluster->series.timeSeries.resize(1, HOURS_PER_YEAR);
    for (unsigned int h = 0; h < HOURS_PER_YEAR; ++h)
    {
        cluster->series.timeSeries[0][h] = nominalCapacity * unitCount;
    }
    cluster->series.timeseriesNumbers.reset(1);

    area->thermal.list.addToCompleteList(cluster);
}

// ---------------------------------------------------------------------------
// Study scaffolding
// ---------------------------------------------------------------------------

inline std::unique_ptr<Data::Study> makeStudyWithAreas(unsigned int areaCount)
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

inline std::unique_ptr<Data::Study> makeBareStudy(
  unsigned int areaCount,
  const std::function<void(Data::Area*)>& perArea = {})
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
        if (perArea)
        {
            perArea(area);
        }
    }
    study->initializeRuntimeInfos();
    return study;
}

// ---------------------------------------------------------------------------
// VariablePrintInfo helpers
// ---------------------------------------------------------------------------

inline constexpr std::array<const char*, 12> kSecondaryPollutants = {
  "NH3 EMIS.", "SO2 EMIS.", "NOX EMIS.", "PM2_5 EMIS.",
  "PM5 EMIS.", "PM10 EMIS.", "NMVOC EMIS.", "OP1 EMIS.",
  "OP2 EMIS.", "OP3 EMIS.", "OP4 EMIS.", "OP5 EMIS."};

inline void addAreaVariable(Data::Study& study,
                            std::string_view name,
                            unsigned int maxColumns,
                            bool enabled = true)
{
    Data::VariablePrintInfo info(Category::FileLevel::va, Category::DataLevel::area);
    info.setMaxColumns(maxColumns);
    if (!enabled)
    {
        info.enablePrint(false);
    }
    study.parameters.variablesPrintInfo.add(std::string(name), info);
}

inline void addUnusedLinkVars(Data::Study& study)
{
    Data::VariablePrintInfo flowLin(Category::FileLevel::va, Category::DataLevel::link);
    flowLin.enablePrint(false);
    study.parameters.variablesPrintInfo.add("FLOW LIN.", flowLin);

    Data::VariablePrintInfo flowQuad(Category::FileLevel::va, Category::DataLevel::link);
    flowQuad.enablePrint(false);
    study.parameters.variablesPrintInfo.add("FLOW QUAD.", flowQuad);
}

inline void addSecondaryPollutantPrintInfos(Data::Study& study)
{
    for (const char* name: kSecondaryPollutants)
    {
        addAreaVariable(study, name, 4u, /*enabled=*/false);
    }
}

inline void finalizePrintInfo(Data::Study& study, bool disableSecondaryPollutants = false)
{
    auto& vpi = study.parameters.variablesPrintInfo;
    vpi.setAllPrintStatusesTo(true);
    vpi.prepareForSimulation(false);
    vpi.setPrintStatus("FLOW LIN.", false);
    vpi.setPrintStatus("FLOW QUAD.", false);
    if (disableSecondaryPollutants)
    {
        for (const char* name: kSecondaryPollutants)
        {
            vpi.setPrintStatus(name, false);
        }
    }
    vpi.computeMaxColumnsCountInReports(study.setsOfAreas);
}

// ---------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Trait structs (dynamic multi-column variables)
// ---------------------------------------------------------------------------

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
        const double value = state.problemeHebdo
                               ->ConsommationsAbattues[state.hourInTheWeek]
                               .ConsommationAbattueDuPays[state.area->index];
        pValues[0][state.hourInTheYear] += value + 1;
        pValues[1][state.hourInTheYear] += (value + 1) * 2.0;
    }
};

using TwoColumnDynamicDigestVariable = Economy::DynamicMultiColumnBase<
  TwoColumnDynamicDigestTraits>;
using TwoColumnDynamicDigestVariables = Container::List<Areas<TwoColumnDynamicDigestVariable>>;

struct VaryingColumnsByAreaDigestTraits
{
    static std::string Caption()
    {
        return "DYN VAR COL";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dynamic variable with area-dependent columns";
    }

    using ResultsProfile = StandardResults<>;
    static constexpr uint8_t decimal = 0;

    static std::vector<Economy::ColumnDescriptor> buildColumnDescriptors(Data::Area* area)
    {
        if (area->index == 0)
        {
            return {{"VAR_A", "MWh"}};
        }
        return {{"VAR_A", "MWh"}, {"VAR_B", "MWh"}};
    }

    static std::vector<Economy::ColumnDescriptor> buildColumnDescriptors(Data::Study& study,
                                                                          Data::Area*)
    {
        std::set<std::string> captions;
        study.areas.each(
          [&captions](Data::Area& currentArea)
          {
              for (const auto& descriptor: buildColumnDescriptors(&currentArea))
              {
                  captions.insert(descriptor.caption);
              }
          });

        std::vector<Economy::ColumnDescriptor> descriptors;
        for (const auto& caption: captions)
        {
            descriptors.push_back({caption, "MWh"});
        }
        return descriptors;
    }

    static void setHourlyValue(
      Economy::VCardDynamicMultiColumn<
        VaryingColumnsByAreaDigestTraits>::IntermediateValuesBaseType& pValues,
      State& state,
      unsigned int,
      const std::vector<Economy::ColumnDescriptor>& descriptors)
    {
        std::map<std::string, size_t> captionToColumn;
        for (size_t i = 0; i < descriptors.size(); ++i)
        {
            captionToColumn[descriptors[i].caption] = i;
        }

        const double value = state.problemeHebdo
                               ->ConsommationsAbattues[state.hourInTheWeek]
                               .ConsommationAbattueDuPays[state.area->index];
        pValues[captionToColumn.at("VAR_A")][state.hourInTheYear] += value + 10.0;

        if (state.area->index == 1)
        {
            auto colB = captionToColumn.find("VAR_B");
            if (colB != captionToColumn.end())
            {
                pValues[colB->second][state.hourInTheYear] += value + 20.0;
            }
        }
    }
};

using VaryingColumnsByAreaDigestVariable = Economy::DynamicMultiColumnBase<
  VaryingColumnsByAreaDigestTraits>;

struct DisjointGroupsByAreaDigestTraits
{
    static std::string Caption()
    {
        return "DYN GROUPS";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dynamic variable with disjoint groups across areas";
    }

    using ResultsProfile = StandardResults<>;
    static constexpr uint8_t decimal = 0;

    static std::vector<Economy::ColumnDescriptor> buildColumnDescriptors(Data::Area* area)
    {
        if (area->index == 0)
        {
            return {{"GAS", "MWh"}};
        }
        return {{"LIGNITE", "MWh"}};
    }

    static std::vector<Economy::ColumnDescriptor> buildColumnDescriptors(Data::Study& study,
                                                                          Data::Area*)
    {
        std::set<std::string> captions;
        study.areas.each(
          [&captions](Data::Area& currentArea)
          {
              for (const auto& descriptor: buildColumnDescriptors(&currentArea))
              {
                  captions.insert(descriptor.caption);
              }
          });

        std::vector<Economy::ColumnDescriptor> descriptors;
        for (const auto& caption: captions)
        {
            descriptors.push_back({caption, "MWh"});
        }
        return descriptors;
    }

    static void setHourlyValue(
      Economy::VCardDynamicMultiColumn<
        DisjointGroupsByAreaDigestTraits>::IntermediateValuesBaseType& pValues,
      State& state,
      unsigned int,
      const std::vector<Economy::ColumnDescriptor>& descriptors)
    {
        std::map<std::string, size_t> captionToColumn;
        for (size_t i = 0; i < descriptors.size(); ++i)
        {
            captionToColumn[descriptors[i].caption] = i;
        }

        const double value = state.problemeHebdo
                               ->ConsommationsAbattues[state.hourInTheWeek]
                               .ConsommationAbattueDuPays[state.area->index];

        if (state.area->index == 0)
        {
            pValues[captionToColumn.at("GAS")][state.hourInTheYear] += value + 10.0;
        }
        else
        {
            pValues[captionToColumn.at("LIGNITE")][state.hourInTheYear] += value + 20.0;
        }
    }
};

using DisjointGroupsByAreaDigestVariable = Economy::DynamicMultiColumnBase<
  DisjointGroupsByAreaDigestTraits>;

// ---------------------------------------------------------------------------
// Variable list aliases
// ---------------------------------------------------------------------------

using ResidualDigestVariables = Container::List<Areas<Economy::ResidualLoad>>;

using VaryingColumnsThenResidualPerAreaVariables = Container::TupleVariableList<
  VaryingColumnsByAreaDigestVariable,
  Economy::ResidualLoad>;
using VaryingColumnsThenResidualDigestVariables
  = Container::List<Areas<VaryingColumnsThenResidualPerAreaVariables>>;

using TwoStaticOneDynamicTwoColumnsPerAreaVariables = Container::TupleVariableList<
  Economy::AvailableDispatchGen,
  Economy::ResidualLoad,
  TwoColumnDynamicDigestVariable>;
using TwoStaticOneDynamicTwoColumnsDigestVariables
  = Container::List<Areas<TwoStaticOneDynamicTwoColumnsPerAreaVariables>>;

using AvlDtgWithCo2ResLoadPerAreaVariables = Container::TupleVariableList<
  Economy::ThermalAirPollutantEmissions,
  Economy::AvailableDispatchGen,
  Economy::ResidualLoad>;
using AvlDtgWithCo2ResLoadVariables
  = Container::List<Areas<AvlDtgWithCo2ResLoadPerAreaVariables>>;

using AllVariablesPerAreaVariables = Container::TupleVariableList<
  Economy::ThermalAirPollutantEmissions,
  Economy::AvailableDispatchGen,
  Economy::ResidualLoad,
  TwoColumnDynamicDigestVariable>;
using AllVariablesDigestVariables = Container::List<Areas<AllVariablesPerAreaVariables>>;

using ProductionByPlantAvlDtgResLoadPerAreaVariables = Container::TupleVariableList<
  Economy::ThermalAirPollutantEmissions,
  Economy::ProductionByDispatchablePlant,
  Economy::AvailableDispatchGen,
  Economy::ResidualLoad>;
using ProductionByPlantAvlDtgResLoadVariables
  = Container::List<Areas<ProductionByPlantAvlDtgResLoadPerAreaVariables>>;

using StaticAndDisjointGroupsPerAreaVariables = Container::TupleVariableList<
  Economy::AvailableDispatchGen,
  Economy::ResidualLoad,
  DisjointGroupsByAreaDigestVariable>;
using StaticAndDisjointGroupsDigestVariables
  = Container::List<Areas<StaticAndDisjointGroupsPerAreaVariables>>;

// ---------------------------------------------------------------------------
// Specific study factories
// ---------------------------------------------------------------------------

inline std::unique_ptr<Data::Study> makeStudyWithVariable(std::string_view caption,
                                                          unsigned int maxColumns,
                                                          unsigned int areaCount = 1)
{
    auto study = makeBareStudy(areaCount);
    addAreaVariable(*study, caption, maxColumns);
    addUnusedLinkVars(*study);
    finalizePrintInfo(*study);
    return study;
}

inline std::unique_ptr<Data::Study> makeStudyForResidualDigest()
{
    return makeStudyWithVariable("RES LOAD", 4u);
}

inline std::unique_ptr<Data::Study> makeStudyForResidualDigestWithTwoAreas()
{
    return makeStudyWithVariable("RES LOAD", 4u, 2);
}

inline std::unique_ptr<Data::Study> makeStudyForOneColumnDynamicDigest()
{
    return makeStudyWithVariable(OneColumnDynamicDigestTraits::Caption(),
                                 OneColumnDynamicDigestTraits::ResultsProfile::count);
}

inline std::unique_ptr<Data::Study> makeStudyForOneColumnDynamicDigestWithTwoAreas()
{
    return makeStudyWithVariable(OneColumnDynamicDigestTraits::Caption(),
                                 OneColumnDynamicDigestTraits::ResultsProfile::count,
                                 2);
}

inline std::unique_ptr<Data::Study> makeStudyForTwoColumnDynamicDigest()
{
    return makeStudyWithVariable(TwoColumnDynamicDigestTraits::Caption(),
                                 2u * TwoColumnDynamicDigestTraits::ResultsProfile::count);
}

inline std::unique_ptr<Data::Study> makeStudyForTwoColumnDynamicDigestWithTwoAreas()
{
    return makeStudyWithVariable(TwoColumnDynamicDigestTraits::Caption(),
                                 2u * TwoColumnDynamicDigestTraits::ResultsProfile::count,
                                 2);
}

inline std::unique_ptr<Data::Study> makeStudyForMixedResidualAndOneColumnDynamicDigest(
  unsigned int areaCount = 1)
{
    auto study = makeBareStudy(areaCount);
    addAreaVariable(*study, "RES LOAD", 4u);
    addAreaVariable(*study,
                    OneColumnDynamicDigestTraits::Caption(),
                    OneColumnDynamicDigestTraits::ResultsProfile::count);
    addUnusedLinkVars(*study);
    finalizePrintInfo(*study);
    return study;
}

inline std::unique_ptr<Data::Study> makeStudyForTwoStaticAndTwoColumnDynamicDigest(
  unsigned int areaCount = 1)
{
    auto study = makeBareStudy(areaCount);
    addAreaVariable(*study, "AVL DTG", 4u);
    addAreaVariable(*study, "RES LOAD", 4u);
    addAreaVariable(*study,
                    TwoColumnDynamicDigestTraits::Caption(),
                    2u * TwoColumnDynamicDigestTraits::ResultsProfile::count);
    addUnusedLinkVars(*study);
    finalizePrintInfo(*study);
    return study;
}

inline std::unique_ptr<Data::Study> makeStudyForVaryingColumnsThenResidualDigest(
  unsigned int areaCount = 2)
{
    auto study = makeBareStudy(areaCount);
    addAreaVariable(*study,
                    VaryingColumnsByAreaDigestTraits::Caption(),
                    2u * VaryingColumnsByAreaDigestTraits::ResultsProfile::count);
    addAreaVariable(*study, "RES LOAD", 4u);
    addUnusedLinkVars(*study);
    finalizePrintInfo(*study);
    return study;
}

inline std::unique_ptr<Data::Study> makeStudyForStaticAndDisjointGroupsDigest(
  unsigned int areaCount = 2)
{
    auto study = makeBareStudy(areaCount);
    addAreaVariable(*study, "AVL DTG", 4u);
    addAreaVariable(*study, "RES LOAD", 4u);
    addAreaVariable(*study,
                    DisjointGroupsByAreaDigestTraits::Caption(),
                    2u * DisjointGroupsByAreaDigestTraits::ResultsProfile::count);
    addUnusedLinkVars(*study);
    finalizePrintInfo(*study);
    return study;
}

inline std::unique_ptr<Data::Study> makeStudyWithAvlDtgCo2AndResLoad(unsigned int areaCount = 1)
{
    auto study = makeBareStudy(areaCount);
    addAreaVariable(*study, "AVL DTG", 4u);
    addAreaVariable(*study, "CO2 EMIS.", 4u);
    addSecondaryPollutantPrintInfos(*study);
    addAreaVariable(*study, "RES LOAD", 4u);
    addUnusedLinkVars(*study);
    finalizePrintInfo(*study, /*disableSecondaryPollutants=*/true);
    return study;
}

inline std::unique_ptr<Data::Study> makeStudyWithAllVariables(unsigned int areaCount = 1)
{
    auto study = makeBareStudy(areaCount,
                               [](Data::Area* area)
                               {
                                   addThermalClusterToArea(area, "coal", 100.0, 2, 0.5);
                               });
    addAreaVariable(*study, "CO2 EMIS.", 4u);
    addSecondaryPollutantPrintInfos(*study);
    addAreaVariable(*study, "AVL DTG", 4u);
    addAreaVariable(*study,
                    TwoColumnDynamicDigestTraits::Caption(),
                    2u * TwoColumnDynamicDigestTraits::ResultsProfile::count);
    addAreaVariable(*study, "RES LOAD", 4u);
    addUnusedLinkVars(*study);
    finalizePrintInfo(*study, /*disableSecondaryPollutants=*/true);
    return study;
}

// ---------------------------------------------------------------------------
// Simulation harness
// ---------------------------------------------------------------------------

inline PROBLEME_HEBDO makeWeeklyResidualProblem(double value, unsigned int nAreas = 1)
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
