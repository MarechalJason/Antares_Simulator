// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ExchangeBalance.h"

void ExchangeBalance::add(uint32_t area)
{
    builder.updateHourWithinWeek(0);

    int interconnection = data.IndexDebutIntercoOrigine[area];
    while (interconnection >= 0)
    {
        builder.ntcDirect(interconnection, 1.0);
        interconnection = data.IndexSuivantIntercoOrigine[interconnection];
    }
    interconnection = data.IndexDebutIntercoExtremite[area];
    while (interconnection >= 0)
    {
        builder.ntcDirect(interconnection, -1.0);

        interconnection = data.IndexSuivantIntercoExtremite[interconnection];
    }

    data.NumeroDeContrainteDeSoldeDEchange[area] = builder.data.nombreDeContraintes;
    builder.equalTo();
    builder.build();
}
