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

    using ResultsType = StandardResults<>;

    static constexpr uint8_t decimal = 4;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateAverage;

    using AuxiliaryDataType = detail::EmptyAuxiliaryData;

    static double value(const State& state)
    {
        return -state.hourlyResults->CoutsMarginauxHoraires[state.hourInTheWeek];
    }

    static void setHourlyValue(IntermediateValues& iv,
                               AuxiliaryDataType&,
                               const State& state,
                               unsigned int)
    {
        iv[state.hourInTheYear] = value(state);
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeAveragesForCurrentYearFromHourlyResults();
    }
};

using VCardPrice = EconomyVariableCard<PriceTraits>;

using Price = EconomyVariableBase<PriceTraits>;

} // namespace Antares::Solver::Variable::Economy
