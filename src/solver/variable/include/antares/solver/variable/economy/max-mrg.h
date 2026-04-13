// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/variable/variable.h"

#include "max-mrg-utils.h"

namespace Antares::Solver::Variable::Economy
{
struct VCardMARGE
{
    static std::string Caption()
    {
        return "MAX MRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Maximum margin throughout all MC years";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    typedef VCardMARGE VCardForSpatialAggregate;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t nodeDepthForGUI = +0;
    static constexpr uint8_t decimal = 0;
    static constexpr int columnCount = 1;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    static constexpr uint8_t hasIntermediateValues = 1;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef std::vector<IntermediateValues> IntermediateValuesType;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;
};

template<class NextT = Container::EndOfList>
class Marge: public Variable::IVariable<Marge<NextT>, NextT, VCardMARGE>
{
public:
    typedef NextT NextType;
    typedef VCardMARGE VCardType;
    typedef Variable::IVariable<Marge<NextT>, NextT, VCardType> AncestorType;

    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

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
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;
        InitializeResultsFromStudy(AncestorType::pResults, study);
        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }
        NextType::initializeFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        NextType::initializeFromArea(study, area);
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        pValuesForTheCurrentYear[numSpace].reset();
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        pValuesForTheCurrentYear[numSpace].computeStatisticsForTheCurrentYear();
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        NextType::hourForEachArea(state, numSpace);
    }

    void weekForEachArea(State& state, unsigned int numSpace)
    {
        double* rawhourly = Memory::RawPointer(pValuesForTheCurrentYear[numSpace].hour);
        MaxMrgUsualDataFactory maxMRGdataFactory(state, numSpace);
        MaxMRGinput maxMRGinput = maxMRGdataFactory.data();
        computeMaxMRG(rawhourly + state.hourInTheYear, maxMRGinput);
        NextType::weekForEachArea(state, numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;
        if (AncestorType::isPrinted[0])
        {
            results.variableCaption = VCardType::Caption();
            results.variableUnit = VCardType::Unit();
            pValuesForTheCurrentYear[numSpace]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    unsigned int pNbYearsParallel;
};

} // namespace Antares::Solver::Variable::Economy
