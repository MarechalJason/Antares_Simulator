#include "antares/solver/optimisation/constraints/HydroGlobalEnergyLevelReserveParticipation.h"

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

        for (bool isUpReserve: {reserveIsDown, reserveIsUp})
        {
            builder.updateHourWithinWeek(pdt);

            for (int t = 0;
                 t < (isUpReserve ? data.areaReserves[pays].referenceGlobalActivationDurationUp
                                  : data.areaReserves[pays].referenceGlobalActivationDurationDown);
                 t++)
            {
                for (auto& capacityReservation:
                     isUpReserve ? data.areaReserves[pays].areaCapacityReservationsUp
                                 : data.areaReserves[pays].areaCapacityReservationsDown)
                {
                    if (capacityReservation.AllHydroReservesParticipation.size())
                    {
                        RESERVE_PARTICIPATION_HYDRO& reserveParticipation
                          = capacityReservation.AllHydroReservesParticipation[cluster];
                        builder.HydroReserveParticipation(
                          isUpReserve,
                          reserveParticipation.globalIndexClusterParticipation,
                          capacityReservation.powerActivationRatio,
                          t,
                          builder.data.NombreDePasDeTempsPourUneOptimisation);
                    }
                }
                if (builder.NumberOfVariables() > 0)
                {
                    builder.HydroLevel(
                      globalClusterIdx,
                      isUpReserve ? -data.areaReserves[pays].maxGlobalEnergyActivationRatioUp
                                  : data.areaReserves[pays].maxGlobalEnergyActivationRatioDown,
                      t,
                      builder.data.NombreDePasDeTempsPourUneOptimisation);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.lessThan();

                if (isUpReserve)
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
                isUpReserve ? namer.HydroGlobalEnergyLevelReserveParticipationUp(
                                builder.data.nombreDeContraintes,
                                "Hydro")
                            : namer.HydroGlobalEnergyLevelReserveParticipationDown(
                                builder.data.nombreDeContraintes,
                                "Hydro");
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
