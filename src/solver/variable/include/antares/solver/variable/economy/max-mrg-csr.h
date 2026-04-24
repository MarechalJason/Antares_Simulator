// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"
#include "max-mrg-utils.h"

namespace Antares::Solver::Variable::Economy
{

struct MaxMargeCsrTraits
{
    static std::string Caption()
    {
        return "MAX MRG CSR";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Max margin for CSR";
    }

    using ResultsType = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void weekForEachArea(IntermediateValues& iv, State& state, unsigned int numSpace)
    {
        double* rawhourly = Memory::RawPointer(iv.hour);
        MaxMrgCSRdataFactory maxMRGcsrDataFactory(state, numSpace);
        MaxMRGinput maxMRGinput = maxMRGcsrDataFactory.data();
        computeMaxMRG(rawhourly + state.hourInTheYear, maxMRGinput);
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardMAX_MRG_CSR = EconomyVariableCard<MaxMargeCsrTraits>;

using MaxMrgCsr = EconomyVariableBase<MaxMargeCsrTraits>;

} // namespace Antares::Solver::Variable::Economy
