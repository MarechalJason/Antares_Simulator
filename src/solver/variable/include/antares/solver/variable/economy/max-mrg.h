// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"
#include "max-mrg-utils.h"

namespace Antares::Solver::Variable::Economy
{

struct MaxMargeTraits
{
    static std::string Caption()
    {
        return "MAX MRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Maximum margin throughout all MC years";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void weekForEachArea(IntermediateValues& iv, State& state, unsigned int numSpace)
    {
        double* rawhourly = Memory::RawPointer(iv.hour);
        MaxMrgUsualDataFactory maxMRGdataFactory(state, numSpace);
        MaxMRGinput maxMRGinput = maxMRGdataFactory.data();
        computeMaxMRG(rawhourly + state.hourInTheYear, maxMRGinput);
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardMARGE = VCard_Base<MaxMargeTraits>;

using Marge = Economy_Base<MaxMargeTraits>;

} // namespace Antares::Solver::Variable::Economy
