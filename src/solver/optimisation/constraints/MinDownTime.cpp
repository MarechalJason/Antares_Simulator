// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MinDownTime.h"

void MinDownTime::add(int area, int index, int timeStep)
{
    const int DureeMinimaleDArretDUnGroupeDuPalierThermique
      = data.PaliersThermiquesDuPays[area].DureeMinimaleDArretDUnGroupeDuPalierThermique[index];
    auto cluster = data.PaliersThermiquesDuPays[area]
                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];

    data.CorrespondanceCntNativesCntOptim[timeStep]
      .NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster]
      = -1;
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                      .NombreDePasDeTempsPourUneOptimisation;

        builder.updateHourWithinWeek(timeStep).numberOfDispatchableUnits(cluster, 1.0);

        for (int k = timeStep - DureeMinimaleDArretDUnGroupeDuPalierThermique + 1; k <= timeStep; k++)
        {
            int t1 = k;
            if (t1 < 0)
            {
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;
            }

            builder.updateHourWithinWeek(t1).numberStoppingDispatchableUnits(cluster, 1.0);
        }
        builder.lessThan();
        if (builder.numberOfVariables() > 1)
        {
            data.CorrespondanceCntNativesCntOptim[timeStep]
              .NumeroDeContrainteDesContraintesDeDureeMinDArret[cluster]
              = builder.data.nombreDeContraintes;
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[area]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
            namer.MinDownTime(builder.data.nombreDeContraintes,
                              data.PaliersThermiquesDuPays[area].NomsDesPaliersThermiques[index]);

            builder.build();
        }
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
