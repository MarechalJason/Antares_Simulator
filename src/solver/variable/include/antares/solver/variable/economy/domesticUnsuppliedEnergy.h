// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct DomesticUnsuppliedEnergyTraits
{
    static std::string Caption()
    {
        return "DENS";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Domestic Unsupplied Energy (demand that cannot be satisfied without "
               "interconnections/links)";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDENS[state.hourInTheWeek];
    }

    static bool checkCondition(const State&)
    {
        return true;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardDomesticUnsuppliedEnergy = VCard_Base<DomesticUnsuppliedEnergyTraits>;

template<class NextT = Container::EndOfList>
using DomesticUnsuppliedEnergy = Economy_Base<DomesticUnsuppliedEnergyTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
