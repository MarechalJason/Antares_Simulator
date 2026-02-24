// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/NbUnitsOutageLessThanNbUnitsStopGroup.h"

NbUnitsOutageLessThanNbUnitsStopData
NbUnitsOutageLessThanNbUnitsStopGroup::GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo()
{
    return {.PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .Simulation = simulation_,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build NbUnitsOutageLessThanNbUnitsStopGroup constraints with
 * respect to default order
 */
void NbUnitsOutageLessThanNbUnitsStopGroup::BuildConstraints()
{
    auto data = GetNbUnitsOutageLessThanNbUnitsStopDataFromProblemHebdo();
    nbUnitsOutageLessThanNbUnitsStop NbUnitsOutageLessThanNbUnitsStop(builder_, data);
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                              ->PaliersThermiquesDuPays[area];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            for (int timeStep = 0; timeStep < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; timeStep++)
            {
                nbUnitsOutageLessThanNbUnitsStop.add(area, index, timeStep);
            }
        }
    }
}
