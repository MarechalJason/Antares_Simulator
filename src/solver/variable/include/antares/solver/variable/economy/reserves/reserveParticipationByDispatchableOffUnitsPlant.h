/*
** Copyright 2007-2023 RTE
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

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve Participation from off units in thermal clusters
*/
template<class NextT = Container::EndOfList>
class ReserveParticipationByDispatchableOffUnitsPlant
    : public ReserveParticipationTemplate<ReserveParticipationByDispatchableOffUnitsPlant<NextT>,
                                          VCardReserveParticipationByDispatchableOffUnitsPlant,
                                          NextT>
{
public:
    //! Type of the next static variable
    typedef NextT NextType;
    //! VCard
    typedef VCardReserveParticipationByDispatchableOffUnitsPlant VCardType;
    //! Ancestor
    typedef ReserveParticipationTemplate<ReserveParticipationByDispatchableOffUnitsPlant<NextT>,
                                         VCardType,
                                         NextT>
      AncestorType;

    using AncestorType::pNbYearsParallel;
    using AncestorType::pSize;
    using AncestorType::pValuesForTheCurrentYear;

    ReserveParticipationByDispatchableOffUnitsPlant() = default;

    size_t getSizeFromArea(Study* /*study*/, Area* area)
    {
        return area->thermal.list.reserveParticipationsCount();
    }

    void populateHourlyValues(State& state, unsigned int numSpace)
    {
        if (state.study.parameters.reservesEnabled
            && !state.area->reserveParticipationIndexMaps.value().thermalClusters.empty())
        {
            for (auto& [clusterName, _]:
                 state.reserveParticipationPerThermalClusterForYear[state.hourInTheYear])
            {
                for (const auto& [reserveName, reserveParticipation]:
                     state.reserveParticipationPerThermalClusterForYear[state.hourInTheYear]
                                                                       [clusterName])
                {
                    pValuesForTheCurrentYear[numSpace]
                                            [state.area->reserveParticipationIndexMaps.value()
                                               .thermalClusters.left.at(
                                                 std::make_pair(reserveName, clusterName))]
                                              .hour[state.hourInTheYear]
                      = reserveParticipation.offUnitsParticipation;
                }
            }
        }
    }

    bool hasIndexMapping(Area* area, uint /*i*/) const
    {
        return area->reserveParticipationIndexMaps
               && !area->reserveParticipationIndexMaps.value().thermalClusters.empty();
    }

    void buildReportForIndex(SurveyResults& results,
                             uint i,
                             int fileLevel,
                             int precision,
                             unsigned int numSpace) const
    {
        auto [reserveName, clusterName] = results.data.area->reserveParticipationIndexMaps.value()
                                            .thermalClusters.right.at(i);
        results.variableCaption = reserveName + "_" + clusterName + "_off";
        results.variableUnit = VCardType::Unit();
        pValuesForTheCurrentYear[numSpace][i]
          .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
    }

}; // class ReserveParticipationByDispatchableOffUnitsPlant

} // namespace Antares::Solver::Variable::Economy::Reserves
