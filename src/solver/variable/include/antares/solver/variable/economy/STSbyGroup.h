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

    using ResultsProfile = StandardResults<>;
    using ResultsType = ResultsProfile;

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

    static std::vector<ColumnDescriptor> buildColumnDescriptors(Data::Study& study,
                                                                Data::Area* /*area*/)
    {
        std::set<std::string> uniqueGroups;
        study.areas.each(
          [&uniqueGroups](Data::Area& currentArea)
          {
              for (const auto& sts: currentArea.shortTermStorage.storagesByIndex)
              {
                  uniqueGroups.insert(sts.properties.groupName);
              }
          });

        std::vector<ColumnDescriptor> descriptors;
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
      [[maybe_unused]] const std::vector<ColumnDescriptor>& descriptors)
    {
        using namespace Antares::Data::ShortTermStorage;
        const auto& shortTermStorage = state.area->shortTermStorage;

        std::set<std::string> uniqueGroups;
        for (const auto& sts: shortTermStorage.storagesByIndex)
        {
            uniqueGroups.insert(sts.properties.groupName);
        }
        std::map<std::string, size_t> groupToNumbers;
        size_t groupIndex = 0;
        for (const auto& groupName: uniqueGroups)
        {
            groupToNumbers[groupName] = groupIndex++;
        }

        uint clusterIndex = 0;
        for (const auto& sts: shortTermStorage.storagesByIndex)
        {
            size_t groupNumber = groupToNumbers.at(sts.properties.groupName);
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

using STSbyGroup = DynamicMultiColumnBase<STSbyGroupTraits>;

} // namespace Antares::Solver::Variable::Economy
