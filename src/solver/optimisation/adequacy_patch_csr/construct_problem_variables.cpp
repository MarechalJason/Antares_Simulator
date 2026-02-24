// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "pi_constantes_externes.h"

void HourlyCSRProblem::constructVariableENS()
{
    int& numberOfVariables = problemeAResoudre_.NombreDeVariables;
    numberOfVariables = 0;

    // variables: ENS of each area inside adq patch
    logs.debug() << " ENS of each area inside adq patch: ";
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        // Only ENS for areas inside adq patch are considered as variables
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            variableManager_.unsuppliedEnergy(area, triggeredHour) = numberOfVariables;
            problemeAResoudre_.TypeDeVariable[numberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            varToBeSetToZeroIfBelowThreshold.insert(numberOfVariables);
            ensVariablesInsideAdqPatch.insert(numberOfVariables);
            logs.debug() << numberOfVariables << " ENS[" << area << "].-["
                         << problemeHebdo_->NomsDesPays[area] << "].";

            numberOfVariables++;
        }
    }
}

void HourlyCSRProblem::constructVariableSpilledEnergy()
{
    int& numberOfVariables = problemeAResoudre_.NombreDeVariables;

    // variables: Spilled Energy  of each area inside adq patch
    logs.debug() << " Spilled Energy  of each area inside adq patch: ";
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        // Only Spilled Energy  for areas inside adq patch are considered as variables
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            variableManager_.spillage(area, triggeredHour) = numberOfVariables;
            problemeAResoudre_.TypeDeVariable[numberOfVariables] = VARIABLE_BORNEE_INFERIEUREMENT;
            varToBeSetToZeroIfBelowThreshold.insert(numberOfVariables);
            logs.debug() << numberOfVariables << " Spilled Energy[" << area << "].-["
                         << problemeHebdo_->NomsDesPays[area] << "].";

            numberOfVariables++;
        }
    }
}

void HourlyCSRProblem::constructVariableFlows()
{
    int& numberOfVariables = problemeAResoudre_.NombreDeVariables;

    // variables: transmissin flows (flow, direct_direct and flow_indirect). For links between 2
    // and 2.
    logs.debug()
      << " transmissin flows (flow, flow_direct and flow_indirect). For links between 2 and 2:";
    for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; Interco++)
    {
        // only consider link between 2 and 2
        if (problemeHebdo_->adequacyPatchRuntimeData->originAreaMode[Interco]
              == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            int algebraicFluxVar;
            int directVar;
            int indirectVar;
            algebraicFluxVar = variableManager_.ntcDirect(Interco, triggeredHour)
              = numberOfVariables;
            problemeAResoudre_.TypeDeVariable[numberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug()
              << numberOfVariables << " flow[" << Interco << "]. ["
              << problemeHebdo_
                   ->NomsDesPays[problemeHebdo_->PaysExtremiteDeLInterconnexion[Interco]]
              << "]-["
              << problemeHebdo_->NomsDesPays[problemeHebdo_->PaysOrigineDeLInterconnexion[Interco]]
              << "].";
            numberOfVariables++;

            directVar = variableManager_.interconnectionDirectCost(Interco, triggeredHour)
              = numberOfVariables;
            problemeAResoudre_.TypeDeVariable[numberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << numberOfVariables << " direct flow[" << Interco << "]. ";
            numberOfVariables++;

            indirectVar = variableManager_.interconnectionIndirectCost(Interco, triggeredHour)
              = numberOfVariables;
            problemeAResoudre_.TypeDeVariable[numberOfVariables] = VARIABLE_BORNEE_DES_DEUX_COTES;
            logs.debug() << numberOfVariables << " indirect flow[" << Interco << "]. ";
            numberOfVariables++;

            linkInsideAdqPatch[algebraicFluxVar] = LinkVariable(directVar, indirectVar);
        }
    }
}
