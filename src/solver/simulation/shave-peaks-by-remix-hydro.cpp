#include "include/antares/solver/simulation/shave-peaks-by-remix-hydro.h"

#include <algorithm>
#include <numeric>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

constexpr double RESERVOIR_LEVEL_TOLERANCE = 1.e-6;

namespace Antares::Solver::Simulation
{

int find_min_index(const std::vector<double>& TotalGen,
                   const std::vector<double>& OutUnsupE,
                   const std::vector<OPTIMAL_HYDRO_USAGE>& OutHydroUsage,
                   const std::vector<bool>& triedBottom,
                   const std::vector<double>& HydroPmax,
                   const std::vector<bool>& enabledHours,
                   double top)
{
    double min_val = top;
    int min_hour = -1;
    for (unsigned int h = 0; h < TotalGen.size(); ++h)
    {
        if (OutUnsupE[h] > 0 && OutHydroUsage[h].TurbinageHoraire < HydroPmax[h] && !triedBottom[h]
            && enabledHours[h])
        {
            if (TotalGen[h] < min_val)
            {
                min_val = TotalGen[h];
                min_hour = h;
            }
        }
    }
    return min_hour;
}

int find_max_index(const std::vector<double>& TotalGen,
                   const std::vector<OPTIMAL_HYDRO_USAGE>& OutHydroUsage,
                   const std::vector<bool>& triedPeak,
                   const std::vector<double>& HydroPmin,
                   const std::vector<bool>& enabledHours,
                   double ref_value,
                   double eps)
{
    double max_val = 0;
    int max_hour = -1;
    for (unsigned int h = 0; h < TotalGen.size(); ++h)
    {
        if (OutHydroUsage[h].TurbinageHoraire > HydroPmin[h] && TotalGen[h] >= ref_value + eps
            && !triedPeak[h] && enabledHours[h])
        {
            if (TotalGen[h] > max_val)
            {
                max_val = TotalGen[h];
                max_hour = h;
            }
        }
    }
    return max_hour;
}

namespace
{
bool operator<=(const std::vector<double>& a, const std::vector<double>& b)
{
    return a.size() == b.size()
           && std::ranges::all_of(std::views::iota(size_t{0}, a.size()),
                                  [&](size_t i) { return a[i] <= b[i]; });
}

bool operator<=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e <= c; });
}

bool operator>=(const std::vector<double>& v, const double c)
{
    return std::ranges::all_of(v, [&c](double e) { return e >= c; });
}

void checkInputCorrectness(const std::vector<double>& DispatchGen,
                           const std::vector<OPTIMAL_HYDRO_USAGE>& hydroUsage,
                           const std::vector<double>& UnsupE,
                           const std::vector<double>& HydroPmax,
                           const std::vector<double>& HydroPmin,
                           double initialLevel,
                           double reservoirCapacity,
                           bool reservoirManagement,
                           const std::vector<double>& inflows,
                           const std::vector<double>& Spillage,
                           const std::vector<double>& DTG_MRG)
{
    std::string msg_prefix = "Remix hydro input : ";

    // Initial level smaller than capacity
    if (initialLevel >= reservoirCapacity + RESERVOIR_LEVEL_TOLERANCE)
    {
        throw std::invalid_argument(msg_prefix + "initial level > reservoir capacity");
    }
    // Arrays sizes must be identical
    std::vector<size_t> sizes = {DispatchGen.size(),
                                 hydroUsage.size(),
                                 UnsupE.size(),
                                 HydroPmax.size(),
                                 HydroPmin.size(),
                                 inflows.size(),
                                 Spillage.size(),
                                 DTG_MRG.size()};

    if (!std::ranges::all_of(sizes, [&sizes](const size_t s) { return s == sizes.front(); }))
    {
        throw std::invalid_argument(msg_prefix + "arrays of different sizes");
    }

    // Arrays are of size 0
    if (!DispatchGen.size())
    {
        throw std::invalid_argument(msg_prefix + "all arrays of sizes 0");
    }

    bool allValid = std::all_of(hydroUsage.begin(),
                                hydroUsage.end(),
                                [&HydroPmax, i = 0](const OPTIMAL_HYDRO_USAGE& usage) mutable
                                { return usage.TurbinageHoraire <= HydroPmax[i++]; });
    // Hydro production < Pmax
    if (!allValid)
    {
        throw std::invalid_argument(msg_prefix
                                    + "Hydro generation not smaller than Pmax everywhere");
    }

    allValid = std::all_of(hydroUsage.begin(),
                           hydroUsage.end(),
                           [&HydroPmin, i = 0](const OPTIMAL_HYDRO_USAGE& usage) mutable
                           { return usage.TurbinageHoraire >= HydroPmin[i++]; });
    // Hydro production > Pmin
    if (!allValid)
    {
        throw std::invalid_argument(msg_prefix
                                    + "Hydro generation not greater than Pmin everywhere");
    }

    if (reservoirManagement)
    {
        if (!std::ranges::all_of(hydroUsage,
                                 [&reservoirCapacity](const OPTIMAL_HYDRO_USAGE& usage)
                                 {
                                     return usage.niveauxHoraires
                                              <= reservoirCapacity + RESERVOIR_LEVEL_TOLERANCE
                                            && usage.niveauxHoraires
                                                 >= reservoirCapacity - RESERVOIR_LEVEL_TOLERANCE;
                                 }))
        {
            throw std::invalid_argument(
              msg_prefix + "levels computed from input don't respect reservoir bounds");
        }
    }
}
} // namespace

