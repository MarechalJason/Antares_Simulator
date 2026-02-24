// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "ConstraintBuilder.h"

struct BindingConstraintHourData
{
    const std::vector<CONTRAINTES_COUPLANTES>& MatriceDesContraintesCouplantes;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
};

/*!
 * represent 'Hourly Binding Constraint' type
 */
class BindingConstraintHour final: private ConstraintFactory
{
public:
    BindingConstraintHour(ConstraintBuilder& builder, BindingConstraintHourData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param timeStep : timestep
     * @param bindingConstraintIndex : the binding constraint number
     */
    void add(int timeStep, int bindingConstraintIndex);

private:
    BindingConstraintHourData& data;
};
