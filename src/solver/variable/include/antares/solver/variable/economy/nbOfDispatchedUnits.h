// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_NbOfDispatchedUnits_H__
#define __SOLVER_VARIABLE_ECONOMY_NbOfDispatchedUnits_H__

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct NbOfDispatchedUnitsTraits
{
    static std::string Caption()
    {
        return "NODU";
    }

    static std::string Unit()
    {
        return " ";
    }

    static std::string Description()
    {
        return "Number Of Dispatched Units throughout all MC years, of all the thermal "
               "dispatchable clusters";
    }

    typedef Results<R::AllYears::Average<        // The average values throughout all years
                      R::AllYears::StdDeviation< // The standard deviation values throughout all
                                                 // years
                        R::AllYears::Min<        // The minimum values throughout all years
                          R::AllYears::Max<      // The maximum values throughout all years
                            >>>>,
                    R::AllYears::Average // The
                    >
      ResultsType;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
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
            values[i] += state.thermalClusterDispatchedUnitsCountForYear[i];
        }
    }
};

using VCardNbOfDispatchedUnits = EconomyVariableCard<NbOfDispatchedUnitsTraits>;

/*!
** \brief Number of dispatched units over all thermal clusters
*/
using NbOfDispatchedUnits = EconomyVariableBase<NbOfDispatchedUnitsTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_NbOfDispatchedUnits_H__
