// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageLevel.h"

void ShortTermStorageLevel::add(int timeStep, int area)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    const int hourInTheYear = builder.data.weekInTheYear * 168 + timeStep;
    namer.updateTimeStep(hourInTheYear);
    namer.updateArea(builder.data.NomsDesPays[area]);

    builder.updateHourWithinWeek(timeStep);
    for (const auto& storage: data.ShortTermStorage[area])
    {
        // L[h] - L[h-1] - efficiency * injection[h] + withdrawal[h] = inflows[h]
        namer.shortTermStorageLevel(builder.data.nombreDeContraintes, storage.name);
        const auto index = storage.clusterGlobalIndex;
        data.CorrespondanceCntNativesCntOptim[timeStep].ShortTermStorageLevelConstraint[index]
          = builder.data.nombreDeContraintes;

        builder.shortTermStorageLevel(index, 1.0)
          .shortTermStorageLevel(index,
                                 -1.0,
                                 -1,
                                 builder.data.NombreDePasDeTempsPourUneOptimisation)
          .shortTermStorageInjection(index, -storage.injectionEfficiency)
          .shortTermStorageWithdrawal(index, storage.withdrawalEfficiency);
        if (storage.allowOverflow)
        {
            builder.shortTermStorageOverflow(index, 1.0);
        }
        builder.equalTo().build();
    }
}
