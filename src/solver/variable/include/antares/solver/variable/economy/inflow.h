// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct InflowsTraits
{
    static std::string Caption()
    {
        return "H. INFL";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Hydraulic inflows";
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

    static bool checkCondition(AuxiliaryDataType area, const State&)
    {
        return area != nullptr;
    }

    static double value(AuxiliaryDataType area, const State& state)
    {
        return state.problemeHebdo->CaracteristiquesHydrauliques[area->index]
          .ApportNaturelHoraire[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardInflows = VCard_Base<InflowsTraits>;

template<class NextT = Container::EndOfList>
using Inflows = Economy_Base<InflowsTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
