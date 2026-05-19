// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct OverflowsTraits
{
    static std::string Caption()
    {
        return "H. OVFL";
    }

    static std::string Unit()
    {
        return "%";
    }

    static std::string Description()
    {
        return "Hydro overflow";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;
    static constexpr uint8_t isPossiblyNonApplicable = 1;

    static void setHourlyValue(IntermediateValues& intermediateValues,
                               typename detail::AuxiliaryDataType<OverflowsTraits>::type&,
                               const State& state,
                               unsigned int)
    {
        intermediateValues.hour[state.hourInTheYear] = state.hourlyResults->debordementsHoraires
                                                         [state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardOverflow = VCard_Base<OverflowsTraits>;

template<class NextT = Container::EndOfList>
using Overflows = Economy_Base<OverflowsTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
