// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

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
    typedef VCardMultiColumn VCardForSpatialAggregate;

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

    typedef IntermediateValues IntermediateValuesBaseType[columnCount];
    typedef IntermediateValuesBaseType* IntermediateValuesType;

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

template<class Traits, int ColCount, class NextT = Container::EndOfList>
class MultiColumnBase: public Variable::IVariable<MultiColumnBase<Traits, ColCount, NextT>,
                                                  NextT,
                                                  VCardMultiColumn<Traits, ColCount>>
{
public:
    typedef NextT NextType;
    typedef VCardMultiColumn<Traits, ColCount> VCardType;
    typedef Variable::IVariable<MultiColumnBase<Traits, ColCount, NextT>, NextT, VCardType>
      AncestorType;
    typedef typename VCardType::ResultsType ResultsType;
    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

    enum
    {
        count = 1 + NextT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = ((VCardType::categoryDataLevel & CDataLevel
                      && VCardType::categoryFileLevel & CFile)
                       ? (NextType::template Statistics<CDataLevel, CFile>::count
                          + VCardType::columnCount * ResultsType::count)
                       : NextType::template Statistics<CDataLevel, CFile>::count),
        };
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

        NextType::initializeFromStudy(study);
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
        NextType::initializeFromArea(study, area);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        if constexpr (requires {
                          Traits::onSimulationBegin(pValuesForTheCurrentYear, pNbYearsParallel);
                      })
        {
            Traits::onSimulationBegin(pValuesForTheCurrentYear, pNbYearsParallel);
        }
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        for (int i = 0; i < ColCount; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        VariableAccessorType::template ComputeStatistics<VCardType>(
          pValuesForTheCurrentYear[numSpace]);
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace], state, numSpace);
        NextType::hourForEachArea(state, numSpace);
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
