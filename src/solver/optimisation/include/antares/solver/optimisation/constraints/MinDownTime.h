// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct MinDownTimeData
{
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    bool Simulation;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
};

/*!
 * represent 'minDownTime' Constraint type
 */
class MinDownTime final: private ConstraintFactory
{
public:
    MinDownTime(ConstraintBuilder& builder, MinDownTimeData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param area : area
     * @param cluster : global index of the cluster
     * @param timeStep : timestep
     * @param Simulation : ---
     */
    void add(int area, int index, int timeStep);

private:
    MinDownTimeData& data;
};
