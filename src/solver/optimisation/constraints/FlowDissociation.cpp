// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FlowDissociation.h"

void FlowDissociation::add(int timeStep, int interconnection)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = data.CoutDeTransport[interconnection];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        data.CorrespondanceCntNativesCntOptim[timeStep].NumeroDeContrainteDeDissociationDeFlux[interconnection]
          = builder.data.nombreDeContraintes;
        const auto origin = builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[interconnection]];
        const auto destination = builder.data
                                   .NomsDesPays[data.PaysExtremiteDeLInterconnexion[interconnection]];
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.updateExtremities(origin, destination);
        namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
        namer.flowDissociation(builder.data.nombreDeContraintes);

        builder.updateHourWithinWeek(timeStep);
        builder.ntcDirect(interconnection, 1.0)
          .interconnectionDirectCost(interconnection, -1.0)
          .interconnectionIndirectCost(interconnection, 1.0);

        builder.equalTo();

        builder.build();
    }
}
