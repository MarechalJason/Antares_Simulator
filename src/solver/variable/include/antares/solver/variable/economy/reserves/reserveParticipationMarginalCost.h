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

#include "antares/solver/variable/variable.h"

#include "vCardReserveParticipationMarginalCost.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/*!
** \brief Reserve participation marginal cost for an area
*/
class ReserveParticipationMarginalCost
    : public IVariable<ReserveParticipationMarginalCost, VCardReserveParticipationMarginalCost>
{
public:
    using VCardType = VCardReserveParticipationMarginalCost;
    using AncestorType = IVariable<ReserveParticipationMarginalCost, VCardType>;

    using ResultsType = typename VCardType::ResultsType;

    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = ((VCardType::categoryDataLevel & CDataLevel
                                       && VCardType::categoryFileLevel & CFile)
                                        ? static_cast<int>(VCardType::columnCount)
                                            * static_cast<int>(ResultsType::count)
                                        : 0);
    };

    ReserveParticipationMarginalCost() = default;

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        pSize = study->parameters.include.reserves
                  ? area->allCapacityReservations.value().areaCapacityReservations.size()
                  : 0;
        if (pSize)
        {
            AncestorType::pResults.resize(pSize);

            for (unsigned int numSpace = 0; numSpace < pNbYearsParallel; ++numSpace)
            {
                pValuesForTheCurrentYear[numSpace].resize(pSize);
                for (unsigned int i = 0; i != pSize; ++i)
                {
                    pValuesForTheCurrentYear[numSpace][i].initializeFromStudy(*study);
                }
            }

            for (unsigned int i = 0; i != pSize; ++i)
            {
                AncestorType::pResults[i].initializeFromStudy(*study);
                AncestorType::pResults[i].reset();
            }
        }
        else
        {
            AncestorType::pResults.clear();
        }
    }

    [[nodiscard]] size_t getMaxNumberColumns() const
    {
        return pSize * ResultsType::count;
    }

    void yearBegin(unsigned int /*year*/, unsigned int numSpace)
    {
        for (unsigned int i = 0; i != pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].reset();
        }
    }

    void yearEnd(unsigned int /*year*/, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < pSize; ++i)
        {
            pValuesForTheCurrentYear[numSpace][i].computeStatisticsForTheCurrentYear();
        }
    }

    void computeSummary(unsigned int year, unsigned int numSpace)
    {
        VariableAccessorType::ComputeSummary(pValuesForTheCurrentYear[numSpace],
                                             AncestorType::pResults,
                                             year);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        if (state.study.parameters.include.reserves)
        {
            auto& area = state.area;
            int column = 0;
            auto reserves = state.problemeHebdo->allReserves.value()[area->index];
            for (const auto& reserve: reserves.areaCapacityReservations)
            {
                pValuesForTheCurrentYear[numSpace][column++].hour[state.hourInTheYear]
                  += state.hourlyResults->Reserves.value()[state.hourInTheWeek]
                       .CoutsMarginauxHoraires[reserve.areaReserveIndex];
            }
        }
    }

    Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int column,
      unsigned int numSpace) const
    {
        return pValuesForTheCurrentYear[numSpace][column].hour;
    }

    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int numSpace) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;

        if (AncestorType::isPrinted[0] && results.data.area->allCapacityReservations)
        {
            assert(results.data.area != nullptr);
            results.variableUnit = VCardType::Unit();
            int column = 0;
            for (const auto& reserveID:
                 results.data.area->allCapacityReservations.value().areaCapacityReservations
                   | std::views::keys)
            {
                Yuni::String caption = results.data.study.runtime.reserveIDToName.value().at(
                  reserveID);
                caption << "_MRG.COST";
                results.variableCaption = caption;
                pValuesForTheCurrentYear[numSpace][column]
                  .template buildAnnualSurveyReport<VCardType>(results, fileLevel, precision);
                column++;
            }
        }
    }

private:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize = 0;
    unsigned int pNbYearsParallel = 0;

}; // class ReserveParticipationMarginalCost

} // namespace Antares::Solver::Variable::Economy::Reserves
