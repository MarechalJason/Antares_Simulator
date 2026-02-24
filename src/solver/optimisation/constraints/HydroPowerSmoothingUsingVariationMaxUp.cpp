// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroPowerSmoothingUsingVariationMaxUp.h"

void HydroPowerSmoothingUsingVariationMaxUp::add(int area, int timeStep)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.updateArea(builder.data.NomsDesPays[area]);
    namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
    namer.hydroPowerSmoothingUsingVariationMaxUp(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(timeStep)
      .hydroPower(area, 1.0)
      .updateHourWithinWeek(0)
      .hydroPowerUp(area, -1.0)
      .greaterThan()
      .build();
}
