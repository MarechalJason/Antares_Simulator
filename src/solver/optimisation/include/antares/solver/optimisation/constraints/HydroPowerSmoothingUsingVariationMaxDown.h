// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

/*!
 * represent 'Hydraulic Power Smoothing Using Variation Max Down' constraint type
 */

class HydroPowerSmoothingUsingVariationMaxDown final: private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param timeStep : timestep
     * @param area : area
     */
    void add(int area, int timeStep);
};
