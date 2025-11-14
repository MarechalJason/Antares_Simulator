#include "antares/solver/optimisation/constraints/STReserveParticipation.h"

void STReserveParticipation::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 15 (o & p)
        // Participation to the up reserve is the sum of the release and store participation
        // constraint : P_res = H_res + Π_res
        // H : Release participation to reserve
        // Π : Store participation to reserve
        // P : Up Reserve Participation

        CAPACITY_RESERVATION& capacityReservation = isUpReserve
                                                      ? data.areaReserves[pays]
                                                          .areaCapacityReservationsUp[reserve]
                                                      : data.areaReserves[pays]
                                                          .areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation = capacityReservation
                                                                  .AllSTStorageReservesParticipation
                                                                    [cluster];

        int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

        builder.updateHourWithinWeek(pdt)
          .STStorageReleaseClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            isUpReserve ? -1.0 : 1.0)
          .STStorageStoreClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            isUpReserve ? -1.0 : 1.0)
          .STStorageClusterReserveParticipation(
            isUpReserve,
            reserveParticipation.globalIndexClusterParticipation,
            isUpReserve ? 1.0 : -1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.STReserveParticipation(builder.data.nombreDeContraintes,
                                     reserveParticipation.clusterName,
                                     capacityReservation.reserveName,
                                     isUpReserve);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
