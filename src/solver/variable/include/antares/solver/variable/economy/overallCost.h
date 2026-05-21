// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_OverallCost_H__
#define __SOLVER_VARIABLE_ECONOMY_OverallCost_H__

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct OverallCostTraits
{
    static std::string Caption()
    {
        return "OV. COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Overall Cost throughout all MC years";
    }

    typedef Results<R::AllYears::Average< // The average values throughout all years
                      >,
                    R::AllYears::Average // Use these values for spatial cluster
                    >
      ResultsType;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    template<class Aux>
    static void setHourlyValue(IntermediateValues& iv, Aux&, const State& state, unsigned int)
    {
        const double costForSpilledOrUnsuppliedEnergy
          = (state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek]
             * state.area->thermal.unsuppliedEnergyCost)
            + (state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek]
               * state.area->thermal.spilledEnergyCost);

        iv[state.hourInTheYear] += costForSpilledOrUnsuppliedEnergy;
        // state is logically non-const; annualSystemCost is a side-channel accumulator.
        const_cast<State&>(state).annualSystemCost += costForSpilledOrUnsuppliedEnergy;
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
            state.annualSystemCost += state.thermalClusterOperatingCostForYear[i];
        }
    }
};

using VCardOverallCost = EconomyVariableCard<OverallCostTraits>;

/*!
** \brief Overall cost over all MC years
*/
using OverallCost = EconomyVariableBase<OverallCostTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_OverallCost_H__
