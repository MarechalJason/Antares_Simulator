// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

void ShortTermStorageCostVariation::addStorageConstraint(const std::string& constraintName,
                                                         int timeStep,
                                                         int area)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    const int hourInTheYear = builder.data.weekInTheYear * 168 + timeStep;
    namer.updateTimeStep(hourInTheYear);
    namer.updateArea(builder.data.NomsDesPays[area]);

    builder.updateHourWithinWeek(timeStep);
    for (const auto& storage: data.ShortTermStorage[area])
    {
        if (IsConstraintEnabled(storage))
        {
            namer.shortTermStorageCostVariation(constraintName,
                                                builder.data.nombreDeContraintes,
                                                storage.name);
            const auto index = storage.clusterGlobalIndex;

            TargetConstraintIndex(timeStep, index) = builder.data.nombreDeContraintes;

            buildConstraint(index);
        }
    }
}
