// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

// CostVariationWithdrawal[h] + Withdrawal[h+1] - Withdrawal[h]  >= 0
void ShortTermStorageCostVariationWithdrawalBackward::buildConstraint(int index)
{
    builder.shortTermCostVariationWithdrawal(index, 1.0)
      .shortTermStorageWithdrawal(index, -1.0)
      .shortTermStorageWithdrawal(index, 1.0, 1, builder.data.NombreDePasDeTempsPourUneOptimisation)
      .greaterThan()
      .build();
}

void ShortTermStorageCostVariationWithdrawalBackward::add(unsigned int timeStep, unsigned int area)
{
    addStorageConstraint("ShortTermStorageCostVariationWithdrawalBackward", timeStep, area);
}

int& ShortTermStorageCostVariationWithdrawalBackward::TargetConstraintIndex(int timeStep, int index)
{
    return data.CorrespondanceCntNativesCntOptim[timeStep]
      .ShortTermStorageCostVariationWithdrawalBackward[index];
}

bool ShortTermStorageCostVariationWithdrawalBackward::IsConstraintEnabled(
  const PROPERTIES& properties)
{
    return properties.penalizeVariationWithdrawal;
}
