// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FinalStockEquivalent.h"

void FinalStockEquivalent::add(int area)
{
    const auto timeStep = builder.data.NombreDePasDeTempsPourUneOptimisation - 1;
    if (data.CaracteristiquesHydrauliques[area].AccurateWaterValue
        && data.CaracteristiquesHydrauliques[area].DirectLevelAccess)
    { /*  equivalence constraint : StockFinal- Niveau[T]= 0*/

        data.NumeroDeContrainteEquivalenceStockFinal[area] = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes);

        namer.updateArea(builder.data.NomsDesPays[area]);
        namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
        namer.finalStockEquivalent(builder.data.nombreDeContraintes);

        builder.updateHourWithinWeek(timeStep)
          .finalStorage(area, 1.0)
          .updateHourWithinWeek(builder.data.NombreDePasDeTempsPourUneOptimisation - 1)
          .hydroLevel(area, -1.0)
          .equalTo()
          .build();
    }
}
