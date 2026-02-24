// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MinDownTimeGroup.h"

MinDownTimeData MinDownTimeGroup::GetMinDownTimeDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build minDownTime constraints with
 * respect to default order
 */
void MinDownTimeGroup::BuildConstraints()
{
    auto data = GetMinDownTimeDataFromProblemHebdo();
    MinDownTime minDownTime(builder_, data);
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[area];
        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int timeStep = 0; timeStep < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; timeStep++)
            {
                minDownTime.add(area, index, timeStep);
            }
        }
    }
}
