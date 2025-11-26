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

        for (Type dir: {Type::DOWN, Type::UP})
        {
            builder.updateHourWithinWeek(pdt);
            int timeMax = data.areaReserves[pays].referenceGlobalActivationDuration[dir];
            double maxGlobalEnergyActivationRatio = (dir == Type::UP ? -1 : 1)
                                                    * data.areaReserves[pays]
                                                        .maxGlobalEnergyActivationRatio[dir];
            for (int t = 0; t < timeMax; t++)
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

                if (dir == Type::UP)
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
                if (dir == Type::UP)
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
