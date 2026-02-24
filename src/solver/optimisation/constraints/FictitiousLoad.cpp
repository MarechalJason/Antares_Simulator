// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FictitiousLoad.h"

void FictitiousLoad::add(int timeStep, int area)
{
    data.CorrespondanceCntNativesCntOptim[timeStep].NumeroDeContraintePourEviterLesChargesFictives[area]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);

    namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
    namer.updateArea(builder.data.NomsDesPays[area]);
    namer.fictiveLoads(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(timeStep);
    ExportPaliers(data.PaliersThermiquesDuPays[area], builder);
    auto coeff = data.DefaillanceNegativeUtiliserHydro[area] ? -1 : 0;
    builder.hydroPower(area, coeff).spillage(area, 1.0);

    builder.lessThan();
    builder.build();
}
