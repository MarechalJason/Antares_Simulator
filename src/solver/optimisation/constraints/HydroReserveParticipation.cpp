#include "antares/solver/optimisation/constraints/HydroReserveParticipation.h"

void HydroReserveParticipation::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 15 (e & f)
        // Participation to the up or down reserve is the sum of the release and store
        // participation constraint : P_res = H_res + Π_res H : Release participation to reserve
        // Π : Store participation to reserve
        // P : Up (e) or down (f) Reserve Participation

        CAPACITY_RESERVATION& capacityReservation = isUpReserve
                                                      ? data.areaReserves[pays]
                                                          .areaCapacityReservationsUp[reserve]
                                                      : data.areaReserves[pays]
                                                          .areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_HYDRO& reserveParticipation = capacityReservation
                                                              .AllHydroReservesParticipation
                                                                [cluster];
        builder.updateHourWithinWeek(pdt)
          .HydroReleaseReserveParticipation(reserveParticipation.globalIndexClusterParticipation,
                                            isUpReserve ? -1.0 : 1.0)
          .HydroStoreReserveParticipation(reserveParticipation.globalIndexClusterParticipation,
                                          isUpReserve ? -1.0 : 1.0)
          .HydroReserveParticipation(isUpReserve,
                                     reserveParticipation.globalIndexClusterParticipation,
                                     isUpReserve ? 1.0 : -1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.HydroReserveParticipation(isUpReserve,
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
