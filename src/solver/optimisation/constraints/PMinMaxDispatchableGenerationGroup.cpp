// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/PMinMaxDispatchableGenerationGroup.h"

/**
 * @brief build P{min,max}DispatchableGeneration constraints with
 * respect to default order
 */
void PMinMaxDispatchableGenerationGroup::BuildConstraints()
{
    auto data = GetStartUpCostsDataFromProblemHebdo();
    PMaxDispatchableGeneration pMaxDispatchableGeneration(builder_, data);
    PMinDispatchableGeneration pMinDispatchableGeneration(builder_, data);
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[area];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int timeStep = 0; timeStep < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; timeStep++)
            {
                pMaxDispatchableGeneration.add(area, index, timeStep);
                pMinDispatchableGeneration.add(area, index, timeStep);
            }
        }
    }
}
