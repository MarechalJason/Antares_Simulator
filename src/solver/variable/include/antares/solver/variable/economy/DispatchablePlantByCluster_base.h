// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file DispatchablePlantByCluster_base.h
**
** \brief Base class for dispatchable plant by cluster variables
**
** ## Traits Contract
**
** A valid DispatchablePlantByCluster Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - \c ResultsType : typedef for results template
**
** - Optional hooks (dispatched via \c if constexpr):
**   - \c AuxiliaryDataType : typedef
**   - \c initializeAuxiliaryData(AuxiliaryDataType&, Data::Study*, unsigned int, size_t) -> void
**   - \c yearBegin(AuxiliaryDataType&, unsigned int, unsigned int, size_t) -> void
**   - \c setHourlyValue(std::vector<IntermediateValues>&, AuxiliaryDataType&, State&, unsigned int)
*-> void
**   - Fallback: \c setHourlyValue(std::vector<IntermediateValues>&, State&, unsigned int) -> void
**   - Fallback: \c setHourlyValue(std::vector<IntermediateValues>&, State&) -> void
**   - \c
*yearEndBuildPrepareDataForEachThermalCluster(std::vector<std::vector<IntermediateValues>>&,
*AuxiliaryDataType&, State&, unsigned int, unsigned int) -> void
**   - \c yearEndBuildForEachThermalCluster(std::vector<std::vector<IntermediateValues>>&, State&,
*unsigned int, unsigned int) -> void
*/

#include <type_traits>

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

template<class Traits>
struct VCardDispatchablePlantByClusterBase
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
    typedef Results<R::AllYears::Average< // The average values throughout all years
      >>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardDispatchablePlantByClusterBase VCardForSpatialAggregate;

    //! Data Level
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    //! File level (provided by the type of the results)
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::de);
    //! Precision (views)
    static constexpr uint8_t precision = Category::all;
    //! Indentation (GUI)
    static constexpr uint8_t nodeDepthForGUI = +0;
    //! Decimal precision
    static constexpr uint8_t decimal = 0;
    //! Number of columns used by the variable
    static constexpr int columnCount = Category::dynamicColumns;
    //! The Spatial aggregation
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    //! Intermediate values
    static constexpr uint8_t hasIntermediateValues = 1;
    //! Can this variable be non applicable (0 : no, 1 : yes)
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef std::vector<IntermediateValues> IntermediateValuesBaseType;
    typedef std::vector<IntermediateValuesBaseType> IntermediateValuesType;

}; // class VCardDispatchablePlantByClusterBase

template<class Traits>
class DispatchablePlantByClusterBase
    : public Variable::IVariable<DispatchablePlantByClusterBase<Traits>,
                                 void,
                                 VCardDispatchablePlantByClusterBase<Traits>>
{
public:
    typedef VCardDispatchablePlantByClusterBase<Traits> VCardType;
    typedef Variable::IVariable<DispatchablePlantByClusterBase<Traits>, void, VCardType>
      AncestorType;

    typedef typename VCardType::ResultsType ResultsType;

    typedef VariableAccessor<ResultsType, VCardType::columnCount> VariableAccessorType;

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
    void initializeFromStudy(Data::Study& study)
    {
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        pSize = area->thermal.list.enabledCount();
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                pValuesForTheCurrentYear[numSpace].resize(pSize);
            }

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; numSpace++)
            {
                for (unsigned int i = 0; i != pSize; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (unsigned int i = 0; i != pSize; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            AncestorType::pResults.clear();
        }

        initializeAuxiliaryDataIfSupported(auxiliaryData_, study, pNbYearsParallel, pSize);
    }

    size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
    }

    void simulationBegin()
    {
    }

    void simulationEnd()
    {
    }

    void yearBegin(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }

        yearBeginIfSupported(auxiliaryData_, year, numSpace, pSize);
    }

    void yearEndBuild(State& state, unsigned int year, unsigned int numSpace)
    {
    }

    void yearEndBuildPrepareDataForEachThermalCluster(State& state,
                                                      uint year,
                                                      unsigned int numSpace)
    {
        yearEndBuildPrepareDataForEachThermalClusterIfSupported(pValuesForTheCurrentYear,
                                                                auxiliaryData_,
                                                                state,
                                                                year,
                                                                numSpace);
    }

    void yearEndBuildForEachThermalCluster(State& state, uint year, unsigned int numSpace)
    {
        yearEndBuildForEachThermalClusterIfSupported(pValuesForTheCurrentYear,
                                                     state,
                                                     year,
                                                     numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
        }
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < pSize; ++i)
        {
            AncestorType::pResults[i].merge(year, pValuesForTheCurrentYear[numSpace][i]);
        }
    }

    void hourBegin(unsigned int hourInTheYear)
    {
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        setHourlyValueIfSupported(pValuesForTheCurrentYear[numSpace],
                                  auxiliaryData_,
                                  state,
                                  numSpace);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        if constexpr (requires {
                          Traits::retrieveRawHourlyValuesForCurrentYear(pValuesForTheCurrentYear,
                                                                        column,
                                                                        numSpace);
                      })
        {
            return Traits::retrieveRawHourlyValuesForCurrentYear(pValuesForTheCurrentYear,
                                                                 column,
                                                                 numSpace);
        }

        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0])
        {
            assert(NULL != results.data.area);
            const auto& thermal = results.data.area->thermal;

            for (auto& cluster: thermal.list.each_enabled())
            {
                results.variableCaption = cluster->name();
                results.variableUnit = VCardType::Unit();
                pValuesForTheCurrentYear[numSpace][cluster->enabledIndex]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
            }
        }
    }

