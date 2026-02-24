// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/AreaBalance.h"

static void shortTermStorageBalance(const ::AREA_INPUT& shortTermStorageInput,
                                    ConstraintBuilder& constraintBuilder)
{
    for (const auto& storage: shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        constraintBuilder.shortTermStorageInjection(index, 1.0)
          .shortTermStorageWithdrawal(index, -1.0);
    }
}

void AreaBalance::add(int timeStep, int area)
{
    data.CorrespondanceCntNativesCntOptim[timeStep].NumeroDeContrainteDesBilansPays[area]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
    namer.UpdateArea(builder.data.NomsDesPays[area]);
    namer.areaBalance(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(timeStep);

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

    ExportPaliers(data.PaliersThermiquesDuPays[area], builder);
    builder.hydroPower(area, -1.0).pumping(area, 1.0).unsuppliedEnergy(area, -1.0).spillage(area, 1.0);

    shortTermStorageBalance(data.ShortTermStorage[area], builder);

    builder.equalTo();
    builder.build();
}
