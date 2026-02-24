// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MinHydroPower.h"

void MinHydroPower::add(int area)
{
    if (data.CaracteristiquesHydrauliques[area].PresenceDHydrauliqueModulable
        && (data.CaracteristiquesHydrauliques[area].TurbinageEntreBornes
            || data.CaracteristiquesHydrauliques[area].PresenceDePompageModulable))
    {
        data.NumeroDeContrainteMinEnergieHydraulique[area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.updateArea(builder.data.NomsDesPays[area]);
        namer.updateTimeStep(builder.data.weekInTheYear);
        namer.minHydroPower(builder.data.nombreDeContraintes);

        for (int timeStep = 0; timeStep < data.NombreDePasDeTempsPourUneOptimisation; timeStep++)
        {
            builder.updateHourWithinWeek(timeStep);
            builder.hydroPower(area, 1.0);
        }

        data.NumeroDeContrainteMinEnergieHydraulique[area] = builder.data.nombreDeContraintes;
        builder.greaterThan().build();
    }
    else
    {
        data.NumeroDeContrainteMinEnergieHydraulique[area] = -1;
    }
}
