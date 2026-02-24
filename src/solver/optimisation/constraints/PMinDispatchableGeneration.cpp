// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/PMinDispatchableGeneration.h"

void PMinDispatchableGeneration::add(int area, int index, int timeStep)
{
    if (!data.Simulation)
    {
        double pminDUnGroupeDuPalierThermique = data.PaliersThermiquesDuPays[area]
                                                  .pminDUnGroupeDuPalierThermique[index];

        int cluster = data.PaliersThermiquesDuPays[area]
                        .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        builder.updateHourWithinWeek(timeStep)
          .dispatchableProduction(cluster, 1.0)
          .numberOfDispatchableUnits(cluster, -pminDUnGroupeDuPalierThermique)
          .greaterThan();
        /*consider Adding naming constraint inside the builder*/
        if (builder.numberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.updateArea(builder.data.NomsDesPays[area]);

            namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
            namer.pMinDispatchableGeneration(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[area].NomsDesPaliersThermiques[index]);
        }
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
