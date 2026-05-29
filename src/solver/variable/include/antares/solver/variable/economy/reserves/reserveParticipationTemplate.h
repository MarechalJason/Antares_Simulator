/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#pragma once

#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

/**
 * @brief CRTP base for ReserveParticipation variable classes with dynamic per-area columns.
 *
 * Derived classes must implement:
 *  - size_t getSizeFromArea(Study* study, Area* area)
 *  - void populateHourlyValues(State& state, unsigned int numSpace)
 *  - bool hasIndexMapping(const Study& study, const Area* area) const
 *  - void buildReportForIndex(SurveyResults& results, uint i, int fileLevel,
 *                             int precision, unsigned int numSpace) const
 */
template<typename DerivedType, typename VCardType>
class ReserveParticipationTemplate: public IVariable<DerivedType, VCardType>
{
public:
    using VCardTypeClass = VCardType;
    using AncestorType = IVariable<DerivedType, VCardType>;

    using ResultsType = typename VCardType::ResultsType;

    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count
          = ((VCardType::categoryDataLevel & CDataLevel
              && VCardType::categoryFileLevel & CFile)
               ? static_cast<int>(VCardType::columnCount)
                   * static_cast<int>(ResultsType::count)
               : 0);
    };

    ReserveParticipationTemplate() = default;

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        pNbYearsParallel = study->maxNbYearsInParallel;
        pValuesForTheCurrentYear.resize(pNbYearsParallel);

        auto* derived = static_cast<DerivedType*>(this);
        pSize = study->parameters.include.reserves ? derived->getSizeFromArea(study, area) : 0;

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
        static_cast<DerivedType*>(this)->populateHourlyValues(state, numSpace);
    }

    [[nodiscard]] Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
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

        if (AncestorType::isPrinted[0])
        {
            assert(results.data.area != nullptr);
            const auto* derived = static_cast<const DerivedType*>(this);

            for (uint i = 0; i < pSize; ++i)
            {
                if (derived->hasIndexMapping(results.data.study, results.data.area))
                {
                    derived->buildReportForIndex(results, i, fileLevel, precision, numSpace);
                }
            }
        }
    }

protected:
    typename VCardType::IntermediateValuesType pValuesForTheCurrentYear;
    size_t pSize = 0;
    unsigned int pNbYearsParallel = 0;

}; // class ReserveParticipationTemplate

} // namespace Antares::Solver::Variable::Economy::Reserves
