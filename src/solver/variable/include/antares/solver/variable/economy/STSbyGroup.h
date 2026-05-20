// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "dynamic_multi_column_base.h"
#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

namespace STS
{
enum VariableType : int
{
    injection = 0,
    withdrawal = 1,
    level = 2,
    NB_COLS_PER_GROUP = 3
};
} // namespace STS

struct STSbyGroupTraits
{
    static std::string Caption()
    {
        return "STS by group";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "STS injections, withdrawals and levels";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;

    static std::vector<ColumnDescriptor> buildColumnDescriptors(Data::Area* area)
    {
        std::vector<ColumnDescriptor> descriptors;
        std::set<std::string> uniqueGroups;

        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            uniqueGroups.insert(sts.properties.groupName);
        }

        for (const auto& groupName: uniqueGroups)
        {
            descriptors.push_back({groupName + "_INJECTION", "MW"});
            descriptors.push_back({groupName + "_WITHDRAWAL", "MW"});
            descriptors.push_back({groupName + "_LEVEL", "MWh"});
        }
        return descriptors;
    }

    static void setHourlyValue(
      VCardDynamicMultiColumn<STSbyGroupTraits>::IntermediateValuesBaseType& pValues,
      State& state,
      unsigned int,
      const std::vector<ColumnDescriptor>& descriptors)
    {
        using namespace Antares::Data::ShortTermStorage;
        const auto& shortTermStorage = state.area->shortTermStorage;

        std::map<std::string, size_t> groupToNumbers;
        const std::string injectionSuffix = "_INJECTION";
        for (size_t i = 0; i < descriptors.size(); i += STS::NB_COLS_PER_GROUP)
        {
            const auto& caption = descriptors[i].caption;
            if (caption.size() > injectionSuffix.size() && caption.ends_with(injectionSuffix))
            {
                groupToNumbers[caption.substr(0, caption.size() - injectionSuffix.size())]
                  = i / STS::NB_COLS_PER_GROUP;
            }
        }

        uint clusterIndex = 0;
        for (const auto& sts: shortTermStorage.storagesByIndex)
        {
            const auto groupIt = groupToNumbers.find(sts.properties.groupName);
            if (groupIt == groupToNumbers.end())
            {
                ++clusterIndex;
                continue;
            }
            const size_t groupNumber = groupIt->second;
            const auto& result = state.hourlyResults->ShortTermStorage[clusterIndex];

            pValues[STS::NB_COLS_PER_GROUP * groupNumber + STS::VariableType::injection]
                   [state.hourInTheYear]
              += result.injection[state.hourInTheWeek];

            pValues[STS::NB_COLS_PER_GROUP * groupNumber + STS::VariableType::withdrawal]
                   [state.hourInTheYear]
              += result.withdrawal[state.hourInTheWeek];

            pValues[STS::NB_COLS_PER_GROUP * groupNumber + STS::VariableType::level]
                   [state.hourInTheYear]
              += result.level[state.hourInTheWeek];

            clusterIndex++;
        }
    }

    static void perColumnComputeStats(IntermediateValues& iv, size_t column)
    {
        if (column % STS::NB_COLS_PER_GROUP == STS::VariableType::level)
        {
            iv.computeAveragesForCurrentYearFromHourlyResults();
        }
        else
        {
            iv.computeStatisticsForTheCurrentYear();
        }
    }
};

using VCardSTSbyGroup = VCardDynamicMultiColumn<STSbyGroupTraits>;

template<class NextT = Container::EndOfList>
using STSbyGroup = DynamicMultiColumnBase<STSbyGroupTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
