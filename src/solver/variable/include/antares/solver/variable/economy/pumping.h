// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct PumpingTraits
{
    static std::string Caption()
    {
        return "H. PUMP";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Hydraulic pumping";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = Antares::Data::Area*;

    static void initializeFromArea(AuxiliaryDataType& area, Data::Study*, Data::Area* inputArea)
    {
        area = inputArea;
    }

    static double value(AuxiliaryDataType /*area*/, const State& state)
    {
        return state.hourlyResults->PompageHoraire[state.hourInTheWeek];
    }

    static void setHourlyValue(IntermediateValues& iv,
                               AuxiliaryDataType area,
                               const State& state,
                               unsigned int)
    {
        if (area != nullptr)
        {
            iv[state.hourInTheYear] = value(area, state);
        }
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardPumping = VCard_Base<PumpingTraits>;

template<class NextT = Container::EndOfList>
using Pumping = Economy_Base<PumpingTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
