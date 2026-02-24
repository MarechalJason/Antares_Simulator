// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MaxPumping.h"

void MaxPumping::add(int area)
{
    if (data.CaracteristiquesHydrauliques[area].PresenceDePompageModulable)
    {
        data.NumeroDeContrainteMaxPompage[area] = builder.data.nombreDeContraintes;

        for (int timeStep = 0; timeStep < builder.data.NombreDePasDeTempsPourUneOptimisation; timeStep++)
        {
            builder.updateHourWithinWeek(timeStep);
            builder.pumping(area, 1.0);
        }
        data.NumeroDeContrainteMaxPompage[area] = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.updateArea(builder.data.NomsDesPays[area]);
        namer.updateTimeStep(builder.data.weekInTheYear);
        namer.maxPumping(builder.data.nombreDeContraintes);
        builder.lessThan().build();
    }
    else
    {
        data.NumeroDeContrainteMaxPompage[area] = -1;
    }
}
