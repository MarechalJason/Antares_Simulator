// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'PMaxDispatchableGeneration' Constraint type
 */
class PMaxDispatchableGeneration final: private ConstraintFactory
{
public:
    PMaxDispatchableGeneration(ConstraintBuilder& builder, StartUpCostsData& data):
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
    StartUpCostsData& data;
};
