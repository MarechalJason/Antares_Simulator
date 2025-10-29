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

#include <spx_constantes_externes.h>

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_rename_problem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireReserves(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    VariableNamer variableNamer(problemeHebdo->ProblemeAResoudre->NomDesVariables);
    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    struct ReserveVariablesInitializer
    {
        PROBLEME_HEBDO* problemeHebdo;
        bool Simulation;
        const std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre;
        int& NombreDeVariables;
        VariableNamer& variableNamer;
        VariableManagement::VariableManager variableManager;

        ReserveVariablesInitializer(PROBLEME_HEBDO* hebdo, bool sim, VariableNamer& namer):
            problemeHebdo(hebdo),
            Simulation(sim),
            ProblemeAResoudre(hebdo->ProblemeAResoudre),
            NombreDeVariables(ProblemeAResoudre->NombreDeVariables),
            variableNamer(namer),
            variableManager(VariableManagerFromProblemHebdo(hebdo))
        {
        }

        // Init variables for a reserve
        void initReserve(int pdt, const int reserveIndex, const std::string& reserveName)
        {
            if (Simulation)
            {
                NombreDeVariables += 2;
            }
            else
            {
                // For Unsatisfied Reserves
                variableManager.InternalUnsatisfiedReserve(reserveIndex, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.InternalUnsatisfiedReserve(NombreDeVariables, reserveName);
                NombreDeVariables++;

                // For Excess Reserves
                variableManager.InternalExcessReserve(reserveIndex, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.InternalExcessReserve(NombreDeVariables, reserveName);
                NombreDeVariables++;
            }
        }

        // Init variables for a Thermal cluster participation to a reserve up or down
        void initThermalReserveParticipation(
          int pdt,
          const RESERVE_PARTICIPATION_THERMAL& clusterReserveParticipation,
          const std::string& reserveName,
          bool isUpReserve)
        {
            const auto& clusterName = clusterReserveParticipation.clusterName;
            if (Simulation)
            {
                NombreDeVariables += isUpReserve ? 4 : 2; // 4 for up reserves, 2 for down
            }
            else
            {
                // For running units in cluster
                variableManager.RunningThermalClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfRunningUnitsToReserve(NombreDeVariables,
                                                                   clusterName,
                                                                   reserveName);
                NombreDeVariables++;

                if (isUpReserve) // For off units in cluster (off units can not participate to down
                                 // reserves)
                {
                    variableManager.OffThermalClusterReserveParticipation(
                      clusterReserveParticipation.globalIndexClusterParticipation,
                      pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.ParticipationOfOffUnitsToReserve(NombreDeVariables,
                                                                   clusterName,
                                                                   reserveName);
                    NombreDeVariables++;

                    variableManager.PowerOfOffUnitsParticipatingToReserve(
                      clusterReserveParticipation.globalIndexClusterParticipation,
                      pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    ProblemeAResoudre->VariablesEntieres[NombreDeVariables]
                      = problemeHebdo->OptimisationAvecVariablesEntieres;
                    variableNamer.PowerOfOffUnitsParticipatingToReserve(NombreDeVariables,
                                                                        clusterName,
                                                                        reserveName);
                    NombreDeVariables++;
                }

                // For all units in cluster
                variableManager.ThermalClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ThermalClusterReserveParticipation(NombreDeVariables,
                                                                 clusterName,
                                                                 reserveName);
                NombreDeVariables++;
            }
        }

        // Init variables for a ShortTerm cluster participation to a reserve
        void initSTStorageReserveParticipation(
          bool isUpReserve,
          int pdt,
          const RESERVE_PARTICIPATION_STSTORAGE& clusterReserveParticipation,
          const std::string& reserveName)
        {
            const auto& clusterName = clusterReserveParticipation.clusterName;
            if (Simulation)
            {
                NombreDeVariables += 3;
            }
            else
            {
                // For Turbining participation to the reserves
                variableManager.STStorageTurbiningClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfSTStorageTurbiningToReserve(NombreDeVariables,
                                                                         clusterName,
                                                                         reserveName);
                NombreDeVariables++;

                // For Pumping participation to the reserves
                variableManager.STStoragePumpingClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfSTStoragePumpingToReserve(NombreDeVariables,
                                                                       clusterName,
                                                                       reserveName);
                NombreDeVariables++;

                // For Short Term Storage participation to the up reserves
                variableManager.STStorageClusterReserveParticipation(
                  isUpReserve,
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;

                variableNamer.ParticipationOfSTStorageToReserve(isUpReserve,
                                                                NombreDeVariables,
                                                                clusterName,
                                                                reserveName);
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                NombreDeVariables++;
            }
        }

        // Init variables for a LongTerm cluster participation to a reserve
        void initLTStorageReserveParticipation(
          bool isUpReserve,
          int pdt,
          const RESERVE_PARTICIPATION_LTSTORAGE& clusterReserveParticipation,
          const std::string& reserveName)
        {
            const auto& clusterName = clusterReserveParticipation.clusterName;
            if (Simulation)
            {
                NombreDeVariables += 3;
            }
            else
            {
                // For Turbining participation to the reserves
                variableManager.LTStorageTurbiningClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfLTStorageTurbiningToReserve(NombreDeVariables,
                                                                         clusterName,
                                                                         reserveName);
                NombreDeVariables++;

                // For Pumping participation to the reserves
                variableManager.LTStoragePumpingClusterReserveParticipation(
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.ParticipationOfLTStoragePumpingToReserve(NombreDeVariables,
                                                                       clusterName,
                                                                       reserveName);
                NombreDeVariables++;

                // For Long Term Storage participation to the reserves
                variableManager.LTStorageClusterReserveParticipation(
                  isUpReserve,
                  clusterReserveParticipation.globalIndexClusterParticipation,
                  pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                variableNamer.ParticipationOfLTStorageToReserve(isUpReserve,
                                                                NombreDeVariables,
                                                                clusterName,
                                                                reserveName);

                NombreDeVariables++;
            }
        }
    };

    ReserveVariablesInitializer reserveVariablesInitializer(problemeHebdo,
                                                            Simulation,
                                                            variableNamer);
    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        auto& CorrespondanceVarNativesVarOptim = problemeHebdo
                                                   ->CorrespondanceVarNativesVarOptim[pdt];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            auto areaReserves = problemeHebdo->allReserves()[pays];

            auto initAllReserves =
              [pays, pdt](auto areaReserves, auto* reserveVariablesInitializer, bool isUpReserve)
            {
                for (auto& areaReserve: isUpReserve ? areaReserves.areaCapacityReservationsUp
                                                    : areaReserves.areaCapacityReservationsDown)
                {
                    reserveVariablesInitializer->initReserve(pdt,
                                                             areaReserve.globalReserveIndex,
                                                             areaReserve.reserveName);

                    // Thermal Clusters
                    for (auto& [clusterId, clusterReserveParticipation]:
                         areaReserve.AllThermalReservesParticipation)
                    {
                        reserveVariablesInitializer->initThermalReserveParticipation(
                          pdt,
                          clusterReserveParticipation,
                          areaReserve.reserveName,
                          isUpReserve);
                    }

                    // Short Term Storage Clusters
                    for (auto& [clusterId, clusterReserveParticipation]:
                         areaReserve.AllSTStorageReservesParticipation)
                    {
                        reserveVariablesInitializer->initSTStorageReserveParticipation(
                          isUpReserve,
                          pdt,
                          clusterReserveParticipation,
                          areaReserve.reserveName);
                    }

                    // Long Term Storage Clusters
                    for (auto& clusterReserveParticipation:
                         areaReserve.AllLTStorageReservesParticipation)
                    {
                        reserveVariablesInitializer->initLTStorageReserveParticipation(
                          isUpReserve,
                          pdt,
                          clusterReserveParticipation,
                          areaReserve.reserveName);
                    }
                }
            };
            initAllReserves(areaReserves, &reserveVariablesInitializer, reserveIsUp);
            initAllReserves(areaReserves, &reserveVariablesInitializer, reserveIsDown);
        }
    }
}
