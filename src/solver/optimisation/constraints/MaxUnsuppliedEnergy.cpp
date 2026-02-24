#include "antares/solver/optimisation/constraints/MaxUnsuppliedEnergy.h"

void MaxUnsuppliedEnergy::add(unsigned timeStep, int area)
{
    data.CorrespondanceCntNativesCntOptim[timeStep].NumeroDeContraintePourBornerLaDefaillance[area]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);

    namer.updateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
    namer.updateArea(builder.data.NomsDesPays[area]);
    namer.maxUnsuppliedEnergy(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(timeStep);
    builder.unsuppliedEnergy(area, 1.0);

    builder.lessThan();
    builder.build();
}
