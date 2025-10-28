/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/constraints/ReserveParticipationGroup.h"

#include "antares/solver/optimisation/constraints/ConstraintGroup.h"
#include "antares/solver/optimisation/constraints/LTPumpingCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/LTPumpingMaxReserve.h"
#include "antares/solver/optimisation/constraints/LTReserveParticipation.h"
#include "antares/solver/optimisation/constraints/LTStockEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/LTStockGlobalEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/LTStockLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/LTTurbiningCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/LTTurbiningMaxReserve.h"
#include "antares/solver/optimisation/constraints/OffUnitsThermalParticipatingToReserves.h"
#include "antares/solver/optimisation/constraints/PMaxReserve.h"
#include "antares/solver/optimisation/constraints/POffUnits.h"
#include "antares/solver/optimisation/constraints/POutBounds.h"
#include "antares/solver/optimisation/constraints/POutCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"
#include "antares/solver/optimisation/constraints/STPumpingCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/STPumpingMaxReserve.h"
#include "antares/solver/optimisation/constraints/STReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STStockEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STStockGlobalEnergyLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STStockLevelReserveParticipation.h"
#include "antares/solver/optimisation/constraints/STTurbiningCapacityThreasholds.h"
#include "antares/solver/optimisation/constraints/STTurbiningMaxReserve.h"
#include "antares/solver/optimisation/constraints/SymmetryReserveParticipation.h"
#include "antares/solver/optimisation/constraints/ThermalReserveParticipation.h"

ReserveParticipationGroup::ReserveParticipationGroup(PROBLEME_HEBDO* problemeHebdo,
                                                     bool simulation,
                                                     ConstraintBuilder& builder):
    ConstraintGroup(problemeHebdo, builder)
{
    this->simulation_ = simulation;
}

