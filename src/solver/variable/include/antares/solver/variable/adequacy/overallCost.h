// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__
#define __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__

#include "antares/solver/variable/economy/economy_base.h"

namespace Antares::Solver::Variable
{
namespace Adequacy
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

    using ResultsProfile = Results<std::tuple<R::AllYears::Average>, R::AllYears::Average>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    template<class Aux>
    static void setHourlyValue(IntermediateValues& iv, Aux&, State& state, unsigned int)
    {
        auto area = state.area;
        auto& thermal = state.thermal;

        iv[state.hourInTheYear] +=
          (state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek]
           * area->thermal.unsuppliedEnergyCost)
          + ((state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek]
              + state.resSpilled.entry[area->index][state.hourInTheWeek])
             * area->thermal.spilledEnergyCost);

        // Hydro costs : water value and pumping
        iv.hour[state.hourInTheYear]
          += state.problemeHebdo->CaracteristiquesHydrauliques[state.area->index]
               .WeeklyWaterValueStateRegular
             * (state.hourlyResults->TurbinageHoraire[state.hourInTheWeek]
                - area->hydro.pumpingEfficiency
                    * state.hourlyResults->PompageHoraire[state.hourInTheWeek]);

        // Thermal costs
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            iv[state.hourInTheYear]
              += thermal[area->index].thermalClustersOperatingCost[cluster->enabledIndex];
        }
    }

    static void yearEndBuildForEachThermalCluster(IntermediateValues& values,
                                                  State& state,
                                                  uint /*year*/,
                                                  unsigned int /*numSpace*/)
    {
        for (unsigned int i = 0; i < state.study.runtime.rangeLimits.hour[Data::rangeCount]; ++i)
        {
            values[i] += state.thermalClusterOperatingCostForYear[i];
        }
    }
};

using VCardOverallCost = Economy::EconomyVariableCard<OverallCostTraits>;

/*!
** \brief Overall cost over all MC years (adequacy)
*/
using OverallCost = Economy::EconomyVariableBase<OverallCostTraits>;

} // namespace Adequacy
} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__
