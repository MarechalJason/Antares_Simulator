#include "antares/solver/optimisation/constraints/STReleaseCapacityThreasholds.h"

void STReleaseCapacityThreasholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (m)
        // Release power remains within limits set by minimum stable power (0) and maximum
        // capacity threasholds Hmin + Sum(H^on_re-) <= H <= Hmax - Sum(H^on_re+) H^on_re- :
        // Release Participation of cluster to Down reserves H^on_re+ : Release Participation of
        // cluster to Up reserves H : Release Power output from cluster Hmax : Maximum Release
        // Power from cluster

        // 15 (m) (1) : H - Sum(H^on_re-) >= Hmin
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservationsDown)
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    auto& reserveParticipation = capacityReservation
                                                   .AllSTStorageReservesParticipation.at(cluster);
                    builder.STStorageReleaseClusterReserveParticipation(
                      reserveParticipation.globalIndexClusterParticipation,
                      -1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageWithdrawal(globalClusterIdx, 1).greaterThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices()
                  .STStorageClusterReleaseCapacityThreasholdsMin[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STReleaseCapacityThreasholdsDown(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (m) (2) :  H + Sum(H^on_re+) <= Hmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservationsUp)
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    auto& reserveParticipation = capacityReservation
                                                   .AllSTStorageReservesParticipation.at(cluster);
                    builder.STStorageReleaseClusterReserveParticipation(
                      reserveParticipation.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageWithdrawal(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices()
                  .STStorageClusterReleaseCapacityThreasholdsMax[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STReleaseCapacityThreasholdsUp(
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