ReserveData ReserveParticipationGroup::GetReserveDataFromProblemHebdo()
{
    return {.Simulation = simulation_,
            .areaReserves = problemeHebdo_->allReserves(),
            .thermalClusters = problemeHebdo_->PaliersThermiquesDuPays,
            .shortTermStorageOfArea = problemeHebdo_->ShortTermStorage,
            .longTermStorageOfArea = problemeHebdo_->CaracteristiquesHydrauliques,
            .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

/**
 * @brief build MinDownTime constraints with
 * respect to default order
 */
void ReserveParticipationGroup::BuildConstraints()
{
    {
        auto data = GetReserveDataFromProblemHebdo();
        PMaxReserve pMaxReserve(builder_, data);
        OffUnitsThermalParticipatingToReserves offUnitsThermalParticipatingToReserves(builder_,
                                                                                      data);
        POffUnits pOffUnits(builder_, data);
        ThermalReserveParticipation thermalReserveParticipation(builder_, data);
        ReserveSatisfaction reserveSatisfaction(builder_, data);
        STTurbiningMaxReserve STTurbiningMaxReserve(builder_, data);
        STPumpingMaxReserve STPumpingMaxReserve(builder_, data);
        STReserveParticipation STReserveParticipation(builder_, data);
        STStockEnergyLevelReserveParticipation STStockEnergyLevelReserveParticipation(builder_,
                                                                                      data);
        LTStockEnergyLevelReserveParticipation LTStockEnergyLevelReserveParticipation(builder_,
                                                                                      data);
        LTTurbiningMaxReserve LTTurbiningMaxReserve(builder_, data);
        LTPumpingMaxReserve LTPumpingMaxReserve(builder_, data);
        LTReserveParticipation LTReserveParticipation(builder_, data);

        SymmetryReserveParticipation symmetryReserveParticipation(builder_, data);

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            // Adding constraints for ReservesUp and ReservesDown
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
                // Thermal clusters reserve participations
                {
                    auto& areaReservesUp = data.areaReserves[pays].areaCapacityReservationsUp;
                    const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                                          ->PaliersThermiquesDuPays
                                                                            [pays];
                    for (int cluster = 0;
                         cluster < PaliersThermiquesDuPays.NombreDePaliersThermiques;
                         cluster++)
                    {
                        // 16 quater
                        pOffUnits.add(pays, cluster, pdt);
                    }

                    uint32_t reserve = 0;
                    for (const auto& areaReserveUp: areaReservesUp)
                    {
                        // 24
                        reserveSatisfaction.add(pays, reserve, pdt, true);

                        for (const auto& [clusterId, clusterReserveParticipation]:
                             areaReserveUp.AllThermalReservesParticipation)
                        {
                            // 16 bis
                            pMaxReserve.add(pays, reserve, clusterId, pdt, true);

                            if (clusterReserveParticipation.maxPowerOff > 0)
                            {
                                // 16 ter
                                offUnitsThermalParticipatingToReserves.add(pays,
                                                                           reserve,
                                                                           clusterId,
                                                                           pdt);
                            }

                            // 17 quinquies
                            thermalReserveParticipation.add(pays, reserve, clusterId, pdt, true);
                        }
                        reserve++;
                    }

                    reserve = 0;
                    auto& areaReservesDown = data.areaReserves[pays].areaCapacityReservationsDown;
                    for (const auto& areaReserveDown: areaReservesDown)
                    {
                        // 24
                        reserveSatisfaction.add(pays, reserve, pdt, false);

                        for (const auto& [clusterId, clusterReserveParticipation]:
                             areaReserveDown.AllThermalReservesParticipation)
                        {
                            // 16 bis
                            pMaxReserve.add(pays, reserve, clusterId, pdt, false);

                            // 17 sexies
                            thermalReserveParticipation.add(pays, reserve, clusterId, pdt, false);
                        }
                        reserve++;
                    }

                    // Thermal cluster Symmetries
                    for (const auto& [clusterId, symmetries]:
                         data.areaReserves[pays].ThermalReservesParticipationSymmetries)
                    {
                        // Add symmetry constraint between first element and all others
                        for (const auto& symmetry: symmetries)
                        {
                            // 18
                            symmetryReserveParticipation.add(pays, symmetry, pdt);
                        }
                    }
                }

                // ShortTerm Storage reserve participations
                {
                    auto STSAddReservesParticipations =
                      [pays, data, pdt](bool isUpReserve,
                                        auto STTurbiningMaxReserve,
                                        auto STPumpingMaxReserve,
                                        auto STReserveParticipation,
                                        auto STStockEnergyLevelReserveParticipation)
                    {
                        auto& areaReserves = isUpReserve
                                               ? data.areaReserves[pays].areaCapacityReservationsUp
                                               : data.areaReserves[pays]
                                                   .areaCapacityReservationsDown;
                        uint32_t reserve = 0;
                        for (const auto& areaReserve: areaReserves)
                        {
                            for (const auto& [clusterId, clusterReserveParticipation]:
                                 areaReserve.AllSTStorageReservesParticipation)
                            {
                                // 15 (k)
                                STTurbiningMaxReserve.add(
                                  pays,
                                  reserve,
                                  clusterReserveParticipation.clusterIdInArea,
                                  pdt,
                                  isUpReserve);
                                // 15 (l)
                                STPumpingMaxReserve.add(pays,
                                                        reserve,
                                                        clusterReserveParticipation.clusterIdInArea,
                                                        pdt,
                                                        isUpReserve);
                                // 15 (o & p)
                                STReserveParticipation.add(
                                  pays,
                                  reserve,
                                  clusterReserveParticipation.clusterIdInArea,
                                  pdt,
                                  isUpReserve);

                                // 15 (h)
                                STStockEnergyLevelReserveParticipation.add(
                                  pays,
                                  clusterReserveParticipation.clusterIdInArea,
                                  reserve,
                                  pdt,
                                  isUpReserve);
                            }
                            reserve++;
                        }
                    };
                    STSAddReservesParticipations(thisReserveIsUp,
                                                 STTurbiningMaxReserve,
                                                 STPumpingMaxReserve,
                                                 STReserveParticipation,
                                                 STStockEnergyLevelReserveParticipation);
                    STSAddReservesParticipations(thisReserveIsDown,
                                                 STTurbiningMaxReserve,
                                                 STPumpingMaxReserve,
                                                 STReserveParticipation,
                                                 STStockEnergyLevelReserveParticipation);

                    // ShortTerm Storage cluster Symmetries
                    for (const auto& [clusterId, symmetries]:
                         data.areaReserves[pays].STStorageReservesParticipationSymmetries)
                    {
                        for (const auto& symmetry: symmetries)
                        {
                            // 18
                            symmetryReserveParticipation.add(pays, symmetry, pdt);
                        }
                    }
                }

                // LongTerm Storage reserve participations
                {
                    auto LTSAddReservesParticipations =
                      [pays, data, pdt](bool isUpReserve,
                                        auto LTTurbiningMaxReserve,
                                        auto LTPumpingMaxReserve,
                                        auto LTReserveParticipation,
                                        auto LTStockEnergyLevelReserveParticipation)
                    {
                        auto& areaReserves = isUpReserve
                                               ? data.areaReserves[pays].areaCapacityReservationsUp
                                               : data.areaReserves[pays]
                                                   .areaCapacityReservationsDown;
                        uint32_t reserve = 0;
                        for (const auto& areaReserve: areaReserves)
                        {
                            for (const auto& clusterReserveParticipation:
                                 areaReserve.AllLTStorageReservesParticipation)
                            {
                                // 15 (a)
                                LTTurbiningMaxReserve.add(
                                  pays,
                                  reserve,
                                  clusterReserveParticipation.clusterIdInArea,
                                  pdt,
                                  isUpReserve);
                                // 15 (b)
                                LTPumpingMaxReserve.add(pays,
                                                        reserve,
                                                        clusterReserveParticipation.clusterIdInArea,
                                                        pdt,
                                                        isUpReserve);
                                // 15 (e & f)
                                LTReserveParticipation.add(
                                  pays,
                                  reserve,
                                  clusterReserveParticipation.clusterIdInArea,
                                  pdt,
                                  isUpReserve);

                                // 15 (s)
                                LTStockEnergyLevelReserveParticipation.add(
                                  pays,
                                  clusterReserveParticipation.clusterIdInArea,
                                  reserve,
                                  pdt,
                                  isUpReserve);
                            }
                            reserve++;
                        }
                    };
                    LTSAddReservesParticipations(thisReserveIsUp,
                                                 LTTurbiningMaxReserve,
                                                 LTPumpingMaxReserve,
                                                 LTReserveParticipation,
                                                 LTStockEnergyLevelReserveParticipation);
                    LTSAddReservesParticipations(thisReserveIsDown,
                                                 LTTurbiningMaxReserve,
                                                 LTPumpingMaxReserve,
                                                 LTReserveParticipation,
                                                 LTStockEnergyLevelReserveParticipation);

                    // LongTerm Storage Symmetries
                    for (const auto& symmetry:
                         data.areaReserves[pays].LTStorageReservesParticipationSymmetries)
                    {
                        // 18
                        symmetryReserveParticipation.add(pays, symmetry, pdt);
                    }
                }
            }
        }
    }
    {
        auto data = GetReserveDataFromProblemHebdo();
        POutCapacityThreasholds pOutCapacityThreasholds(builder_, data);
        POutBounds pOutBounds(builder_, data);
        STTurbiningCapacityThreasholds STTurbiningCapacityThreasholds(builder_, data);
        STPumpingCapacityThreasholds STPumpingCapacityThreasholds(builder_, data);
        LTTurbiningCapacityThreasholds LTTurbiningCapacityThreasholds(builder_, data);
        LTPumpingCapacityThreasholds LTPumpingCapacityThreasholds(builder_, data);
        LTStockLevelReserveParticipation LTStockLevelReserveParticipation(builder_, data);
        STStockLevelReserveParticipation STStockLevelReserveParticipation(builder_, data);
        STStockGlobalEnergyLevelReserveParticipation STStockGlobalEnergyLevelReserveParticipation(
          builder_,
          data);
        LTStockGlobalEnergyLevelReserveParticipation LTStockGlobalEnergyLevelReserveParticipation(
          builder_,
          data);

        for (int pdt = 0; pdt < problemeHebdo_->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
            {
                // Thermal Clusters
                const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo_
                                                                      ->PaliersThermiquesDuPays
                                                                        [pays];
                for (int cluster = 0; cluster < PaliersThermiquesDuPays.NombreDePaliersThermiques;
                     cluster++)
                {
                    // 17 bis
                    pOutCapacityThreasholds.add(pays, cluster, pdt);

                    // 17 ter
                    pOutBounds.add(pays, cluster, pdt);
                }

                // Short Term Storage Clusters
                const auto& STStorageDuPays = problemeHebdo_->ShortTermStorage[pays];
                for (int cluster = 0; cluster < STStorageDuPays.size(); cluster++)
                {
                    // 15 (m)
                    STTurbiningCapacityThreasholds.add(pays, cluster, pdt);
                    // 15 (n)
                    STPumpingCapacityThreasholds.add(pays, cluster, pdt);
                    // 15 (g)
                    STStockLevelReserveParticipation.add(pays, cluster, pdt);
                    // 15 (i)
                    STStockGlobalEnergyLevelReserveParticipation.add(pays, cluster, pdt);
                }

                // Long Term Storage Clusters
                // Check if the LT Storage is participating to the reserves
                auto isClusterParticipatingToReserves =
                  [](std::vector<CAPACITY_RESERVATION>& reserves)
                {
                    auto hasReserveParticipations = [](CAPACITY_RESERVATION& res)
                    { return res.AllLTStorageReservesParticipation.size() > 0; };
                    return std::any_of(reserves.begin(), reserves.end(), hasReserveParticipations);
                };

                if (isClusterParticipatingToReserves(
                      problemeHebdo_->allReserves()[pays].areaCapacityReservationsDown)
                    || isClusterParticipatingToReserves(
                      problemeHebdo_->allReserves()[pays].areaCapacityReservationsUp))
                {
                    // 15 (c)
                    LTTurbiningCapacityThreasholds.add(pays, 0, pdt);
                    // 15 (d)
                    LTPumpingCapacityThreasholds.add(pays, 0, pdt);
                    // 15 (r)
                    LTStockLevelReserveParticipation.add(pays, 0, pdt);
                    // 15 (t)
                    LTStockGlobalEnergyLevelReserveParticipation.add(pays, 0, pdt);
                }
            }
        }
    }
}
