// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/BindingConstraintHour.h"

void BindingConstraintHour::add(int timeStep, int bindingConstraintIndex)
{
    data.CorrespondanceCntNativesCntOptim[timeStep]
      .NumeroDeContrainteDesContraintesCouplantes[bindingConstraintIndex]
      = builder.data.nombreDeContraintes;

    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[bindingConstraintIndex];

    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
    {
        return;
    }

    builder.updateHourWithinWeek(timeStep);
    // Links
    const int nbInterco = MatriceDesContraintesCouplantes
                            .NombreDInterconnexionsDansLaContrainteCouplante;
    for (int index = 0; index < nbInterco; index++)
    {
        const int interconnection = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
        const double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
        const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
        builder.updateHourWithinWeek(timeStep).ntcDirect(interconnection,
                                                    poids,
                                                    offset,
                                                    builder.data.NombreDePasDeTemps);
    }

    // Thermal clusters
    const int nbClusters = MatriceDesContraintesCouplantes
                             .NombreDePaliersDispatchDansLaContrainteCouplante;
    for (int index = 0; index < nbClusters; index++)
    {
        const int area = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[area];
        const int palier = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                             [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
        const double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
        const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

        builder.updateHourWithinWeek(timeStep).dispatchableProduction(palier,
                                                                 poids,
                                                                 offset,
                                                                 builder.data.NombreDePasDeTemps);
    }

    builder.setOperator(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
    {
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + timeStep);
        namer.BindingConstraintHour(builder.data.nombreDeContraintes,
                                    MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    }
    builder.build();
}
