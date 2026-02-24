// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

// CostVariationWithdrawal[h] - Withdrawal[h+1] + Withdrawal[h]  >= 0
void ShortTermStorageCostVariationWithdrawalForward::buildConstraint(int index)
{
    builder.shortTermCostVariationWithdrawal(index, 1.0)
      .shortTermStorageWithdrawal(index, 1.0)
      .shortTermStorageWithdrawal(index,
                                  -1.0,
                                  1,
                                  builder.data.NombreDePasDeTempsPourUneOptimisation)
      .greaterThan()
      .build();
}

void ShortTermStorageCostVariationWithdrawalForward::add(unsigned int timeStep, unsigned int area)
{
    addStorageConstraint("ShortTermStorageCostVariationWithdrawalForward", timeStep, area);
}

bool ShortTermStorageCostVariationWithdrawalForward::IsConstraintEnabled(
  const PROPERTIES& properties)
{
    return properties.penalizeVariationWithdrawal;
}

int& ShortTermStorageCostVariationWithdrawalForward::TargetConstraintIndex(int timeStep, int index)
{
    return data.CorrespondanceCntNativesCntOptim[timeStep]
      .ShortTermStorageCostVariationWithdrawalForward[index];
}
