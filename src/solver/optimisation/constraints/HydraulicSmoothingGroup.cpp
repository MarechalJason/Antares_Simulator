// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydraulicSmoothingGroup.h"

#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxDown.h"
#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxUp.h"
#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationSum.h"

void HydraulicSmoothingGroup::BuildConstraints()
{
    HydroPowerSmoothingUsingVariationSum hydroPowerSmoothingUsingVariationSum(builder_);
    HydroPowerSmoothingUsingVariationMaxDown hydroPowerSmoothingUsingVariationMaxDown(builder_);
    HydroPowerSmoothingUsingVariationMaxUp hydroPowerSmoothingUsingVariationMaxUp(builder_);

    if (problemeHebdo_->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
        {
            if (!problemeHebdo_->CaracteristiquesHydrauliques[area].PresenceDHydrauliqueModulable)
            {
                continue;
            }

            hydroPowerSmoothingUsingVariationSum
              .add(area, problemeHebdo_->NombreDePasDeTempsPourUneOptimisation);
        }
    }

    else if (problemeHebdo_->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
        {
            if (!problemeHebdo_->CaracteristiquesHydrauliques[area].PresenceDHydrauliqueModulable)
            {
                continue;
            }

            for (int timeStep = 0; timeStep < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; timeStep++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(area, timeStep);
                hydroPowerSmoothingUsingVariationMaxUp.add(area, timeStep);
            }
        }
    }
}
