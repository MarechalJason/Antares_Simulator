
#pragma once

#include <tuple>
#include <vector>

#include "antares/solver/simulation/OptimalHydroUsage.h"

namespace Antares::Solver::Simulation
{

struct RemixHydroOutput
{
    std::vector<OPTIMAL_HYDRO_USAGE> hydroUsage;
    std::vector<double> UnsupE;

    // Allow std::tie(a, b, c) = remixHydroOutput;
    inline operator std::tuple<std::vector<OPTIMAL_HYDRO_USAGE>&, std::vector<double>&>()

    {
        return {hydroUsage, UnsupE};
    }
};

RemixHydroOutput shavePeaksByRemixingHydro(const std::vector<double>& DispatchGen,
                                           const std::vector<OPTIMAL_HYDRO_USAGE>& hydroUsage,
                                           const std::vector<double>& UnsupE,
                                           const std::vector<double>& HydroPmax,
                                           const std::vector<double>& HydroPmin,
                                           double initialLevel,
                                           double capacity,
                                           double efficiency,
                                           bool reservoirManagement,
                                           const std::vector<double>& inflow,
                                           const std::vector<double>& Spillage,
                                           const std::vector<double>& DTG_MRG);

} // namespace Antares::Solver::Simulation
