// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <spx_constantes_externes.h>

#include "antares/solver/optimisation/opt_rename_problem.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "variables/VariableManagerUtils.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int nombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;
    int nombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);
    const bool intVariables = problemeHebdo->OptimisationAvecVariablesEntieres;
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        variableNamer.updateArea(problemeHebdo->NomsDesPays[pays]);
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = problemeHebdo
                                                              ->PaliersThermiquesDuPays[pays];

        for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
        {
            const int palier = PaliersThermiquesDuPays
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
            const auto& clusterName = PaliersThermiquesDuPays.NomsDesPaliersThermiques[index];
            auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                variableNamer.updateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
                if (Simulation)
                {
                    nombreDeVariables += 4;
                    continue;
                }

                variableManager.numberOfDispatchableUnits(palier, pdt) = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.nodu(nombreDeVariables, clusterName);
                nombreDeVariables++;

                variableManager.numberStartingDispatchableUnits(palier, pdt) = nombreDeVariables;

                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.numberStartingDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;

                variableManager.numberStoppingDispatchableUnits(palier, pdt) = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_INFERIEUREMENT;
                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.numberStoppingDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;

                variableManager.numberBreakingDownDispatchableUnits(palier, pdt)
                  = nombreDeVariables;
                ProblemeAResoudre->TypeDeVariable[nombreDeVariables]
                  = VARIABLE_BORNEE_DES_DEUX_COTES;

                ProblemeAResoudre->VariablesEntieres[nombreDeVariables] = intVariables;
                variableNamer.numberBreakingDownDispatchableUnits(nombreDeVariables, clusterName);
                nombreDeVariables++;
            }
        }
    }
    ProblemeAResoudre->NombreDeVariables = nombreDeVariables;
}
