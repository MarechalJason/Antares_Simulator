#include "antares/solver/optimisation/constraints/HydroGlobalEnergyLevelReserveParticipation.h"
using namespace reserve;

void HydroGlobalEnergyLevelReserveParticipation::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.hydroOfArea[pays].GlobalHydroIndex;

    if (!data.Simulation)
    {
        // 15 (t)
        // Stock participation is energy constrained (optional constraints)
        //  Sum(P_{res,t_st} * R_{min,res} +/- J_down/up * R_{lambda,t_st}) <= n_min * R_up
        // R_t : stock level at time t
        // P_{res,t_st} : power participation for reserve down res at time t_st
        // R_{min,res} : max power participation ratio
        // R_up : max stock level

        for (Direction dir: {Direction::DOWN, Direction::UP})
        {
            builder.updateHourWithinWeek(pdt);
            int time = dir == Direction::UP
                         ? data.areaReserves[pays].referenceGlobalActivationDurationUp
                         : data.areaReserves[pays].referenceGlobalActivationDurationDown;
            double maxGlobalEnergyActivationRatio = dir == Direction::UP
                                                      ? -data.areaReserves[pays]
                                                           .maxGlobalEnergyActivationRatioUp
                                                      : data.areaReserves[pays]
                                                          .maxGlobalEnergyActivationRatioDown;
            for (int t = 0; t < time; t++)
            {
                for (auto& capacityReservation:
                     data.areaReserves[pays].areaCapacityReservations | filter(dir))
                {
                    if (capacityReservation.AllHydroReservesParticipation.size())
                    {
                        RESERVE_PARTICIPATION_HYDRO& reserveParticipation
                          = capacityReservation.AllHydroReservesParticipation[cluster];
                        builder.HydroReserveParticipation(
                          dir,
                          reserveParticipation.globalIndexClusterParticipation,
                          capacityReservation.powerActivationRatio,
                          t,
                          builder.data.NombreDePasDeTempsPourUneOptimisation);
                    }
                }
                if (builder.NumberOfVariables() > 0)
                {
                    builder.HydroLevel(globalClusterIdx,
                                       maxGlobalEnergyActivationRatio,
                                       t,
                                       builder.data.NombreDePasDeTempsPourUneOptimisation);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.lessThan();

                if (dir == Direction::UP)
                {
                    data.CorrespondanceCntNativesCntOptim[pdt]
                      .reservesIndices.value()
                      .HydroGlobalEnergyLevelParticipationUp[globalClusterIdx]
                      = builder.data.nombreDeContraintes;
                }
                else
                {
                    data.CorrespondanceCntNativesCntOptim[pdt]
                      .reservesIndices.value()
                      .HydroGlobalEnergyLevelParticipationDown[globalClusterIdx]
                      = builder.data.nombreDeContraintes;
                }

                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                if (dir == Direction::UP)
                {
                    namer.HydroGlobalEnergyLevelReserveParticipationUp(
                      builder.data.nombreDeContraintes,
                      "Hydro");
                }
                else
                {
                    namer.HydroGlobalEnergyLevelReserveParticipationDown(
                      builder.data.nombreDeContraintes,
                      "Hydro");
                }
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForHydroReserves(
          pays,
          true /*account For Global Activation Duration*/);
        ;
    }
}
