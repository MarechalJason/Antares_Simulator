// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file multi_column_base.h
**
** \brief Base class for static multi-column variables (fixed column count)
**
** ## Traits Contract
**
** A valid MultiColumn Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - \c ResultsType : type alias for results template
**   - \c decimal : uint8_t
**   - \c columnCaption(unsigned int) -> std::string (for GUI display)
**
** - Required static member constants:
**   - \c columnCount : constexpr int (number of columns)
**
** - Optional hooks:
**   - \c onInitializeFromStudy(Data::Study&) -> void
**   - \c onInitializeFromArea(Data::Area*, Data::Study*) -> void
**   - \c onSimulationBegin(IntermediateValuesBaseType*, unsigned int) -> void
**   - \c setHourlyValue(IntermediateValues(&)[ColCount], State&, unsigned int numSpace) -> void
*/

#include <type_traits>

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

template<class Traits, int ColCount>
struct VCardMultiColumn
{
    static std::string Caption()
    {
        return Traits::Caption();
    }

    static std::string Unit()
    {
        return Traits::Unit();
    }

    static std::string Description()
    {
        return Traits::Description();
    }

    using ResultsType = typename Traits::ResultsType;
    using VCardForSpatialAggregate = VCardMultiColumn;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t nodeDepthForGUI = +0;
    static constexpr uint8_t decimal = Traits::decimal;
    static constexpr int columnCount = ColCount;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    static constexpr uint8_t hasIntermediateValues = 1;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    using IntermediateValuesBaseType = IntermediateValues[columnCount];
    using IntermediateValuesType = IntermediateValuesBaseType*;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;

    struct Multiple
    {
        static std::string Caption(unsigned int indx)
        {
            return Traits::columnCaption(indx);
        }

        static std::string Unit([[maybe_unused]] unsigned int indx)
        {
            return Traits::Unit();
        }
    };
};

template<class Traits, int ColCount>
class MultiColumnBase: public Variable::IVariable<MultiColumnBase<Traits, ColCount>,
                                                  void,
                                                  VCardMultiColumn<Traits, ColCount>>
{
public:
    using VCardType = VCardMultiColumn<Traits, ColCount>;
    using AncestorType = Variable::IVariable<MultiColumnBase<Traits, ColCount>, void, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = ((VCardType::categoryDataLevel & CDataLevel
                                      && VCardType::categoryFileLevel & CFile)
                                      ? VCardType::columnCount * ResultsType::count
                                      : 0);
    };

public:
    ~MultiColumnBase()
    {
        delete[] pValuesForTheCurrentYear;
    }

    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;
        InitializeResultsFromStudy(AncestorType::pResults, study);

        pValuesForTheCurrentYear = new VCardType::IntermediateValuesBaseType[pNbYearsParallel];
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            for (int i = 0; i < ColCount; ++i)
            {
                pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(study);
            }
        }

        if constexpr (requires { Traits::onInitializeFromStudy(std::declval<Data::Study&>()); })
        {
            Traits::onInitializeFromStudy(study);
        }
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        for (int i = 0; i < ColCount; ++i)
        {
            results[i].initializeFromStudy(study);
            results[i].reset();
        }
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        if constexpr (requires {
                          Traits::onInitializeFromArea(std::declval<Data::Area*>(),
                                                       std::declval<Data::Study*>());
                      })
        {
            Traits::onInitializeFromArea(area, study);
        }
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
    }

    void simulationBegin()
    {
        if constexpr (requires {
                          Traits::onSimulationBegin(pValuesForTheCurrentYear, pNbYearsParallel);
                      })
        {
            Traits::onSimulationBegin(pValuesForTheCurrentYear, pNbYearsParallel);
        }
    }

    void simulationEnd()
    {
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        for (int i = 0; i < ColCount; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        VariableAccessorType::template ComputeStatistics<VCardType>(
          pValuesForTheCurrentYear[numSpace]);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace], state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        for (int i = 0; i < ColCount; ++i)
        {
            if (AncestorType::isPrinted[i])
            {
                results.variableCaption = VCardType::Multiple::Caption(i);
                results.variableUnit = VCardType::Multiple::Unit(i);
                pValuesForTheCurrentYear[numSpace][i]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
            }
            results.isCurrentVarNA++;
        }
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel;
};

} // namespace Antares::Solver::Variable::Economy
