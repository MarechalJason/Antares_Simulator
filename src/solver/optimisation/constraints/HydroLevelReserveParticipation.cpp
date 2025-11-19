#include "antares/solver/optimisation/constraints/HydroLevelReserveParticipation.h"
using namespace reserve;

void HydroLevelReserveParticipation::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.hydroOfArea[pays].GlobalHydroIndex;

    if (!data.Simulation)
    {
        // 15 (r) (1)
        // Participation of down reserves requires a sufficient level of stock
        // R_t + Sum(P_{res} * R_{min,res}) <= R_up
        // R_t : stock level at time t
        // P_{res} : power participation for reserve down res
        // R_{min,res} : max power participation ratio
        // R_up : max stock level
        {
            builder.updateHourWithinWeek(pdt);
            for (auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(DIRECTION::DOWN))
            {
                if (capacityReservation.AllHydroReservesParticipation.size())
                {
                    RESERVE_PARTICIPATION_HYDRO reserveParticipations
                      = capacityReservation.AllHydroReservesParticipation[cluster];
                    builder.HydroReserveParticipation(
                      DIRECTION::DOWN,
                      reserveParticipations.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio);
                }
            }
            if (builder.NumberOfVariables() > 0)
            {
                builder.HydroLevel(globalClusterIdx, 1.);
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .HydroLevelParticipationDown[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.HydroLevelReserveParticipation(DIRECTION::DOWN,
                                                     builder.data.nombreDeContraintes,
                                                     "Hydro");
                builder.build();
            }
        }

        // 15 (r) (2)
        // Participation of up reserves requires a sufficient level of stock
        // -R_t + Sum(P_{res} * R_{min,res}) <= -R_down
        // R_t : stock level at time t
        // P_{res} : power participation for reserve up res
        // R_{min,res} : max power participation ratio
        // R_down : min stock level
        {
            builder.updateHourWithinWeek(pdt);

            for (auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(DIRECTION::UP))
            {
                if (capacityReservation.AllHydroReservesParticipation.size())
                {
                    RESERVE_PARTICIPATION_HYDRO reserveParticipations
                      = capacityReservation.AllHydroReservesParticipation[cluster];
                    builder.HydroReserveParticipation(
                      DIRECTION::UP,
                      reserveParticipations.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio);
                }
            }
            if (builder.NumberOfVariables() > 0)
            {
                builder.HydroLevel(globalClusterIdx, -1.);
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .HydroLevelParticipationUp[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.HydroLevelReserveParticipation(DIRECTION::UP,
                                                     builder.data.nombreDeContraintes,
                                                     "Hydro");
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForHydroReserves(pays);
    }
}
