#include "antares/solver/optimisation/constraints/LTTurbiningCapacityThreasholds.h"

void LTTurbiningCapacityThreasholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.longTermStorageOfArea[pays].GlobalHydroIndex;

    if (!data.Simulation)
    {
        // 15 (c)
        // Turbining power remains within limits set by minimum stable power (0) and maximum
        // capacity threasholds Hmin + Sum(H^on_re-) <= H <= Hmax - Sum(H^on_re+) H^on_re- :
        // Turbining Participation of cluster to Down reserves H^on_re+ : Turbining Participation of
        // cluster to Up reserves H : Turbining Power output from cluster Hmax : Maximum Turbining
        // Power from cluster

        // 15 (c) (1) : H - Sum(H^on_re-) >= Hmin
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations:
                     capacityReservation.AllLTStorageReservesParticipation)
                {
                    builder.LTStorageTurbiningClusterReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      -1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                if (data.longTermStorageOfArea[pays].PresenceDHydrauliqueModulable)
                {
                    builder.HydProd(pays, 1);
                }
                builder.greaterThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices()
                  .LTStorageClusterTurbiningCapacityThreasholdsMin[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.LTTurbiningCapacityThreasholdsDown(builder.data.nombreDeContraintes,
                                                         "LongTermStorage");
                builder.build();
            }
        }

        // 15 (c) (2) :  H + Sum(H^on_re+) <= Hmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations:
                     capacityReservation.AllLTStorageReservesParticipation)
                {
                    builder.LTStorageTurbiningClusterReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                if (data.longTermStorageOfArea[pays].PresenceDHydrauliqueModulable)
                {
                    builder.HydProd(pays, 1);
                }
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices()
                  .LTStorageClusterTurbiningCapacityThreasholdsMax[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.LTTurbiningCapacityThreasholdsUp(builder.data.nombreDeContraintes,
                                                       "LongTermStorage");
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForLTStorageReserves(pays);
    }
}
