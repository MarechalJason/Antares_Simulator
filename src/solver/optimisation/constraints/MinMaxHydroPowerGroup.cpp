// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MinMaxHydroPowerGroup.h"

MinHydroPowerData MinMaxHydroPowerGroup::GetMinHydroPowerData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                       ->NombreDePasDeTempsPourUneOptimisation,
            .NumeroDeContrainteMinEnergieHydraulique = problemeHebdo_
                                                         ->NumeroDeContrainteMinEnergieHydraulique};
}

MaxHydroPowerData MinMaxHydroPowerGroup::GetMaxHydroPowerData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                       ->NombreDePasDeTempsPourUneOptimisation,
            .NumeroDeContrainteMaxEnergieHydraulique = problemeHebdo_
                                                         ->NumeroDeContrainteMaxEnergieHydraulique};
}

void MinMaxHydroPowerGroup::BuildConstraints()
{
    auto minHydroPowerData = GetMinHydroPowerData();
    minHydroPower MinHydroPower(builder_, minHydroPowerData);
    auto maxHydroPowerData = GetMaxHydroPowerData();
    maxHydroPower MaxHydroPower(builder_, maxHydroPowerData);

    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        minHydroPower.add(area);
        maxHydroPower.add(area);
    }
}
