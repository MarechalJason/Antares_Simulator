// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_CongestionFeeAbs_H__
#define __SOLVER_VARIABLE_ECONOMY_CongestionFeeAbs_H__

#include <cmath>

#include "congestionFee_base.h"

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

    static double computeHourlyValue(State& state, double upstreamPrice, double downstreamPrice)
    {
        return std::abs(state.ntc.ValeurDuFlux[state.link->index]
                        * (upstreamPrice - downstreamPrice));
    }
};

/*!
** \brief Marginal CongestionFeeAbs
*/
template<class NextT = Container::EndOfList>
using CongestionFeeAbs = CongestionFee_Base<CongestionFeeAbsTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_CongestionFeeAbs_H__
