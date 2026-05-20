// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/logs/logs.h>

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct OperatingCostTraits
{
    static std::string Caption()
    {
        return "OP. COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Operating Cost throughout all MC years, of all the thermal dispatchable clusters";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    // This variable produces no hourly value: its results are accumulated at
    // year end in yearEndBuildForEachThermalCluster. We still implement an
    // explicit no-op setHourlyValue so the economy_base contract is satisfied
    // intentionally (rather than by a silent fallback), and we log it once.
    template<class AuxiliaryData>
    static void setHourlyValue(IntermediateValues& /*values*/,
                               AuxiliaryData& /*auxiliaryData*/,
                               const State& /*state*/,
                               unsigned int /*numSpace*/)
    {
        [[maybe_unused]] static const bool logged = []
        {
            Antares::logs.info() << "Variable '" << Caption()
                                 << "' has no hourly value (computed at year end)";
            return true;
        }();
    }

    static void yearEndBuildForEachThermalCluster(IntermediateValues& values,
                                                  State& state,
                                                  uint /*year*/,
                                                  unsigned int /*numSpace*/)
    {
        for (unsigned int i = state.study.runtime.rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            values[i] += state.thermalClusterOperatingCostForYear[i];
        }
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardOperatingCost = EconomyVariableCard<OperatingCostTraits>;

using OperatingCost = EconomyVariableBase<OperatingCostTraits>;

} // namespace Antares::Solver::Variable::Economy
