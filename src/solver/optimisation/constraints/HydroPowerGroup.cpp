// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroPowerGroup.h"

HydroPowerData HydroPowerGroup::GetHydroPowerDataFromProblemHebdo()
{
    return {
      .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
      .NombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                 ->NombreDePasDeTempsPourUneOptimisation,
      .NumeroDeContrainteEnergieHydraulique = problemeHebdo_->NumeroDeContrainteEnergieHydraulique};
}

void HydroPowerGroup::BuildConstraints()
{
    auto hydroPowerData = GetHydroPowerDataFromProblemHebdo();
    HydroPower hydroPower(builder_, hydroPowerData);

    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        hydroPower.add(area);
    }
}
