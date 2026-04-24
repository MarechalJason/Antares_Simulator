// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file economy_base.h
**
** \brief Base class for area-level economy variables
**
** ## Traits Contract
**
** A valid Economy variable Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - \c ResultsType : type alias for results template
**   - \c decimal : uint8_t
**   - \c spatialAggregate : uint8_t
**   - \c computeStats(IntermediateValues&) -> void
**
** - Optional hooks (dispatched via \c if constexpr (requires { ... })):
**   - \c AuxiliaryDataType : type alias (enables auxiliary data support)
**   - \c isPossiblyNonApplicable : uint8_t (0 or 1)
**   - \c initializeFromArea(AuxiliaryDataType&, Data::Study*, Data::Area*) -> void
*(InitializationPolicy)
**   - \c yearBegin(IntermediateValues&, AuxiliaryDataType&, uint year, uint
*numSpace) -> void  (YearlyLifecyclePolicy)
**   - \c yearEndBuild(IntermediateValues&, AuxiliaryDataType&, State&, uint year, unsigned
*int numSpace) -> void  (YearlyLifecyclePolicy)
**   - \c yearEndBuildForEachThermalCluster(IntermediateValues&, State&, uint year, unsigned
*int numSpace) -> void  (ThermalClusterPolicy)
**   - \c weekForEachArea(IntermediateValues&, State&, uint numSpace) -> void
*(HourlyAggregationPolicy)
**   - \c setHourlyValue(IntermediateValues&, AuxiliaryDataType&, State&, uint numSpace) ->
*void  (HourlyComputationPolicy)
**
** ## Hook execution order
** - initializeFromStudy() / initializeFromArea() / initializeFromLink()
** - simulationBegin() / simulationEnd()
** - yearBegin() -> [yearBegin hook] -> yearEndBuild() -> [yearEndBuild hook] -> yearEnd()
** - computeSummary() / hourBegin() -> hourForEachArea() -> [setHourlyValue hook]
** - weekForEachArea() (optional, called after hourForEachArea for weekly aggregation)
**
** ## Policy groupings proposal
** The optional hooks can be grouped by lifecycle phase:
**
** - InitializationPolicy: initializeFromArea
** - YearlyLifecyclePolicy: yearBegin, yearEndBuild
** - ThermalClusterPolicy: yearEndBuildForEachThermalCluster
** - HourlyAggregationPolicy: weekForEachArea
** - HourlyComputationPolicy: setHourlyValue
**
** Future refactoring: Use tag dispatch to dispatch to policy classes rather than if constexpr
*cascades.
*/

#include <type_traits>

#include <antares/memory/memory.h>
#include <antares/solver/variable/categories.h>
#include <antares/solver/variable/container.h>
#include <antares/solver/variable/state.h>
#include <antares/solver/variable/storage/intermediate.h>
#include <antares/solver/variable/storage/results.h>
#include <antares/solver/variable/surveyresults.h>
#include <antares/solver/variable/variable.h>
#include <antares/study/area/area.h>
#include <antares/study/area/links.h>
#include <antares/study/study.h>

namespace Antares::Solver::Variable::Economy
{

namespace Hooks_
{
template<class Traits, class AuxData>
void initializeFromAreaIfSupported(AuxData& aux, Data::Study* study, Data::Area* area)
{
    if constexpr (requires { Traits::initializeFromArea(aux, study, area); })
    {
        Traits::initializeFromArea(aux, study, area);
    }
}

template<class Traits, class IV, class Aux>
void yearBeginIfSupported(IV& iv, Aux& aux, uint year, uint numSpace)
{
    if constexpr (requires { Traits::yearBegin(iv, aux, year, numSpace); })
    {
        Traits::yearBegin(iv, aux, year, numSpace);
    }
}

template<class Traits, class IV, class Aux, class State>
void yearEndBuildIfSupported(IV& iv, Aux& aux, State& state, uint year, uint numSpace)
{
    if constexpr (requires { Traits::yearEndBuild(iv, aux, state, year, numSpace); })
    {
        Traits::yearEndBuild(iv, aux, state, year, numSpace);
    }
}

template<class Traits, class IV, class State>
void yearEndBuildForEachIfSupported(IV& iv, State& state, uint year, uint numSpace)
{
    if constexpr (requires { Traits::yearEndBuildForEachThermalCluster(iv, state, year, numSpace); })
    {
        Traits::yearEndBuildForEachThermalCluster(iv, state, year, numSpace);
    }
}

template<class Traits, class IV, class State>
void weekForEachAreaIfSupported(IV& iv, State& state, uint numSpace)
{
    if constexpr (requires { Traits::weekForEachArea(iv, state, numSpace); })
    {
        Traits::weekForEachArea(iv, state, numSpace);
    }
}

template<class Traits, class IV, class Aux, class State>
void setHourlyValueIfSupported(IV& iv, Aux& aux, State& state, uint numSpace)
{
    if constexpr (requires { Traits::setHourlyValue(iv, aux, state, numSpace); })
    {
        Traits::setHourlyValue(iv, aux, state, numSpace);
    }
    else if constexpr (requires { Traits::setHourlyValue(iv, state, numSpace); })
    {
        Traits::setHourlyValue(iv, state, numSpace);
    }
}

} // namespace Hooks_

namespace detail

{
struct EmptyAuxiliaryData
{
};

template<class TraitsT, class = void>
struct AuxiliaryDataType
{
    using type = EmptyAuxiliaryData;
};

template<class TraitsT>
struct AuxiliaryDataType<TraitsT, std::void_t<typename TraitsT::AuxiliaryDataType>>
{
    using type = typename TraitsT::AuxiliaryDataType;
};

template<class VCardType, class ResultsType, int CDataLevel, int CFile>
inline constexpr int statisticsCount =
  ((VCardType::categoryDataLevel & CDataLevel && VCardType::categoryFileLevel & CFile)
   ? VCardType::columnCount * ResultsType::count
   : 0);
} // namespace detail

template<class Traits>
struct EconomyVariableCard
{
    //! Caption
    static std::string Caption()
    {
        return Traits::Caption();
    }

