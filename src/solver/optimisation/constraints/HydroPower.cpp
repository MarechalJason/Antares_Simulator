// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroPower.h"

void HydroPower::add(int area)
{
    const int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                        .NombreDePasDeTempsPourUneOptimisation;
    const auto& caracteristiquesHydrauliques = data.CaracteristiquesHydrauliques[area];
    if (caracteristiquesHydrauliques.PresenceDHydrauliqueModulable
        && !caracteristiquesHydrauliques.TurbinageEntreBornes)
    {
        if (caracteristiquesHydrauliques.PresenceDePompageModulable)
        {
            data.NumeroDeContrainteEnergieHydraulique[area] = builder.data.nombreDeContraintes;

            const double pumpingRatio = caracteristiquesHydrauliques.PumpingRatio;
            for (int timeStep = 0; timeStep < NombreDePasDeTempsPourUneOptimisation; timeStep++)
            {
                builder.updateHourWithinWeek(timeStep)
                  .hydroPower(area, 1.0)
                  .pumping(area, -pumpingRatio)
                  .overflow(area, 1.0);
            }
        }
        else
        {
            for (int timeStep = 0; timeStep < NombreDePasDeTempsPourUneOptimisation; timeStep++)
            {
                builder.updateHourWithinWeek(timeStep).hydroPower(area, 1.0).overflow(area, 1.0);
            }
        }
        data.NumeroDeContrainteEnergieHydraulique[area] = builder.data.nombreDeContraintes;

        builder.equalTo();
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.updateArea(builder.data.NomsDesPays[area]);
        namer.updateTimeStep(builder.data.weekInTheYear);
        namer.hydroPower(builder.data.nombreDeContraintes);
        builder.build();
    }
    else
    {
        data.NumeroDeContrainteEnergieHydraulique[area] = -1;
    }
}
