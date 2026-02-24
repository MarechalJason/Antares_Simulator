// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <spx_constantes_externes.h>

#include "antares/solver/optimisation/opt_rename_problem.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "variables/VariableManagerUtils.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(PROBLEME_HEBDO*,
                                                                                   bool);

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;
    int NombreDeVariables = 0;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            variableManager.ntcDirect(interco, pdt) = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;

            const auto origin = problemeHebdo->NomsDesPays
                                  [problemeHebdo->PaysOrigineDeLInterconnexion[interco]];
            const auto destination = problemeHebdo->NomsDesPays
                                       [problemeHebdo->PaysExtremiteDeLInterconnexion[interco]];
            variableNamer.updateExtremities(origin, destination);
            variableNamer.ntcDirect(NombreDeVariables);
            NombreDeVariables++;

            if (problemeHebdo->CoutDeTransport[interco].IntercoGereeAvecDesCouts)
            {
                variableManager.interconnectionDirectCost(interco, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.interconnectionDirectCost(NombreDeVariables);
                NombreDeVariables++;
                variableManager.interconnectionIndirectCost(interco, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.interconnectionIndirectCost(NombreDeVariables);
                NombreDeVariables++;
            }
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                                  ->PaliersThermiquesDuPays[pays];
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                const int palier = PaliersThermiquesDuPays
                                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                const auto& clusterName = PaliersThermiquesDuPays.NomsDesPaliersThermiques[index];
                variableManager.dispatchableProduction(palier, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.dispatchableProduction(NombreDeVariables, clusterName);
                NombreDeVariables++;
            }

            for (const auto& storage: problemeHebdo->ShortTermStorage[pays])
            {
                const int clusterGlobalIndex = storage.clusterGlobalIndex;
                // 1. Injection
                variableManager.shortTermStorageInjection(clusterGlobalIndex, pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.shortTermStorageInjection(NombreDeVariables, storage.name);
                NombreDeVariables++;
                // 2. Withdrawal
                variableManager.shortTermStorageWithdrawal(clusterGlobalIndex, pdt)
                  = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.shortTermStorageWithdrawal(NombreDeVariables, storage.name);
                NombreDeVariables++;
                // 3. Level
                variableManager.shortTermStorageLevel(clusterGlobalIndex, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.shortTermStorageLevel(NombreDeVariables, storage.name);
                NombreDeVariables++;

                // 4. Cost Variation Injection
                if (storage.penalizeVariationInjection)
                {
                    variableManager.shortTermStorageCostVariationInjection(clusterGlobalIndex, pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.shortTermStorageCostVariationInjection(NombreDeVariables,
                                                                         storage.name);
                    ++NombreDeVariables;
                }
                // 5. Cost Variation Withdrawal
                if (storage.penalizeVariationWithdrawal)
                {
                    variableManager.shortTermStorageCostVariationWithdrawal(clusterGlobalIndex, pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.shortTermStorageCostVariationWithdrawal(NombreDeVariables,
                                                                          storage.name);
                    ++NombreDeVariables;
                }
                // 6. Overflow
                if (storage.allowOverflow)
                {
                    variableManager.shortTermStorageOverflow(clusterGlobalIndex, pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.shortTermStorageOverflow(NombreDeVariables, storage.name);
                    NombreDeVariables++;
                }
            }

            variableManager.unsuppliedEnergy(pays, pdt) = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            variableNamer.unsuppliedEnergy(NombreDeVariables);
            NombreDeVariables++;

            variableManager.spillage(pays, pdt) = NombreDeVariables;

            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
            variableNamer.spillage(NombreDeVariables);
            NombreDeVariables++;
        }

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
            {
                variableManager.hydroPower(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.hydroPower(NombreDeVariables);
                NombreDeVariables++;
            }
            else
            {
                variableManager.hydroPower(pays, pdt) = -1;
            }
            variableManager.hydroPowerDown(pays, pdt) = -1;
            variableManager.hydroPowerUp(pays, pdt) = -1;
            if (problemeHebdo->TypeDeLissageHydraulique
                == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
            {
                if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                {
                    variableManager.hydroPowerDown(pays, pdt) = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.hydroPowerDown(NombreDeVariables);
                    NombreDeVariables++;
                    variableManager.hydroPowerUp(pays, pdt) = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_INFERIEUREMENT;
                    variableNamer.hydroPowerUp(NombreDeVariables);
                    NombreDeVariables++;
                }
            }
            else if (problemeHebdo->TypeDeLissageHydraulique
                       == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX
                     && problemeHebdo->CaracteristiquesHydrauliques[pays]
                          .PresenceDHydrauliqueModulable
                     && pdt == 0)
            {
                variableManager.hydroPowerDown(pays, pdt) = NombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.hydroPowerDown(NombreDeVariables);
                NombreDeVariables++;
                variableManager.hydroPowerUp(pays, pdt) = NombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.hydroPowerUp(NombreDeVariables);
                NombreDeVariables++;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
            {
                variableManager.pumping(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.pumping(NombreDeVariables);
                NombreDeVariables++;
            }
            else
            {
                variableManager.pumping(pays, pdt) = -1;
            }

            if (problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
            {
                variableManager.hydroLevel(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.hydroLevel(NombreDeVariables);
                NombreDeVariables++;
                variableManager.overflow(pays, pdt) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.overflow(NombreDeVariables);
                NombreDeVariables++;
            }
            else
            {
                variableManager.hydroLevel(pays, pdt) = -1;
                variableManager.overflow(pays, pdt) = -1;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168
                                     + NombreDePasDeTempsPourUneOptimisation - 1);
        variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            variableManager.finalStorage(pays) = NombreDeVariables;
            ProblemeAResoudre->TypeDeVariable[NombreDeVariables] = VARIABLE_NON_BORNEE;
            variableNamer.finalStorage(NombreDeVariables);
            NombreDeVariables++;

            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                variableManager.layerStorage(pays, nblayer) = NombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;
                variableNamer.layerStorage(NombreDeVariables, nblayer);
                NombreDeVariables++;
            }
        }
        else
        {
            variableManager.finalStorage(pays) = -1;
            for (uint nblayer = 0; nblayer < 100; nblayer++)
            {
                variableManager.layerStorage(pays, nblayer) = -1;
            }
        }
    }

    ProblemeAResoudre->NombreDeVariables = NombreDeVariables;

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                      false);
    }

    return;
}
