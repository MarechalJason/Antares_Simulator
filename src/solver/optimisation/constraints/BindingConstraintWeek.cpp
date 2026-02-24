// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/BindingConstraintWeek.h"

void BindingConstraintWeek::add(int bindingConstraintIndex)
{
    int semaine = builder.data.weekInTheYear;

    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[bindingConstraintIndex];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HEBDOMADAIRE)
    {
        return;
    }

    const int nbInterco = MatriceDesContraintesCouplantes
                            .NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = MatriceDesContraintesCouplantes
                             .NombreDePaliersDispatchDansLaContrainteCouplante;

    for (int index = 0; index < nbInterco; index++)
    {
        int interconnection = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
        double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
        int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
        for (int timeStep = 0; timeStep < builder.data.NombreDePasDeTempsPourUneOptimisation; timeStep++)
        {
            builder.updateHourWithinWeek(timeStep).ntcDirect(interconnection,
                                                        poids,
                                                        offset,
                                                        builder.data.NombreDePasDeTemps);
        }
    }

    for (int index = 0; index < nbClusters; index++)
    {
        int area = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[area];
        const int palier = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                             [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
        double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
        int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];
        for (int timeStep = 0; timeStep < builder.data.NombreDePasDeTempsPourUneOptimisation; timeStep++)
        {
            builder.updateHourWithinWeek(timeStep)
              .dispatchableProduction(palier, poids, offset, builder.data.NombreDePasDeTemps);
        }
    }

    builder.setOperator(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);

    data.NumeroDeContrainteDesContraintesCouplantes[bindingConstraintIndex] = builder.data
                                                                      .nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateTimeStep(semaine);
    namer.BindingConstraintWeek(builder.data.nombreDeContraintes,
                                MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    builder.build();
}
