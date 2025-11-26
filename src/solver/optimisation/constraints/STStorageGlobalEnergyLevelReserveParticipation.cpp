#include "antares/solver/optimisation/constraints/STStorageGlobalEnergyLevelReserveParticipation.h"
using namespace reserve;

void STStorageGlobalEnergyLevelReserveParticipation::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (i)
        // Stock participation is energy constrained (optional constraints)
        //  Sum(P_{res,t_st} * R_{min,res} +/- J_down/up * R_{lambda,t_st}) <= n_min * R_up
        // R_t : stock level at time t
        // P_{res,t_st} : power participation for reserve down res at time t_st
        // R_{min,res} : max power participation ratio
        // R_up : max stock level

        for (auto dir: {Type::DOWN, Type::UP})
        {
            builder.updateHourWithinWeek(pdt);

            for (int t = 0; t < data.areaReserves[pays].referenceGlobalActivationDuration[dir]; t++)
            {
                for (auto& capacityReservation:
                     data.areaReserves[pays].areaCapacityReservations | filter(dir))
                {
                    if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                    {
                        RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation
                          = capacityReservation.AllSTStorageReservesParticipation[cluster];
                        builder.STStorageClusterReserveParticipation(
                          capacityReservation.direction,
                          reserveParticipation.globalIndexClusterParticipation,
                          capacityReservation.powerActivationRatio,
                          t,
                          builder.data.NombreDePasDeTempsPourUneOptimisation);
                    }
                }
                if (builder.NumberOfVariables() > 0)
                {
                    int sign = dir == Type::UP ? -1 : 1;
                    builder.ShortTermStorageLevel(
                      globalClusterIdx,
                      sign * data.areaReserves[pays].maxGlobalEnergyActivationRatio[dir],
                      t,
                      builder.data.NombreDePasDeTempsPourUneOptimisation);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.lessThan();

                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .STStorageGlobalStockEnergyLevelParticipation[dir][globalClusterIdx]
                  = builder.data.nombreDeContraintes;

                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STGlobalEnergyStockLevelReserveParticipation(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name,
                  dir);
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForSTStorageReserves(
          pays,
          cluster,
          true /* account for global activation duration*/);
    }
}