    //! Unit
    static std::string Unit()
    {
        return Traits::Unit();
    }

    //! The short description of the variable
    static std::string Description()
    {
        return Traits::Description();
    }

    //! The expected results
    using ResultsType = typename Traits::ResultsType;

    using VCardForSpatialAggregate = EconomyVariableCard;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = Traits::decimal;
    //! Number of columns used by the variable (One ResultsType per column)
    static constexpr int columnCount = 1;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Traits::spatialAggregate;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = []
    {
        if constexpr (requires { Traits::isPossiblyNonApplicable; })
        {
            return Traits::isPossiblyNonApplicable;
        }
        else
        {
            return uint8_t{0};
        }
    }();

    using IntermediateValuesBaseType = IntermediateValues;
    using IntermediateValuesType = std::vector<IntermediateValues>;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;

}; // struct EconomyVariableCard

/*!
** \brief Base class for area-level economy variables like LOLP and LOLD
**
** Post-A refactor: variables are standalone (no NextT propagation).
** Each variable inherits directly from the 2-parameter IVariable.
*/
template<class Traits>
class EconomyVariableBase
    : public Variable::IVariable<EconomyVariableBase<Traits>, EconomyVariableCard<Traits>>
{
public:
    //! VCard
    using VCardType = EconomyVariableCard<Traits>;
    //! Ancestor
    using AncestorType = Variable::IVariable<EconomyVariableBase<Traits>, VCardType>;

    //! List of expected results
    using ResultsType = typename VCardType::ResultsType;

    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count =
          detail::statisticsCount<VCardType, ResultsType, CDataLevel, CFile>;
    };

public:
    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;

        // Intermediate values
        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        Hooks_::initializeFromAreaIfSupported<Traits>(auxiliaryData_, study, area);
    }

    void initializeFromLink(Data::Study* /*study*/, Data::AreaLink* /*link*/)
    {
    }

    void simulationBegin()
    {
        for (uint numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].reset();
        }
    }

    void simulationEnd()
    {
    }

    void yearBegin(uint year, uint numSpace)
    {
        // Reset the values for the current year
        pValuesForTheCurrentYear[numSpace].reset();

        Hooks_::yearBeginIfSupported<Traits>(
          pValuesForTheCurrentYear[numSpace],
          auxiliaryData_,
          year,
          numSpace);
    }

    void yearEndBuild(State& state, uint year, uint numSpace)
    {
        Hooks_::yearEndBuildIfSupported<Traits>(
          pValuesForTheCurrentYear[numSpace],
          auxiliaryData_,
          state,
          year,
          numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, uint numSpace)
    {
        Hooks_::yearEndBuildForEachIfSupported<Traits>(
          pValuesForTheCurrentYear[numSpace],
          state,
          year,
          numSpace);
    }

    void yearEnd(uint /*year*/, uint numSpace)
    {
        // Compute all statistics for the current year (daily,weekly,monthly)
        Traits::computeStats(pValuesForTheCurrentYear[numSpace]);
    }

    void computeSummary(uint year, uint numSpace)
    {
        // Merge all those values with the global results
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);
    }

    void hourBegin(uint /*hourInTheYear*/)
    {
    }

    void hourForEachArea(State& state, uint numSpace)
    {
        Hooks_::setHourlyValueIfSupported<Traits>(
          pValuesForTheCurrentYear[numSpace],
          auxiliaryData_,
          state,
          numSpace);
    }

    void weekForEachArea(State& state, uint numSpace)
    {
        Hooks_::weekForEachAreaIfSupported<Traits>(
          pValuesForTheCurrentYear[numSpace],
          state,
          numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      uint,
      uint numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      uint numSpace) const
    {
        // Initializing external pointer on current variable non applicable status
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            // Write the data for the current year
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear[numSpace]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }


private:
    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    AuxiliaryDataType auxiliaryData_{};
    uint pNbYearsParallel = 0;

}; // class EconomyVariableBase


} // namespace Antares::Solver::Variable::Economy
