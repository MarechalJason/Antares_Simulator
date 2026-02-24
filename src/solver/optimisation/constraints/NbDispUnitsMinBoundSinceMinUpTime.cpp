// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/NbDispUnitsMinBoundSinceMinUpTime.h"

void NbDispUnitsMinBoundSinceMinUpTime::add(int area, int index, int timeStep)
{
    auto cluster = data.PaliersThermiquesDuPays[area]
                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = data.PaliersThermiquesDuPays[area].DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index];

    data.CorrespondanceCntNativesCntOptim[timeStep]
      .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
      = -1;
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                      .NombreDePasDeTempsPourUneOptimisation;

        builder.updateHourWithinWeek(timeStep).numberOfDispatchableUnits(cluster, 1.0);

        for (int k = timeStep - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= timeStep; k++)
        {
            int t1 = k;
            if (t1 < 0)
            {
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;
            }

            builder.updateHourWithinWeek(t1)
              .numberStartingDispatchableUnits(cluster, -1.0)
              .numberBreakingDownDispatchableUnits(cluster, 1.0);
        }

        builder.greaterThan();
        if (builder.numberOfVariables() > 1)
        {
            data.CorrespondanceCntNativesCntOptim[timeStep]
              .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
              = builder.data.nombreDeContraintes;

            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.updateArea(builder.data.NomsDesPays[area]);

            namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
            namer.nbDispUnitsMinBoundSinceMinUpTime(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[area].NomsDesPaliersThermiques[index]);
            builder.build();
        }
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
