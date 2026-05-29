/*
** Copyright 2007-2025 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

#include "reserveParticipationTemplate.h"
#include "vCardReserveParticipationByDispatchableOnUnitsPlant.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve Participation from on/running units in thermal clusters
*/
class ReserveParticipationByDispatchableOnUnitsPlant
    : public ReserveParticipationTemplate<ReserveParticipationByDispatchableOnUnitsPlant,
                                          VCardReserveParticipationByDispatchableOnUnitsPlant>
{
public:
    using VCardType = VCardReserveParticipationByDispatchableOnUnitsPlant;
    using AncestorType = ReserveParticipationTemplate<
      ReserveParticipationByDispatchableOnUnitsPlant,
      VCardType>;

    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationByDispatchableOnUnitsPlant() = default;

    size_t getSizeFromArea(Data::Study* /*study*/, Data::Area* area)
    {
        return area->thermal.list.reserveParticipationsCount();
    }

    void populateHourlyValues(State& state, unsigned int numSpace);

    bool hasIndexMapping(const Data::Study& study, const Data::Area* area) const
    {
        return study.parameters.include.reserves
               && !study.runtime.reserveParticipationIndexMaps.value()
                     .at(area->id)
                     .thermalClusters.empty();
    }

    void buildReportForIndex(SurveyResults& results,
                             uint i,
                             int fileLevel,
                             int precision,
                             unsigned int numSpace) const
    {
        auto [reserveID, clusterName] = results.data.study.runtime.reserveParticipationIndexMaps
                                          .value()
                                          .at(results.data.area->id)
                                          .thermalClusters.right.at(i);
        auto reserveName = results.data.study.runtime.reserveIDToName.value().at(reserveID);
        results.variableCaption = reserveName + "_" + clusterName;
        results.variableUnit = VCardType::Unit();
        pValuesForTheCurrentYear[numSpace][i]
          .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
    }

}; // class ReserveParticipationByDispatchableOnUnitsPlant

inline void ReserveParticipationByDispatchableOnUnitsPlant::populateHourlyValues(
  State& state,
  unsigned int numSpace)
{
    if (hasIndexMapping(state.study, state.area))
    {
        const auto& resData = state.reserveData.value().at(state.area->index);
        for (const auto& clusterName:
             resData.reserveParticipationPerThermalClusterForYear.at(state.hourInTheYear)
               | std::views::keys)
        {
            for (const auto& [reserveName, reserveParticipation]:
                 resData.reserveParticipationPerThermalClusterForYear.at(state.hourInTheYear)
                   .at(clusterName))
            {
                pValuesForTheCurrentYear[numSpace]
                                        [state.study.runtime.reserveParticipationIndexMaps.value()
                                           .at(state.area->id)
                                           .thermalClusters.left.at(
                                             std::make_pair(reserveName, clusterName))]
                                          .hour[state.hourInTheYear]
                  = reserveParticipation.onUnitsParticipation;
            }
        }
    }
}

} // namespace Antares::Solver::Variable::Economy::Reserves
