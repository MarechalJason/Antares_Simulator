// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/study/area/scratchpad.h>

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct DispatchableGenMarginTraits
{
    static std::string Caption()
    {
        return "DTG MRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dispatchable Generation Margin";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void setHourlyValue(IntermediateValues& yearlyValues,
                               detail::EmptyAuxiliaryData&,
                               const State& state,
                               unsigned int)
    {
        yearlyValues[state.hourInTheYear] += state.scratchpad
                                               ->dispatchableGenerationMargin[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardDispatchableGenMargin = VCard_Base<DispatchableGenMarginTraits>;

/*!
** \brief Marginal DispatchableGenMargin
*/
template<class NextT = Container::EndOfList>
using DispatchableGenMargin = Economy_Base<DispatchableGenMarginTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
