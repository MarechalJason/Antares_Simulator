// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct PriceCSRTraits
{
    static std::string Caption()
    {
        return "MRG. PRICE CSR";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Marginal Price CSR, throughout all MC years";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 4;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateAverage;
    static constexpr uint8_t spatialAggregatePostProcessing
      = Category::spatialAggregatePostProcessingPrice;

    using AuxiliaryDataType = detail::EmptyAuxiliaryData;

    static double value(const State& state)
    {
        return -state.hourlyResults->CoutsMarginauxHorairesCSR[state.hourInTheWeek];
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

using VCardPriceCSR = VCard_Base<PriceCSRTraits>;

using PriceCSR = Economy_Base<PriceCSRTraits>;

} // namespace Antares::Solver::Variable::Economy