protected:
    using AuxiliaryDataType = typename detail::AuxiliaryDataType<Traits>::type;

    static void setHourlyValueIfSupported(std::vector<IntermediateValues>& clusterValues,
                                          AuxiliaryDataType& auxiliaryData,
                                          State& state,
                                          unsigned int numSpace)
    {
        if constexpr (requires {
                          Traits::setHourlyValue(clusterValues, auxiliaryData, state, numSpace);
                      })
        {
            Traits::setHourlyValue(clusterValues, auxiliaryData, state, numSpace);
        }
        else if constexpr (requires { Traits::setHourlyValue(clusterValues, state, numSpace); })
        {
            Traits::setHourlyValue(clusterValues, state, numSpace);
        }
    }

    static void initializeAuxiliaryDataIfSupported(AuxiliaryDataType& auxiliaryData,
                                                   Data::Study* study,
                                                   unsigned int nbYearsParallel,
                                                   size_t nbClusters)
    {
        if constexpr (requires {
                          Traits::initializeAuxiliaryData(auxiliaryData,
                                                          study,
                                                          nbYearsParallel,
                                                          nbClusters);
                      })
        {
            Traits::initializeAuxiliaryData(auxiliaryData, study, nbYearsParallel, nbClusters);
        }
    }

    static void yearBeginIfSupported(AuxiliaryDataType& auxiliaryData,
                                     unsigned int year,
                                     unsigned int numSpace,
                                     size_t nbClusters)
    {
        if constexpr (requires { Traits::yearBegin(auxiliaryData, year, numSpace, nbClusters); })
        {
            Traits::yearBegin(auxiliaryData, year, numSpace, nbClusters);
        }
    }

    static void yearEndBuildPrepareDataForEachThermalClusterIfSupported(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      AuxiliaryDataType& auxiliaryData,
      State& state,
      uint year,
      unsigned int numSpace)
    {
        if constexpr (requires {
                          Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                               auxiliaryData,
                                                                               state,
                                                                               year,
                                                                               numSpace);
                      })
        {
            Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                 auxiliaryData,
                                                                 state,
                                                                 year,
                                                                 numSpace);
        }
        else if constexpr (requires {
                               Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                                    state,
                                                                                    year,
                                                                                    numSpace);
                           })
        {
            Traits::yearEndBuildPrepareDataForEachThermalCluster(yearlyValues,
                                                                 state,
                                                                 year,
                                                                 numSpace);
        }
    }

    static void yearEndBuildForEachThermalClusterIfSupported(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      State& state,
      uint year,
      unsigned int numSpace)
    {
        if constexpr (requires {
                          Traits::yearEndBuildForEachThermalCluster(yearlyValues,
                                                                    state,
                                                                    year,
                                                                    numSpace);
                      })
        {
            Traits::yearEndBuildForEachThermalCluster(yearlyValues, state, year, numSpace);
        }
    }

    //! Intermediate values for each year
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    AuxiliaryDataType auxiliaryData_;
    size_t pSize = 0;
    unsigned int pNbYearsParallel = 0;

}; // class DispatchablePlantByClusterBase

} // namespace Antares::Solver::Variable::Economy
