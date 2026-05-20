// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct DtgMarginCsrTraits
{
    static std::string Caption()
    {
        return "DTG MRG CSR";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dispatchable Generation Margin (after CSR optimization)";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = detail::EmptyAuxiliaryData;

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDtgMrgCsr[state.hourInTheWeek];
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
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardDtgMarginCsr = EconomyVariableCard<DtgMarginCsrTraits>;

using DtgMarginCsr = EconomyVariableBase<DtgMarginCsrTraits>;

} // namespace Antares::Solver::Variable::Economy
