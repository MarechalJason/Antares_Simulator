// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/NbUnitsOutageLessThanNbUnitsStop.h"

void NbUnitsOutageLessThanNbUnitsStop::add(int area, int index, int timeStep)
{
    if (!data.Simulation)
    {
        auto cluster = data.PaliersThermiquesDuPays[area]
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        data.CorrespondanceCntNativesCntOptim[timeStep]
          .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
          = -1;

        builder.updateHourWithinWeek(timeStep)
          .numberBreakingDownDispatchableUnits(cluster, 1.0)
          .numberStoppingDispatchableUnits(cluster, -1.0)
          .lessThan();

        if (builder.numberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[area]);
            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
            namer.NbUnitsOutageLessThanNbUnitsStop(
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
