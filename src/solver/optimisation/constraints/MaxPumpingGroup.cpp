// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MaxPumpingGroup.h"

MaxPumpingData MaxPumpingGroup::GetMaxPumpingData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NumeroDeContrainteMaxPompage = problemeHebdo_->NumeroDeContrainteMaxPompage};
}

void MaxPumpingGroup::BuildConstraints()
{
    auto maxPumpingData = GetMaxPumpingData();
    maxPumping MaxPumping(builder_, maxPumpingData);

    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; area++)
    {
        maxPumping.add(area);
    }
}
