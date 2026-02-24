// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ExchangeBalanceGroup.h"

ExchangeBalanceData ExchangeBalanceGroup::GetExchangeBalanceDataFromProblemHebdo()
{
    return {.IndexDebutIntercoOrigine = problemeHebdo_->IndexDebutIntercoOrigine,
            .IndexSuivantIntercoOrigine = problemeHebdo_->IndexSuivantIntercoOrigine,
            .IndexDebutIntercoExtremite = problemeHebdo_->IndexDebutIntercoExtremite,
            .IndexSuivantIntercoExtremite = problemeHebdo_->IndexSuivantIntercoExtremite,
            .NumeroDeContrainteDeSoldeDEchange = problemeHebdo_->NumeroDeContrainteDeSoldeDEchange};
}

void ExchangeBalanceGroup::BuildConstraints()
{
    auto data = GetExchangeBalanceDataFromProblemHebdo();
    ExchangeBalance exchangeBalance(builder_, data);
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays - 1; area++)
    {
        exchangeBalance.add(area);
    }
}
