// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

// CostVariationInjection[h] - Injection[h+1] + Injection[h]  >= 0
void ShortTermStorageCostVariationInjectionForward::buildConstraint(int index)
{
    builder.shortTermCostVariationInjection(index, 1.0)
      .shortTermStorageInjection(index, 1.0)
      .shortTermStorageInjection(index, -1.0, 1, builder.data.NombreDePasDeTempsPourUneOptimisation)
      .greaterThan()
      .build();
}

void ShortTermStorageCostVariationInjectionForward::add(unsigned int timeStep, unsigned int area)
{
    addStorageConstraint("ShortTermStorageCostVariationInjectionForward", timeStep, area);
}

int& ShortTermStorageCostVariationInjectionForward::ShortTermStorageCostVariationInjectionForward::
  TargetConstraintIndex(int timeStep, int index)
{
    return data.CorrespondanceCntNativesCntOptim[timeStep]
      .ShortTermStorageCostVariationInjectionForward[index];
}

bool ShortTermStorageCostVariationInjectionForward::IsConstraintEnabled(
  const PROPERTIES& properties)
{
    return properties.penalizeVariationInjection;
}
