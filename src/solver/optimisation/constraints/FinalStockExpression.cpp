// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FinalStockExpression.h"

void FinalStockExpression::add(int area)
{
    const auto timeStep = builder.data.NombreDePasDeTempsPourUneOptimisation - 1;

    if (data.CaracteristiquesHydrauliques[area].AccurateWaterValue)
    /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
    {
        builder.updateHourWithinWeek(timeStep).finalStorage(area, -1.0);
        for (int layerindex = 0; layerindex < 100; layerindex++)
        {
            builder.layerStorage(area, layerindex, 1.0);
        }
        data.NumeroDeContrainteExpressionStockFinal[area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);

        namer.updateArea(builder.data.NomsDesPays[area]);
        namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
        namer.finalStockExpression(builder.data.nombreDeContraintes);
        builder.equalTo().build();
    }
}
