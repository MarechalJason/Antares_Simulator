// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ConsistenceNumberOfDispatchableUnitsGroup.h"

#include "antares/solver/optimisation/constraints/ConsistenceNumberOfDispatchableUnits.h"

/**
 * @brief build ConsistenceNumberOfDispatchableUnits constraints with
 * respect to default order
 */
void ConsistenceNumberOfDispatchableUnitsGroup::BuildConstraints()
{
    auto data = GetStartUpCostsDataFromProblemHebdo();
    ConsistenceNumberOfDispatchableUnits consistenceNumberOfDispatchableUnits(builder_, data);
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[area];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int timeStep = 0; timeStep < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; timeStep++)
            {
                consistenceNumberOfDispatchableUnits.add(area, index, timeStep);
            }
        }
    }
}
