// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MaxHydroPower.h"

void MaxHydroPower::add(int area)
{
    if (data.CaracteristiquesHydrauliques[area].PresenceDHydrauliqueModulable
        && (data.CaracteristiquesHydrauliques[area].TurbinageEntreBornes
            || data.CaracteristiquesHydrauliques[area].PresenceDePompageModulable))
    {
        data.NumeroDeContrainteMaxEnergieHydraulique[area] = builder.data.nombreDeContraintes;

        for (int timeStep = 0; timeStep < builder.data.NombreDePasDeTempsPourUneOptimisation; timeStep++)
        {
            builder.updateHourWithinWeek(timeStep);
            builder.hydroPower(area, 1.0);
        }
        data.NumeroDeContrainteMaxEnergieHydraulique[area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.updateArea(builder.data.NomsDesPays[area]);
        namer.updateTimeStep(builder.data.weekInTheYear);
        namer.maxHydroPower(builder.data.nombreDeContraintes);

        builder.lessThan().build();
    }
    else
    {
        data.NumeroDeContrainteMaxEnergieHydraulique[area] = -1;
    }
}
