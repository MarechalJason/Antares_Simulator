// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationSum.h"

void HydroPowerSmoothingUsingVariationSum::add(int area,
                                               const int nombreDePasDeTempsPourUneOptimisation)
{
    for (int timeStep = 0; timeStep < nombreDePasDeTempsPourUneOptimisation; timeStep++)
    {
        int nextTimeStep = timeStep + 1;
        if (nextTimeStep >= nombreDePasDeTempsPourUneOptimisation)
        {
            nextTimeStep = 0;
        }
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.updateArea(builder.data.NomsDesPays[area]);
        namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
        namer.hydroPowerSmoothingUsingVariationSum(builder.data.nombreDeContraintes);

        builder.updateHourWithinWeek(timeStep)
          .hydroPower(area, 1.0)
          .updateHourWithinWeek(nextTimeStep)
          .hydroPower(area, -1.0)
          .updateHourWithinWeek(timeStep)
          .hydroPowerDown(area, -1.0)
          .hydroPowerUp(area, 1.0)
          .equalTo()
          .build();
    }
}
