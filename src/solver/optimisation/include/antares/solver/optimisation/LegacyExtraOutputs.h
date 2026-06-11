// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <vector>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"
#include "antares/solver/optimisation/LegacyVariableInfo.h"

namespace Antares::Optimization
{

// Adds the derived "extra outputs" of the legacy solver to the simulation
// table, computed from the weekly solution:
//  - thermal `prop_cost`: linear objective coefficient on the cluster's
//    generation variable times the generated power;
//  - area `imbalance_cost`: spillage_cost * spilled_energy
//    + unsupplied_energy_cost * unsupplied_energy, where both costs are the
//    linear objective coefficients on the corresponding area variables.
void AddLegacyExtraOutputs(
  Antares::IO::Outputs::SimulationTable& simulationTable,
  const std::vector<std::optional<LegacyVariableInfo>>& variablesInfo,
  const std::vector<double>& solutionValues,
  const std::vector<double>& linearCosts,
  const Antares::Optimisation::LinearProblemApi::FillContext& fillContext,
  unsigned currentBlock);

} // namespace Antares::Optimization
