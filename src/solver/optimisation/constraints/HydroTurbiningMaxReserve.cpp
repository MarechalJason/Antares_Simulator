#include "antares/solver/optimisation/constraints/HydroTurbiningMaxReserve.h"

void HydroTurbiningMaxReserve::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 15 (a)
        // Participation to the reserve using turbining is bounded by the max turbining reserve
        // participation of the cluster constraint : H <= H^max H : Turbining power H^max : Maximum
        // accessible power of the cluster for the reserve

        CAPACITY_RESERVATION& capacityReservation = isUpReserve
                                                      ? data.areaReserves[pays]
                                                          .areaCapacityReservationsUp[reserve]
                                                      : data.areaReserves[pays]
                                                          .areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_HYDRO& reserveParticipation = capacityReservation
                                                              .AllHydroReservesParticipation
                                                                [cluster];

        builder.updateHourWithinWeek(pdt)
          .HydroTurbiningReserveParticipation(reserveParticipation.globalIndexClusterParticipation,
                                              1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .reservesIndices()
          .HydroMaxTurbiningParticipation[reserveParticipation.globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.HydroTurbiningMaxReserve(builder.data.nombreDeContraintes,
                                       reserveParticipation.clusterName,
                                       capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
