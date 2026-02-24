// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/AreaHydroLevelGroup.h"

AreaHydroLevelData AreaHydroLevelGroup::GetAreaHydroLevelData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques};
}

void AreaHydroLevelGroup::BuildConstraints()
{
    auto areaHydroLevelData = GetAreaHydroLevelData();
    AreaHydroLevel areaHydroLevel(builder_, areaHydroLevelData);

    for (int timeStep = 0; timeStep < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; timeStep++)
    {
        for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
        {
            areaHydroLevel.add(area, timeStep);
        }
    }
}
