// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/PMaxDispatchableGeneration.h"

void PMaxDispatchableGeneration::add(int area, int index, int timeStep)
{
    if (!data.Simulation)
    {
        double pmaxDUnGroupeDuPalierThermique = data.PaliersThermiquesDuPays[area]
                                                  .PmaxDUnGroupeDuPalierThermique[index];
        int cluster = data.PaliersThermiquesDuPays[area]
                        .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        builder.updateHourWithinWeek(timeStep)
          .dispatchableProduction(cluster, 1.0)
          .numberOfDispatchableUnits(cluster, -pmaxDUnGroupeDuPalierThermique)
          .lessThan();
        if (builder.numberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);

            namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
            namer.updateArea(builder.data.NomsDesPays[area]);

            namer.pMaxDispatchableGeneration(
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
