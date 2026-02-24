// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ConsistenceNumberOfDispatchableUnits.h"

void ConsistenceNumberOfDispatchableUnits::add(int area, int index, int timeStep)
{
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                      .NombreDePasDeTempsPourUneOptimisation;

        auto cluster = data.PaliersThermiquesDuPays[area]
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        int Pdtmoins1 = timeStep - 1;
        if (Pdtmoins1 < 0)
        {
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;
        }

        builder.updateHourWithinWeek(timeStep)
          .numberOfDispatchableUnits(cluster, 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .numberOfDispatchableUnits(cluster, -1)
          .updateHourWithinWeek(timeStep)
          .numberStartingDispatchableUnits(cluster, -1)
          .numberStoppingDispatchableUnits(cluster, 1)
          .equalTo();

        if (builder.numberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.updateArea(builder.data.NomsDesPays[area]);

            namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
            namer.consistenceNODU(
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
