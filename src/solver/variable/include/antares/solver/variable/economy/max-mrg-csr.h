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

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

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

using VCardMAX_MRG_CSR = VCard_Base<MaxMargeCsrTraits>;

template<class NextT = Container::EndOfList>
using MaxMrgCsr = Economy_Base<MaxMargeCsrTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
