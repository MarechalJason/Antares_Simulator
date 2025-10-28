#include "antares/solver/optimisation/constraints/LTReserveParticipation.h"

void LTReserveParticipation::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 15 (e & f)
        // Participation to the up or down reserve is the sum of the turbining and pumping
        // participation constraint : P_res = H_res + Π_res H : Turbining participation to reserve
        // Π : Pumping participation to reserve
        // P : Up (e) or down (f) Reserve Participation

        CAPACITY_RESERVATION& capacityReservation = isUpReserve
                                                      ? data.areaReserves[pays]
                                                          .areaCapacityReservationsUp[reserve]
                                                      : data.areaReserves[pays]
                                                          .areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_LTSTORAGE& reserveParticipation = capacityReservation
                                                                  .AllLTStorageReservesParticipation
                                                                    [cluster];
        builder.updateHourWithinWeek(pdt)
          .LTStorageTurbiningClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            isUpReserve ? -1.0 : 1.0)
          .LTStoragePumpingClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            isUpReserve ? -1.0 : 1.0)
          .LTStorageClusterReserveParticipation(
            isUpReserve,
            reserveParticipation.globalIndexClusterParticipation,
            isUpReserve ? 1.0 : -1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.LTReserveParticipation(isUpReserve,
                                     builder.data.nombreDeContraintes,
                                     reserveParticipation.clusterName,
                                     capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
