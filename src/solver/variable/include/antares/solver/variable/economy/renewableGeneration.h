// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "dynamic_multi_column_base.h"
#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct RenewableGenerationTraits
{
    static std::string Caption()
    {
        return "Renewable Gen.";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Value of all the renewable generation throughout all MC years";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;

    static std::vector<ColumnDescriptor> buildColumnDescriptors(Data::Area* area)
    {
        std::vector<ColumnDescriptor> descriptors;
        std::map<std::string, size_t> groupNumbers;

        for (auto& cluster: area->renewable.list.each_enabled())
        {
            const std::string& group = cluster->getGroup();
            if (groupNumbers.find(group) == groupNumbers.end())
            {
                size_t idx = descriptors.size();
                groupNumbers[group] = idx;
                descriptors.push_back({group, "MWh"});
            }
        }
        return descriptors;
    }

    static void setHourlyValuesWithDescriptors(
      VCardDynamicMultiColumn<RenewableGenerationTraits>::IntermediateValuesBaseType& pValues,
      State& state,
      unsigned int,
      [[maybe_unused]] const std::vector<ColumnDescriptor>&,
      const std::map<std::string, size_t>& groupToNumbers)
    {
        auto* area = state.area;
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            size_t groupNumber = groupToNumbers.at(cluster->getGroup());
            double production = cluster->valueAtTimeStep(state.year, state.hourInTheYear);
            pValues[groupNumber][state.hourInTheYear] += production;
        }
    }
};

using VCardRenewableGeneration = VCardDynamicMultiColumn<RenewableGenerationTraits>;

template<class NextT = Container::EndOfList>
using RenewableGeneration = DynamicMultiColumnBase<RenewableGenerationTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
