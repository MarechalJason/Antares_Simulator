/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define BOOST_TEST_MODULE study
#define WIN32_LEAN_AND_MEAN
#include <boost/test/unit_test.hpp>

#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include "antares/study/study.h"

using namespace Antares::Data;

/*!
 * Study with one area named "A"
 */
struct OneProblemWithReserves
{
    OneProblemWithReserves()
    {
        study = std::make_unique<Study>();
        areaA = study->areaAdd("A");
        CAPACITY_RESERVATION areaCapacityReservationsUp;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        tmpCapacityReservationUp.unsuppliedCost = 1;
        tmpCapacityReservationUp.maxActivationHours = 2;
        tmpCapacityReservationUp.maxActivationRatio = 3;
        tmpCapacityReservationUp.maxEnergyActivationRatio = 4;

        tmpCapacityReservationDown.unsuppliedCost = 5;
        tmpCapacityReservationDown.maxActivationHours = 6;
        tmpCapacityReservationDown.maxActivationRatio = 7;
        tmpCapacityReservationDown.maxEnergyActivationRatio = 8;

        areaA->allCapacityReservations = AllCapacityReservations();
        areaA->allCapacityReservations()
          .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUp);
        areaA->allCapacityReservations()
          .areaCapacityReservationsDown.emplace("ReserveDown", tmpCapacityReservationDown);
    }

    std::unique_ptr<Study> study;
    Area* areaA;
    PROBLEME_HEBDO problem;
    CapacityReservation tmpCapacityReservationUp;
    CapacityReservation tmpCapacityReservationDown;
};

BOOST_AUTO_TEST_SUITE(reserves_operations)

BOOST_AUTO_TEST_CASE(reserve_add)
{
    auto study = std::make_unique<Study>();
    const auto areaA = study->areaAdd("A");
    CapacityReservation tmpCapacityReservationUp;
    tmpCapacityReservationUp.unsuppliedCost = 0;
    areaA->allCapacityReservations = AllCapacityReservations();
    areaA->allCapacityReservations().areaCapacityReservationsUp.emplace("ReserveUp",
                                                                        tmpCapacityReservationUp);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().contains("ReserveUp"), true);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations().areaCapacityReservationsUp.at("ReserveUp").unsuppliedCost,
      0);
}

BOOST_FIXTURE_TEST_CASE(reserve_up, OneProblemWithReserves)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().size(), 2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().areaCapacityReservationsDown.size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().areaCapacityReservationsUp.size(), 1);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations().areaCapacityReservationsUp.at("ReserveUp").unsuppliedCost,
      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .maxActivationHours,
                      2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .maxActivationRatio,
                      3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .maxEnergyActivationRatio,
                      4);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .unsuppliedCost,
                      5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .maxActivationHours,
                      6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .maxActivationRatio,
                      7);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .maxEnergyActivationRatio,
                      8);
}

BOOST_AUTO_TEST_SUITE_END() // version
