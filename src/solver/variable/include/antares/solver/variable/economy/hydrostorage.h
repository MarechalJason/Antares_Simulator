// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct HydroStorageTraits
{
    static std::string Caption()
    {
        return "H. STOR";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Hydro Storage Generation";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static bool checkCondition(const State&)
    {
        return true;
    }

    static double value(const State& state)
    {
        return state.hourlyResults->TurbinageHoraire[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardHydroStorage = VCard_Base<HydroStorageTraits>;

/*!
** \brief C02 Average value of the overrall HydroStorage emissions expected from all
**   the thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
using HydroStorage = Economy_Base<HydroStorageTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
