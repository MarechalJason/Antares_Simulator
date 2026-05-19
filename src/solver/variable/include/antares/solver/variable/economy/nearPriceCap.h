// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

constexpr double eps = 1e-2;
constexpr double margin = 5.;

struct NearPriceCapTraits
{
    static std::string Caption()
    {
        return "NPCAP HOURS";
    }

    static std::string Unit()
    {
        return "Hours";
    }

    static std::string Description()
    {
        return "Near Price Cap Hours";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 4;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSumThen1IfPositive;

    using AuxiliaryDataType = double;

    static void initializeFromArea(AuxiliaryDataType& unsuppliedEnergyCost,
                                   Data::Study*,
                                   Data::Area* area)
    {
        unsuppliedEnergyCost = area->thermal.unsuppliedEnergyCost;
    }

    static bool checkCondition(AuxiliaryDataType unsuppliedEnergyCost, const State& state)
    {
        double mrgPrice = -state.hourlyResults->CoutsMarginauxHoraires[state.hourInTheWeek];
        return mrgPrice > unsuppliedEnergyCost - margin + eps;
    }

    static double value(AuxiliaryDataType, const State&)
    {
        return 1.;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardNearPriceCap = VCard_Base<NearPriceCapTraits>;

/*!
** \brief
*/
template<class NextT = Container::EndOfList>
using NearPriceCap = Economy_Base<NearPriceCapTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
