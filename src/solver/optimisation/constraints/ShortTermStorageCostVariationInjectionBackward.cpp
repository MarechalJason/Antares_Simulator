// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

// CostVariationInjection[h] + Injection[h+1] - Injection[h]  >= 0
void ShortTermStorageCostVariationInjectionBackward::buildConstraint(int index)
{
    builder.shortTermCostVariationInjection(index, 1.0)
      .shortTermStorageInjection(index, -1.0)
      .shortTermStorageInjection(index, 1.0, 1, builder.data.NombreDePasDeTempsPourUneOptimisation)
      .greaterThan()
      .build();
}

void ShortTermStorageCostVariationInjectionBackward::add(unsigned int timeStep, unsigned int area)
{
    addStorageConstraint("ShortTermStorageCostVariationInjectionBackward", timeStep, area);
}

int& ShortTermStorageCostVariationInjectionBackward::
  ShortTermStorageCostVariationInjectionBackward::TargetConstraintIndex(int timeStep, int index)
{
    return data.CorrespondanceCntNativesCntOptim[timeStep]
      .ShortTermStorageCostVariationInjectionBackward[index];
}

bool ShortTermStorageCostVariationInjectionBackward::IsConstraintEnabled(
  const PROPERTIES& properties)
{
    return properties.penalizeVariationInjection;
}
