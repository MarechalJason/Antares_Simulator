#include "antares/solver/optimisation/constraints/LTStockGlobalEnergyLevelReserveParticipation.h"

void LTStockGlobalEnergyLevelReserveParticipation::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.longTermStorageOfArea[pays].GlobalHydroIndex;

    if (!data.Simulation)
    {
        // 15 (t)
        // Stock participation is energy constrained (optional constraints)
        //  Sum(P_{res,t_st} * R_{min,res} +/- J_down/up * R_{lambda,t_st}) <= n_min * R_up
        // R_t : stock level at time t
        // P_{res,t_st} : power participation for reserve down res at time t_st
        // R_{min,res} : max power participation ratio
        // R_up : max stock level

        for (bool isUpReserve: {reserveIsUp, reserveIsDown})
        {
            builder.updateHourWithinWeek(pdt);

            for (int t = 0;
                 t < isUpReserve ? data.areaReserves[pays].referenceGlobalActivationDurationUp
                                 : data.areaReserves[pays].referenceGlobalActivationDurationDown;
                 t++)
            {
                for (auto& capacityReservation:
                     isUpReserve ? data.areaReserves[pays].areaCapacityReservationsUp
                                 : data.areaReserves[pays].areaCapacityReservationsDown)
                {
                    if (capacityReservation.AllLTStorageReservesParticipation.size())
                    {
                        RESERVE_PARTICIPATION_LTSTORAGE& reserveParticipation
                          = capacityReservation.AllLTStorageReservesParticipation[cluster];
                        builder.LTStorageClusterReserveParticipation(
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
                      isUpReserve ? data.areaReserves[pays].maxGlobalEnergyActivationRatioUp
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
                      .reservesIndices()
                      .LTStorageGlobalStockEnergyLevelParticipationUp[globalClusterIdx]
                      = builder.data.nombreDeContraintes;
                }
                else
                {
                    data.CorrespondanceCntNativesCntOptim[pdt]
                      .reservesIndices()
                      .LTStorageGlobalStockEnergyLevelParticipationDown[globalClusterIdx]
                      = builder.data.nombreDeContraintes;
                }

                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                isUpReserve ? namer.LTGlobalEnergyStockLevelReserveParticipationUp(
                                builder.data.nombreDeContraintes,
                                "LongTermStorage")
                            : namer.LTGlobalEnergyStockLevelReserveParticipationDown(
                                builder.data.nombreDeContraintes,
                                "LongTermStorage");
                builder.build();
            }
        }
    }
    else
    {
        // Lambda that count the number of reserveParticipations
        auto countReservesParticipations =
          [cluster](const std::vector<CAPACITY_RESERVATION>& reservations, int t_max)
        {
            int counter = 0;
            for (const auto& capacityReservation: reservations)
            {
                counter += capacityReservation.AllLTStorageReservesParticipation.size() * t_max;
            }
            return counter;
        };

        int nbTermsUp = countReservesParticipations(
          data.areaReserves[pays].areaCapacityReservationsUp,
          data.areaReserves[pays].referenceGlobalActivationDurationUp);
        int nbTermsDown = countReservesParticipations(
          data.areaReserves[pays].areaCapacityReservationsDown,
          data.areaReserves[pays].referenceGlobalActivationDurationDown);

        builder.data.nombreDeContraintes += (nbTermsUp > 0) + (nbTermsDown > 0);
    }
}
