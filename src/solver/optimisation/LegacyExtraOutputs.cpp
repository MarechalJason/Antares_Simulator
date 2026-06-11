// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputs.h"

#include "antares/solver/optimisation/LegacySolutionView.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;

namespace Antares::Optimization
{

namespace
{
std::optional<unsigned> BlockTimeIndex(const FillContext& fillContext, unsigned timeIndex)
{
    if (timeIndex >= fillContext.getGlobalFirstTimeStep()
        && timeIndex <= fillContext.getGlobalLastTimeStep())
    {
        return timeIndex - fillContext.getGlobalFirstTimeStep() + 1;
    }
    return std::nullopt;
}

void AddExtraOutputEntry(SimulationTable& simulationTable,
                         const std::string& output,
                         const LegacyVariableInfo& info,
                         double value,
                         const FillContext& fillContext,
                         unsigned currentBlock)
{
    simulationTable.addEntry({.block = currentBlock + 1,
                              .component = info.component,
                              .output = output,
                              .absolute_time_index = info.timeIndex + 1,
                              .block_time_index = BlockTimeIndex(fillContext, info.timeIndex),
                              .scenario_index = fillContext.getYear(),
                              .value = value,
                              .status = std::nullopt});
}

// prop_cost = generation_cost * generation_power: both factors belong to the
// generation variable itself, so they are read by index, which also keeps
// identically-named clusters of different areas apart.
void AddThermalPropCost(SimulationTable& simulationTable,
                        const LegacyVariableInfo& info,
                        std::size_t index,
                        const std::vector<double>& solutionValues,
                        const std::vector<double>& linearCosts,
                        const FillContext& fillContext,
                        unsigned currentBlock)
{
    AddExtraOutputEntry(simulationTable,
                        "prop_cost",
                        info,
                        linearCosts[index] * solutionValues[index],
                        fillContext,
                        currentBlock);
}

// imbalance_cost = spillage_cost * spilled_energy
//                  + unsupplied_energy_cost * unsupplied_energy
// Driven by the area's UnsuppliedEnergy variable; the Spillage variable of the
// same area and timestep is found through the solution view.
void AddAreaImbalanceCost(SimulationTable& simulationTable,
                          const LegacyVariableInfo& info,
                          std::size_t index,
                          const std::vector<double>& solutionValues,
                          const std::vector<double>& linearCosts,
                          const LegacySolutionView& solution,
                          const FillContext& fillContext,
                          unsigned currentBlock)
{
    const auto spilled = solution.value("Spillage", info.component, info.timeIndex);
    const auto spillageCost = solution.linearCost("Spillage", info.component, info.timeIndex);
    if (!spilled || !spillageCost)
    {
        return;
    }

    const double value = *spillageCost * *spilled + linearCosts[index] * solutionValues[index];
    AddExtraOutputEntry(simulationTable, "imbalance_cost", info, value, fillContext, currentBlock);
}
} // namespace

void AddLegacyExtraOutputs(SimulationTable& simulationTable,
                           const std::vector<std::optional<LegacyVariableInfo>>& variablesInfo,
                           const std::vector<double>& solutionValues,
                           const std::vector<double>& linearCosts,
                           const FillContext& fillContext,
                           unsigned currentBlock)
{
    const LegacySolutionView solution(variablesInfo, solutionValues, linearCosts);

    for (std::size_t index = 0; index < variablesInfo.size(); ++index)
    {
        const auto& info = variablesInfo[index];
        if (!info)
        {
            continue;
        }

        if (info->name == "DispatchableProduction")
        {
            AddThermalPropCost(simulationTable,
                               *info,
                               index,
                               solutionValues,
                               linearCosts,
                               fillContext,
                               currentBlock);
        }
        else if (info->name == "UnsuppliedEnergy")
        {
            AddAreaImbalanceCost(simulationTable,
                                 *info,
                                 index,
                                 solutionValues,
                                 linearCosts,
                                 solution,
                                 fillContext,
                                 currentBlock);
        }
    }
}

} // namespace Antares::Optimization
