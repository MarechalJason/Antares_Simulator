// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct PriceTraits
{
    static std::string Caption()
    {
        return "MRG. PRICE";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Marginal Price, throughout all MC years";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 4;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateAverage;
    static constexpr uint8_t spatialAggregatePostProcessing
      = Category::spatialAggregatePostProcessingPrice;

    static double value(const State& state)
    {
        return -state.hourlyResults->CoutsMarginauxHoraires[state.hourInTheWeek];
    }

    static bool checkCondition(const State&)
    {
        return true;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeAveragesForCurrentYearFromHourlyResults();
    }
};

using VCardPrice = VCard_Base<PriceTraits>;

template<class NextT = Container::EndOfList>
using Price = Economy_Base<PriceTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