RemixHydroOutput shavePeaksByRemixingHydro(const std::vector<double>& DispatchGen,
                                           const std::vector<OPTIMAL_HYDRO_USAGE>& hydroUsage,
                                           const std::vector<double>& UnsupE,
                                           const std::vector<double>& HydroPmax,
                                           const std::vector<double>& HydroPmin,
                                           double initialLevel,
                                           double reservoirCapacity,
                                           double pumpingEfficiency,
                                           bool reservoirManagement,
                                           const std::vector<double>& inflows,
                                           const std::vector<double>& Spillage,
                                           const std::vector<double>& DTG_MRG)
{
    std::vector<double> levels(DispatchGen.size());
    if (!levels.empty() && reservoirManagement)
    {
        levels[0] = initialLevel + inflows[0] - hydroUsage[0].debordementsHoraires
                    + pumpingEfficiency * hydroUsage[0].PompageHoraire
                    - hydroUsage[0].TurbinageHoraire;
        for (size_t h = 1; h < levels.size(); ++h)
        {
            levels[h] = levels[h - 1] + inflows[h] - hydroUsage[h].debordementsHoraires
                        + pumpingEfficiency * hydroUsage[h].PompageHoraire
                        - hydroUsage[h].TurbinageHoraire;
        }
    }

    checkInputCorrectness(DispatchGen,
                          hydroUsage,
                          UnsupE,
                          HydroPmax,
                          HydroPmin,
                          initialLevel,
                          reservoirCapacity,
                          reservoirManagement,
                          inflows,
                          Spillage,
                          DTG_MRG);

    std::vector<OPTIMAL_HYDRO_USAGE> OutHydroUsage = hydroUsage;
    std::vector<double> OutUnsupE = UnsupE;

    int loop = 1000;
    double eps = 1e-3;
    double top = *std::max_element(DispatchGen.begin(), DispatchGen.end())
                 + std::max_element(hydroUsage.begin(),
                                    hydroUsage.end(),
                                    [](const OPTIMAL_HYDRO_USAGE& a, const OPTIMAL_HYDRO_USAGE& b)
                                    { return a.TurbinageHoraire < b.TurbinageHoraire; })
                     ->TurbinageHoraire
                 + *std::max_element(UnsupE.begin(), UnsupE.end()) + 1;

    std::vector<bool> enabledHours(DispatchGen.size(), false);
    for (unsigned int h = 0; h < enabledHours.size(); h++)
    {
        if (Spillage[h] + DTG_MRG[h] == 0. && hydroUsage[h].TurbinageHoraire + UnsupE[h] > 0.)
        {
            enabledHours[h] = true;
        }
    }

    std::vector<double> TotalGen(DispatchGen.size());
    std::transform(DispatchGen.begin(),
                   DispatchGen.end(),
                   hydroUsage.begin(),
                   TotalGen.begin(),
                   [](double dispatch, const OPTIMAL_HYDRO_USAGE& hydro)
                   { return dispatch + hydro.TurbinageHoraire; });

    while (loop-- > 0)
    {
        std::vector<bool> triedBottom(DispatchGen.size(), false);
        double delta = 0;

        while (true)
        {
            int hourBottom = find_min_index(TotalGen,
                                            OutUnsupE,
                                            OutHydroUsage,
                                            triedBottom,
                                            HydroPmax,
                                            enabledHours,
                                            top);
            if (hourBottom == -1)
            {
                break;
            }

            std::vector<bool> triedPeak(DispatchGen.size(), false);
            while (true)
            {
                int hourPeak = find_max_index(TotalGen,
                                              OutHydroUsage,
                                              triedPeak,
                                              HydroPmin,
                                              enabledHours,
                                              TotalGen[hourBottom],
                                              eps);
                if (hourPeak == -1)
                {
                    break;
                }
                double max_pic, max_creux;

                if (reservoirManagement)
                {
                    std::span<double> intermediate_level(levels.begin()
                                                           + std::min(hourBottom, hourPeak),
                                                         levels.begin()
                                                           + std::max(hourBottom, hourPeak));

                    if (hourBottom < hourPeak)
                    {
                        max_pic = reservoirCapacity;
                        max_creux = *std::ranges::min_element(intermediate_level);
                    }
                    else
                    {
                        max_pic = reservoirCapacity - *std::ranges::max_element(intermediate_level);
                        max_creux = reservoirCapacity;
                    }
                }
                else
                {
                    max_pic = std::numeric_limits<double>::max();
                    max_creux = std::numeric_limits<double>::max();
                }

                max_pic = std::min(OutHydroUsage[hourPeak].TurbinageHoraire - HydroPmin[hourPeak],
                                   max_pic);
                max_creux = std::min(
                  {HydroPmax[hourBottom] - OutHydroUsage[hourBottom].TurbinageHoraire,
                   OutUnsupE[hourBottom],
                   max_creux});

                double dif_pic_creux = std::max(TotalGen[hourPeak] - TotalGen[hourBottom], 0.);

                delta = std::max(std::min({max_pic, max_creux, dif_pic_creux / 2.}), 0.);

                if (delta <= eps)
                {
                    delta = 0;
                }

                if (delta > 0)
                {
                    OutHydroUsage[hourPeak].TurbinageHoraire -= delta;
                    OutHydroUsage[hourBottom].TurbinageHoraire += delta;
                    OutUnsupE[hourPeak] = OutHydroUsage[hourPeak].TurbinageHoraire
                                          + UnsupE[hourPeak]
                                          - OutHydroUsage[hourPeak].TurbinageHoraire;
                    OutUnsupE[hourBottom] = OutHydroUsage[hourBottom].TurbinageHoraire
                                            + UnsupE[hourBottom]
                                            - OutHydroUsage[hourBottom].TurbinageHoraire;
                    break;
                }
                else
                {
                    triedPeak[hourPeak] = true;
                }
            }

            if (delta > 0)
            {
                break;
            }
            triedBottom[hourBottom] = true;
        }

        if (delta == 0)
        {
            break;
        }

        std::transform(DispatchGen.begin(),
                       DispatchGen.end(),
                       OutHydroUsage.begin(),
                       TotalGen.begin(),
                       [](double dispatch, const OPTIMAL_HYDRO_USAGE& hydro)
                       { return dispatch + hydro.TurbinageHoraire; });

        if (reservoirManagement)
        {
            levels[0] = initialLevel + inflows[0] - hydroUsage[0].debordementsHoraires
                        + pumpingEfficiency * hydroUsage[0].PompageHoraire
                        - OutHydroUsage[0].TurbinageHoraire;
            for (size_t h = 1; h < levels.size(); ++h)
            {
                levels[h] = levels[h - 1] + inflows[h] - hydroUsage[h].debordementsHoraires
                            + pumpingEfficiency * hydroUsage[h].PompageHoraire
                            - OutHydroUsage[h].TurbinageHoraire;
            }
        }
    }
    return {OutHydroUsage, OutUnsupE};
}

} // End namespace Antares::Solver::Simulation
