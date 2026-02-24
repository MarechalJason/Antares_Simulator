// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FinalStockGroup.h"

FinalStockEquivalentData FinalStockGroup::GetFinalStockEquivalentData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NumeroDeContrainteEquivalenceStockFinal = problemeHebdo_
                                                         ->NumeroDeContrainteEquivalenceStockFinal};
}

FinalStockExpressionData FinalStockGroup::GetFinalStockExpressionData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NumeroDeContrainteExpressionStockFinal = problemeHebdo_
                                                        ->NumeroDeContrainteExpressionStockFinal};
}

void FinalStockGroup::BuildConstraints()
{
    auto finalStockEquivalentData = GetFinalStockEquivalentData();
    FinalStockEquivalent finalStockEquivalent(builder_, finalStockEquivalentData);
    auto finalStockExpressionData = GetFinalStockExpressionData();
    FinalStockExpression finalStockExpression(builder_, finalStockExpressionData);

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        finalStockEquivalent.add(area);

        finalStockExpression.add(area);
    }
}
