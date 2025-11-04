#include "antares/solver/optimisation/constraints/STStockLevelReserveParticipation.h"

void STStockLevelReserveParticipation::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (g) (1)
        // Participation of down reserves requires a sufficient level of stock
        // R_t + Sum(P_{res} * R_{min,res}) <= R_up
        // R_t : stock level at time t
        // P_{res} : power participation for reserve down res
        // R_{min,res} : max power participation ratio
        // R_up : max stock level
        {
            builder.updateHourWithinWeek(pdt);

            for (auto& capacityReservation: data.areaReserves[pays].areaCapacityReservationsDown)
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    RESERVE_PARTICIPATION_STSTORAGE reserveParticipations
                      = capacityReservation.AllSTStorageReservesParticipation[cluster];
                    builder.STStorageClusterReserveParticipation(
                      reserveIsDown,
                      reserveParticipations.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio);
                }
            }
            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageLevel(globalClusterIdx, 1.);
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices()
                  .STStorageLevelParticipationDown[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STStockLevelReserveParticipationDown(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (g) (2)
        // Participation of up reserves requires a sufficient level of stock
        // -R_t + Sum(P_{res} * R_{min,res}) <= -R_down
        // R_t : stock level at time t
        // P_{res} : power participation for reserve up res
        // R_{min,res} : max power participation ratio
        // R_down : min stock level
        {
            builder.updateHourWithinWeek(pdt);

            for (auto& capacityReservation: data.areaReserves[pays].areaCapacityReservationsUp)
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    RESERVE_PARTICIPATION_STSTORAGE reserveParticipations
                      = capacityReservation.AllSTStorageReservesParticipation[cluster];
                    builder.STStorageClusterReserveParticipation(
                      reserveIsUp,
                      reserveParticipations.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio);
                }
            }
            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageLevel(globalClusterIdx, -1.);
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices()
                  .STStorageLevelParticipationUp[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STStockLevelReserveParticipationUp(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForSTStorageReserves(
          pays,
          cluster);
    }
}
