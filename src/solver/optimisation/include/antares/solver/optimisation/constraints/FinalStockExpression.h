// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "ConstraintBuilder.h"

struct FinalStockExpressionData
{
    const std::vector<ENERGIES_ET_PUISSANCES_HYDRAULIQUES> CaracteristiquesHydrauliques;
    std::vector<int>& NumeroDeContrainteExpressionStockFinal;
};

/*!
 * represent 'Final Stock Expression' constraint type
 */
class FinalStockExpression final: private ConstraintFactory
{
public:
    FinalStockExpression(ConstraintBuilder& builder, FinalStockExpressionData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param area : area
     */
    void add(int area);

private:
    FinalStockExpressionData& data;
};
