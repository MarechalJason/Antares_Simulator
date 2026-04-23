// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/solver/variable/variable.h>

namespace Antares::Solver::Variable::Economy
{

template<class Traits>
struct VCard_LinkBase
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

    //! The expecte results
    typedef typename Traits::ResultsType ResultsType;

    typedef VCard_LinkBase VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::link;
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
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesBaseType;
    typedef std::vector<IntermediateValues> IntermediateValuesType;

}; // class VCard

template<class Traits, class NextT = Container::EndOfList>
class EconomyLink_Base
    : public Variable::IVariable<EconomyLink_Base<Traits, NextT>, NextT, VCard_LinkBase<Traits>>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCard_LinkBase<Traits> VCardType;
    //! Ancestor
    typedef Variable::IVariable<EconomyLink_Base<Traits, NextT>, NextT, VCardType> AncestorType;

    //! List of expected results
    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

    enum
    {
        //! How many items have we got
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

        // Average on all years
        AncestorType::pResults.initializeFromStudy(study);
        AncestorType::pResults.reset();

        // Intermediate values
        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].initializeFromStudy(study);
        }

        // Next
        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        // Next
        NextType::initializeFromArea(study, area);
    }

    void initializeFromAreaLink(Data::Study* study, Data::AreaLink* link)
    {
        // Next
        NextType::initializeFromAreaLink(study, link);
    }

    void simulationBegin()
    {
        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
        {
            pValuesForTheCurrentYear[numSpace].reset();
        }
        // Next
        NextType::simulationBegin();
    }

    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    void yearBegin(uint year, unsigned int numSpace)
    {
        // Reset
        pValuesForTheCurrentYear[numSpace].reset();
        // Next variable
        NextType::yearBegin(year, numSpace);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
        // Next variable
        NextType::yearEndBuild(state, year, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        // Compute all statistics for the current year (daily,weekly,monthly)
        Traits::computeStats(pValuesForTheCurrentYear[numSpace]);

        // Next variable
        NextType::yearEnd(year, numSpace);
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        // Merge all those values with the global results
        AncestorType::pResults.merge(year, pValuesForTheCurrentYear[numSpace]);

        // Next variable
        NextType::computeSummary(year, numSpace);
    }

    void hourBegin(uint hourInTheYear)
    {
        // Next variable
        NextType::hourBegin(hourInTheYear);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        // Next variable
        NextType::hourForEachArea(state, numSpace);
    }

    void hourForEachLink(State& state, unsigned int numSpace)
    {
        if constexpr (requires {
                          Traits::hourForEachLink(pValuesForTheCurrentYear[numSpace], state);
                      })
        {
            Traits::hourForEachLink(pValuesForTheCurrentYear[numSpace], state);
        }
        else if constexpr (requires { Traits::hourValue(state); })
        {
            pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear] += Traits::hourValue(
              state);
        }
        else if constexpr (requires { Traits::hourValue(state, 0.0, 0.0); })
        {
            const double upstreamPrice = state.problemeHebdo
                                           ->ResultatsHoraires[state.link->from->index]
                                           .CoutsMarginauxHoraires[state.hourInTheWeek];
            const double downstreamPrice = state.problemeHebdo
                                             ->ResultatsHoraires[state.link->with->index]
                                             .CoutsMarginauxHoraires[state.hourInTheWeek];
            pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear] = Traits::hourValue(
              state,
              upstreamPrice,
              downstreamPrice);
        }
        // Next item in the list
        NextType::hourForEachLink(state, numSpace);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        if constexpr (requires {
                          Traits::buildDigest(results,
                                              digestLevel,
                                              dataLevel,
                                              AncestorType::pResults);
                      })
        {
            Traits::buildDigest(results, digestLevel, dataLevel, AncestorType::pResults);
        }
        // Next
        NextType::buildDigest(results, digestLevel, dataLevel);
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
    unsigned int pNbYearsParallel;

}; // class EconomyLink_Base

} // namespace Antares::Solver::Variable::Economy
