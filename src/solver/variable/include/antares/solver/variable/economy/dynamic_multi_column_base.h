// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

/*!
** \file dynamic_multi_column_base.h
**
** \brief Base class for dynamic multi-column variables (runtime column count)
**
** ## Traits Contract
**
** A valid DynamicMultiColumn Traits must provide:
** - Required static methods:
**   - \c Caption() -> std::string
**   - \c Unit() -> std::string
**   - \c Description() -> std::string
**   - \c ResultsType : typedef for results template
**   - \c decimal : uint8_t
**   - \c buildColumnDescriptors(Data::Area*) -> std::vector<ColumnDescriptor>
**
** - Optional hooks:
**   - \c onSimulationBegin(IntermediateValuesBaseType&, unsigned int) -> void
**   - \c perColumnComputeStats(IntermediateValues&, size_t columnIndex) -> void
**   - \c setHourlyValue(IntermediateValuesBaseType&, State&, unsigned int, const
*std::vector<ColumnDescriptor>&) -> void
**
** Note: The traits should rebuild groupToNumbers map internally from descriptors if needed.
*/

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct ColumnDescriptor
{
    std::string caption;
    std::string unit;
};

template<class Traits>
struct VCardDynamicMultiColumn
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
    typedef VCardDynamicMultiColumn VCardForSpatialAggregate;

    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    static constexpr uint8_t precision = Category::all;
    static constexpr uint8_t nodeDepthForGUI = +0;
    static constexpr uint8_t decimal = Traits::decimal;
    static constexpr int columnCount = Category::dynamicColumns;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    static constexpr uint8_t hasIntermediateValues = 1;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef std::vector<IntermediateValues> IntermediateValuesBaseType;
    typedef std::vector<IntermediateValuesBaseType> IntermediateValuesType;

    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesDeepType[]>;

    struct Multiple
    {
        static std::string Caption(unsigned int indx,
                                   const std::vector<ColumnDescriptor>& descriptors)
        {
            return indx < descriptors.size() ? descriptors[indx].caption : "<unknown>";
        }

        static std::string Unit(unsigned int indx, const std::vector<ColumnDescriptor>& descriptors)
        {
            return indx < descriptors.size() ? descriptors[indx].unit : Traits::Unit();
        }
    };
};

template<class Traits, class NextT = Container::EndOfList>
class DynamicMultiColumnBase: public Variable::IVariable<DynamicMultiColumnBase<Traits, NextT>,
                                                         NextT,
                                                         VCardDynamicMultiColumn<Traits>>
{
public:
    typedef NextT NextType;
    typedef VCardDynamicMultiColumn<Traits> VCardType;
    typedef Variable::IVariable<DynamicMultiColumnBase<Traits, NextT>, NextT, VCardType>
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
    void initializeFromStudy(Data::Study& study)
    {
        pNbYearsParallel = study.maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        AncestorType::pResults.clear();
        AncestorType::pResults.resize(nbColumns_);
        for (unsigned int i = 0; i < nbColumns_; ++i)
        {
            AncestorType::pResults[i].initializeFromStudy(study);
            AncestorType::pResults[i].reset();
        }

        NextType::initializeFromStudy(study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        auto descriptors = Traits::buildColumnDescriptors(area);
        nbColumns_ = descriptors.size();
        descriptors_ = std::move(descriptors);

        groupToNumbers_.clear();
        for (size_t i = 0; i < nbColumns_; ++i)
        {
            groupToNumbers_[descriptors_[i].caption] = i;
        }

        pValuesForTheCurrentYear.resize(pNbYearsParallel);
        for (auto& vec: pValuesForTheCurrentYear)
        {
            vec.resize(nbColumns_);
        }

        for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
        {
            for (size_t col = 0; col < nbColumns_; ++col)
            {
                pValuesForTheCurrentYear[numSpace][col].initializeFromStudy(*study);
            }
        }

        AncestorType::pResults.resize(nbColumns_);
        for (unsigned int i = 0; i < nbColumns_; ++i)
        {
            AncestorType::pResults[i].initializeFromStudy(*study);
            AncestorType::pResults[i].reset();
        }

        NextType::initializeFromArea(study, area);
    }

    size_t getMaxNumberColumns() const
    {
        return nbColumns_ * ResultsType::count;
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
        for (size_t i = 0; i < nbColumns_; ++i)
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
        for (size_t column = 0; column < nbColumns_; ++column)
        {
            if constexpr (requires {
                              Traits::perColumnComputeStats(
                                pValuesForTheCurrentYear[numSpace][column],
                                column);
                          })
            {
                Traits::perColumnComputeStats(pValuesForTheCurrentYear[numSpace][column], column);
            }
            else
            {
                pValuesForTheCurrentYear[numSpace][column].computeStatisticsForTheCurrentYear();
            }
        }
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
        Traits::setHourlyValue(pValuesForTheCurrentYear[numSpace], state, numSpace, descriptors_);
        NextType::hourForEachArea(state, numSpace);
    }

    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        NextType::buildDigest(results, digestLevel, dataLevel);
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

        if (!AncestorType::isPrinted[0])
        {
            return;
        }

        for (size_t column = 0; column < nbColumns_; ++column)
        {
            results.variableCaption = VCardType::Multiple::Caption(column, descriptors_);
            results.variableUnit = VCardType::Multiple::Unit(column, descriptors_);
            pValuesForTheCurrentYear[numSpace][column]
              .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
        }
    }

    void buildSurveyReport(SurveyResults& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        if (!AncestorType::isPrinted[0])
        {
            return;
        }

        if ((dataLevel & VCardType::categoryDataLevel) && (fileLevel & VCardType::categoryFileLevel)
            && (precision & VCardType::precision))
        {
            results.isCurrentVarNA[0] = AncestorType::isNonApplicable[0];

            for (size_t column = 0; column < nbColumns_; ++column)
            {
                results.variableCaption = descriptors_[column].caption;
                results.variableUnit = descriptors_[column].unit;
                AncestorType::pResults[column].template buildSurveyReport<ResultsType, VCardType>(
                  results,
                  AncestorType::pResults[column],
                  dataLevel,
                  fileLevel,
                  precision);
            }
        }
        NextType::buildSurveyReport(results, dataLevel, fileLevel, precision);
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    std::vector<ColumnDescriptor> descriptors_;
    std::map<std::string, size_t> groupToNumbers_;
    size_t nbColumns_ = 0;
    unsigned int pNbYearsParallel;
};

} // namespace Antares::Solver::Variable::Economy
