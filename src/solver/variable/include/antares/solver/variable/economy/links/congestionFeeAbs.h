// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_CongestionFeeAbs_H__
#define __SOLVER_VARIABLE_ECONOMY_CongestionFeeAbs_H__

#include <cmath>

#include "links_base.h"

namespace Antares::Solver::Variable::Economy
{
struct CongestionFeeAbsTraits
{
    static std::string Caption()
    {
        return "CONG. FEE (ABS.)";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Congestion fee collected throughout all MC years (Absolute value)";
    }

    typedef Results<R::AllYears::Average<    // The average values throughout all years
      R::AllYears::StdDeviation<             // The standard deviation values throughout all years
        R::AllYears::Min<                    // The minimum values throughout all years
          R::AllYears::Max<                  // The maximum values throughout all years
            >>>>>
      ResultsType;

    static constexpr uint8_t decimal = 0;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static double computeHourlyValue(State& state, double upstreamPrice, double downstreamPrice)
    {
        return std::abs(state.ntc.ValeurDuFlux[state.link->index] * (upstreamPrice - downstreamPrice));
    }
};

/*!
** \brief Marginal CongestionFeeAbs
*/
template<class NextT = Container::EndOfList>
using CongestionFeeAbs = EconomyLink_Base<CongestionFeeAbsTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_CongestionFeeAbs_H__
