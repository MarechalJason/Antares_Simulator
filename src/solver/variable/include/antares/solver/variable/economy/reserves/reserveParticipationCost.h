/*
** Copyright 2007-2025 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

#include "antares/solver/variable/economy/economy_base.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

struct ReserveParticipationCostTraits
{
    static std::string Caption()
    {
        return "RESERVE PARTICIPATION COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Reserve Participation Cost throughout all MC years of all clusters : Thermal, "
               "hydro and short term";
    }

    using ResultsProfile = StandardResults<R::AllYears::Average>;

    static constexpr uint8_t decimal = 2;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    template<class AuxiliaryData>
    static void setHourlyValue(IntermediateValues& values,
                               AuxiliaryData& /*auxiliaryData*/,
                               const State& state,
                               unsigned int /*numSpace*/)
    {
        if (state.reserveData)
        {
            values[state.hourInTheYear] = state.reserveData.value()
                                            .at(state.area->index)
                                            .reserveParticipationCostForYear[state.hourInTheYear];
        }
        else
        {
            values[state.hourInTheYear] = 0;
        }
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }
};

using ReserveParticipationCost = EconomyVariableBase<ReserveParticipationCostTraits>;

} // namespace Antares::Solver::Variable::Economy::Reserves
