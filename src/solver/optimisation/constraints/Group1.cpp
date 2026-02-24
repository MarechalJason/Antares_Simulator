// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/Group1.h"

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"
#include "antares/solver/optimisation/constraints/ShortTermStorageCumulation.h"

AreaBalanceData Group1::GetAreaBalanceData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .IndexDebutIntercoOrigine = problemeHebdo_->IndexDebutIntercoOrigine,
            .IndexSuivantIntercoOrigine = problemeHebdo_->IndexSuivantIntercoOrigine,
            .IndexDebutIntercoExtremite = problemeHebdo_->IndexDebutIntercoExtremite,
            .IndexSuivantIntercoExtremite = problemeHebdo_->IndexSuivantIntercoExtremite,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .ShortTermStorage = problemeHebdo_->ShortTermStorage};
}

FictitiousLoadData Group1::GetFictitiousLoadData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .DefaillanceNegativeUtiliserHydro = problemeHebdo_->DefaillanceNegativeUtiliserHydro};
}

MaxUnsupEnergyData Group1::GetMaxUnsupEnergyData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

ShortTermStorageData Group1::GetShortTermStorageData()
{
    return {
      .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
      .ShortTermStorage = problemeHebdo_->ShortTermStorage,
    };
}

ShortTermStorageCumulativeConstraintData Group1::GetShortTermStorageCumulativeConstraintData()
{
    return {{.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
             .ShortTermStorage = problemeHebdo_->ShortTermStorage},
            problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires};
}

FlowDissociationData Group1::GetFlowDissociationData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .CoutDeTransport = problemeHebdo_->CoutDeTransport,
            .PaysOrigineDeLInterconnexion = problemeHebdo_->PaysOrigineDeLInterconnexion,
            .PaysExtremiteDeLInterconnexion = problemeHebdo_->PaysExtremiteDeLInterconnexion};
}

BindingConstraintHourData Group1::GetBindingConstraintHourData()
{
    return

      {.MatriceDesContraintesCouplantes = problemeHebdo_->MatriceDesContraintesCouplantes,
       .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
       .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays};
}

void Group1::BuildConstraints()
{
    auto areaBalanceData = GetAreaBalanceData();
    AreaBalance areaBalance(builder_, areaBalanceData);

    auto fictitiousLoadData = GetFictitiousLoadData();
    FictitiousLoad fictitiousLoad(builder_, fictitiousLoadData);

    auto maxUnsupEnergyData = GetMaxUnsupEnergyData();
    MaxUnsuppliedEnergy maxUnsuppliedEnergy(builder_, maxUnsupEnergyData);

    auto shortTermStorageData = GetShortTermStorageData();
    ShortTermStorageLevel shortTermStorageLevel(builder_, shortTermStorageData);

    ShortTermStorageCostVariationInjectionBackward shortTermStorageCostVariationInjectionBackward(
      builder_,
      shortTermStorageData);
    ShortTermStorageCostVariationInjectionForward shortTermStorageCostVariationInjectionForward(
      builder_,
      shortTermStorageData);
    ShortTermStorageCostVariationWithdrawalBackward shortTermStorageCostVariationWithdrawalBackward(
      builder_,
      shortTermStorageData);
    ShortTermStorageCostVariationWithdrawalForward shortTermStorageCostVariationWithdrawalForward(
      builder_,
      shortTermStorageData);

    auto shortTermStorageCumulativeConstraintData = GetShortTermStorageCumulativeConstraintData();
    ShortTermStorageCumulation shortTermStorageCumulation(builder_,
                                                          shortTermStorageCumulativeConstraintData);

    auto flowDissociationData = GetFlowDissociationData();
    FlowDissociation flowDissociation(builder_, flowDissociationData);

    auto bindingConstraintHourData = GetBindingConstraintHourData();
    BindingConstraintHour bindingConstraintHour(builder_, bindingConstraintHourData);

    int nombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    for (int timeStep = 0; timeStep < nombreDePasDeTempsPourUneOptimisation; timeStep++)
    {
        for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
        {
            areaBalance.add(timeStep, area);
            fictitiousLoad.add(timeStep, area);
            maxUnsuppliedEnergy.add(timeStep, area);
            shortTermStorageLevel.add(timeStep, area);
            shortTermStorageCostVariationInjectionBackward.add(timeStep, area);
            shortTermStorageCostVariationInjectionForward.add(timeStep, area);
            shortTermStorageCostVariationWithdrawalBackward.add(timeStep, area);
            shortTermStorageCostVariationWithdrawalForward.add(timeStep, area);
        }

        for (uint32_t interconnection = 0; interconnection < problemeHebdo_->NombreDInterconnexions; interconnection++)
        {
            flowDissociation.add(timeStep, interconnection);
        }

        for (uint32_t bindingConstraintIndex = 0;
             bindingConstraintIndex < problemeHebdo_->NombreDeContraintesCouplantes;
             bindingConstraintIndex++)
        {
            bindingConstraintHour.add(timeStep, bindingConstraintIndex);
        }
    }

    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        shortTermStorageCumulation.add(area);
    }
}
