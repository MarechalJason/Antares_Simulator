// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct AvailableDispatchGenTraits
{
    static std::string Caption()
    {
        return "AVL DTG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Available dispatchable generation";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = Data::Area*;

    static void initializeFromArea(AuxiliaryDataType& area, Data::Study*, Data::Area* inputArea)
    {
        area = inputArea;
    }

    static void yearBegin(IntermediateValues& yearlyValues,
                          AuxiliaryDataType area,
                          unsigned int year,
                          unsigned int)
    {
        if (!area)
        {
            return;
        }

        // Somme de toutes les productions disponibles pour l'ensemble des paliers thermiques.
        for (const auto& cluster: area->thermal.list.each_enabled())
        {
            const auto& availableProduction = cluster->series.getColumn(year);
            for (unsigned int hour = 0; hour != cluster->series.timeSeries.height; ++hour)
            {
                yearlyValues.hour[hour] += availableProduction[hour];
            }
        }
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardAvailableDispatchGen = EconomyVariableCard<AvailableDispatchGenTraits>;

/*!
** \brief Marginal AvailableDispatchGen
*/
using AvailableDispatchGen = EconomyVariableBase<AvailableDispatchGenTraits>;

} // namespace Antares::Solver::Variable::Economy
