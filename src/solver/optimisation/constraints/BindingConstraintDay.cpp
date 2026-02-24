// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/BindingConstraintDay.h"

void BindingConstraintDay::add(int bindingConstraintIndex)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[bindingConstraintIndex];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
    {
        return;
    }

    const int nbInterco = MatriceDesContraintesCouplantes
                            .NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = MatriceDesContraintesCouplantes
                             .NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                        .NombreDePasDeTempsPourUneOptimisation;
    const int NombreDePasDeTempsDUneJournee = data.NombreDePasDeTempsDUneJournee;
    int startTimeStep = 0;
    while (startTimeStep < NombreDePasDeTempsPourUneOptimisation)
    {
        int jour = data.NumeroDeJourDuPasDeTemps[startTimeStep];

        for (int index = 0; index < nbInterco; index++)
        {
            int interconnection = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
            double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

            for (int timeStep = startTimeStep; timeStep < startTimeStep + NombreDePasDeTempsDUneJournee; timeStep++)
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
            const int palier = PaliersThermiquesDuPays
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques
                                   [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
            double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

            for (int timeStep = startTimeStep; timeStep < startTimeStep + NombreDePasDeTempsDUneJournee; timeStep++)
            {
                builder.updateHourWithinWeek(timeStep)
                  .dispatchableProduction(palier, poids, offset, builder.data.NombreDePasDeTemps);
            }
        }

        data.CorrespondanceCntNativesCntOptimJournalieres[jour]
          .NumeroDeContrainteDesContraintesCouplantes[bindingConstraintIndex]
          = builder.data.nombreDeContraintes;

        builder.setOperator(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
        {
            const auto dayInTheYear = builder.data.weekInTheYear * 7 + jour;
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.updateTimeStep(dayInTheYear);
            namer.bindingConstraintDay(builder.data.nombreDeContraintes,
                                       MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
        }
        builder.build();
        startTimeStep += data.NombreDePasDeTempsDUneJournee;
    }
}
