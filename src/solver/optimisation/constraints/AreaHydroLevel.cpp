// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/AreaHydroLevel.h"

void AreaHydroLevel::add(int area, int timeStep)
{
    data.CorrespondanceCntNativesCntOptim[timeStep].NumeroDeContrainteDesNiveauxPays[area]
      = builder.data.nombreDeContraintes;
    if (data.CaracteristiquesHydrauliques[area].SuiviNiveauHoraire)
    {
        builder.updateHourWithinWeek(timeStep).hydroLevel(area, 1.0);
        if (timeStep > 0)
        {
            builder.updateHourWithinWeek(timeStep - 1).hydroLevel(area, -1.0);
        }
        ConstraintNamer namer(builder.data.NomDesContraintes);

        namer.UpdateArea(builder.data.NomsDesPays[area]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
        namer.AreaHydroLevel(builder.data.nombreDeContraintes);
        data.CorrespondanceCntNativesCntOptim[timeStep].NumeroDeContrainteDesNiveauxPays[area]
          = builder.data.nombreDeContraintes;
        builder.updateHourWithinWeek(timeStep)
          .hydroPower(area, 1.0)
          .pumping(area, -data.CaracteristiquesHydrauliques[area].PumpingRatio)
          .overflow(area, 1.)
          .equalTo()
          .build();
    }
    else
    {
        data.CorrespondanceCntNativesCntOptim[timeStep].NumeroDeContrainteDesNiveauxPays[area] = -1;
    }
}
