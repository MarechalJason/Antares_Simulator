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
#include <files-system.h>
#include <filesystem>

#include <boost/test/unit_test.hpp>

#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include "antares/study/study.h"

using namespace Antares::Data;

void addThermalCluster(Data::Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

/*!
 * Study with one area named "A"
 */
struct OneProblemWithReservesOneArea
{
    OneProblemWithReservesOneArea()
    {
        study = std::make_unique<Study>();
        areaA = study->areaAdd("A");
        CAPACITY_RESERVATION areaCapacityReservationsUp;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        tmpCapacityReservationUp.unsuppliedCost = 1;
        tmpCapacityReservationUp.referenceActivationHours = 2;
        tmpCapacityReservationUp.powerActivationRatio = 3;
        tmpCapacityReservationUp.energyActivationRatio = 4;

        tmpCapacityReservationDown.unsuppliedCost = 5;
        tmpCapacityReservationDown.referenceActivationHours = 6;
        tmpCapacityReservationDown.powerActivationRatio = 7;
        tmpCapacityReservationDown.energyActivationRatio = 8;

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

/*!
 * Study with two areas named "A" and "B"
 */
struct OneProblemWithReservesTwoAreas
{
    OneProblemWithReservesTwoAreas()
    {
        study = std::make_unique<Study>();
        areaA = study->areaAdd("A");
        areaB = study->areaAdd("B");
        CAPACITY_RESERVATION areaCapacityReservationsUp;
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;

        tmpCapacityReservationUp.unsuppliedCost = 1;
        tmpCapacityReservationUp.referenceActivationHours = 2;
        tmpCapacityReservationUp.powerActivationRatio = 3;
        tmpCapacityReservationUp.energyActivationRatio = 4;

        tmpCapacityReservationDown.unsuppliedCost = 5;
        tmpCapacityReservationDown.referenceActivationHours = 6;
        tmpCapacityReservationDown.powerActivationRatio = 7;
        tmpCapacityReservationDown.energyActivationRatio = 8;

        tmpCapacityReservationUpB.unsuppliedCost = 11;
        tmpCapacityReservationUpB.referenceActivationHours = 12;
        tmpCapacityReservationUpB.powerActivationRatio = 13;
        tmpCapacityReservationUpB.energyActivationRatio = 14;

        tmpCapacityReservationDownB.unsuppliedCost = 15;
        tmpCapacityReservationDownB.referenceActivationHours = 16;
        tmpCapacityReservationDownB.powerActivationRatio = 17;
        tmpCapacityReservationDownB.energyActivationRatio = 18;

        areaA->allCapacityReservations = AllCapacityReservations();
        areaA->allCapacityReservations()
          .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUp);
        areaA->allCapacityReservations()
          .areaCapacityReservationsDown.emplace("ReserveDown", tmpCapacityReservationDown);

        areaB->allCapacityReservations = AllCapacityReservations();
        areaB->allCapacityReservations()
          .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUpB);
        areaB->allCapacityReservations()
          .areaCapacityReservationsDown.emplace("ReserveDown", tmpCapacityReservationDownB);
    }

    std::unique_ptr<Study> study;
    Area* areaA;
    Area* areaB;
    PROBLEME_HEBDO problem;
    CapacityReservation tmpCapacityReservationUp;
    CapacityReservation tmpCapacityReservationDown;

    CapacityReservation tmpCapacityReservationUpB;
    CapacityReservation tmpCapacityReservationDownB;
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

BOOST_AUTO_TEST_CASE(reserve_add_double)
{
    auto study = std::make_unique<Study>();
    const auto areaA = study->areaAdd("A");
    CapacityReservation tmpCapacityReservationUp;

    CapacityReservation tmpCapacityReservationUpTwo;
    areaA->allCapacityReservations = AllCapacityReservations();
    areaA->allCapacityReservations().areaCapacityReservationsUp.emplace("ReserveUp",
                                                                        tmpCapacityReservationUp);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().contains("ReserveUp"), true);
    areaA->allCapacityReservations()
      .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUpTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().size(), 1);
    areaA->allCapacityReservations()
      .areaCapacityReservationsUp.emplace("ReserveUpTwo", tmpCapacityReservationUpTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().size(), 2);
}

BOOST_FIXTURE_TEST_CASE(reserve_one_area, OneProblemWithReservesOneArea)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().size(), 2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().areaCapacityReservationsDown.size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().areaCapacityReservationsUp.size(), 1);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations().areaCapacityReservationsUp.at("ReserveUp").unsuppliedCost,
      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .referenceActivationHours,
                      2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .powerActivationRatio,
                      3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .energyActivationRatio,
                      4);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .unsuppliedCost,
                      5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .referenceActivationHours,
                      6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .powerActivationRatio,
                      7);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .energyActivationRatio,
                      8);
}

BOOST_FIXTURE_TEST_CASE(reserve_up_two_areas, OneProblemWithReservesTwoAreas)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().size(), 2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().areaCapacityReservationsDown.size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations().areaCapacityReservationsUp.size(), 1);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations().size(), 2);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations().areaCapacityReservationsDown.size(), 1);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations().areaCapacityReservationsUp.size(), 1);
    BOOST_CHECK_EQUAL(
      areaA->allCapacityReservations().areaCapacityReservationsUp.at("ReserveUp").unsuppliedCost,
      1);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .unsuppliedCost,
                      5);

    BOOST_CHECK_EQUAL(
      areaB->allCapacityReservations().areaCapacityReservationsUp.at("ReserveUp").unsuppliedCost,
      11);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .unsuppliedCost,
                      15);
}

BOOST_FIXTURE_TEST_CASE(test_loadReserveParticipations_Normal, OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .isParticipatingInReserve("ReserveUp"));
    BOOST_CHECK(!areaA->thermal.list.findInAll("cluster1")
                   ->reserveParticipationContainer.value()
                   .isParticipatingInReserve("ReserveDown"));
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveMaxPower("ReserveUp"),
                      9.9);
}

BOOST_AUTO_TEST_SUITE_END() // version
