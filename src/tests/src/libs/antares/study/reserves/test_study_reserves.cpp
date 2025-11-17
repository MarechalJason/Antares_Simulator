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
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <antares/logs/logs.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include "antares/study/study.h"
using Antares::UnitTests::CaptureAntaresLogs;
using namespace Antares::Data;

void addThermalCluster(Data::Area* area, const std::string& name)
{
    auto c = std::make_shared<ThermalCluster>(area);
    c->setName(name);
    area->thermal.list.addToCompleteList(c);
}

void addShortTermStorage(Data::Area* area, const std::string& name)
{
    ShortTermStorage::STStorageCluster cluster;
    cluster.properties.name = name;
    cluster.id = name;
    area->shortTermStorage.storagesByIndex.push_back(cluster);
}

/*!
 * Study with one area named "A"
 */
class OneProblemWithReservesOneArea
{
public:
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

        tmpCapacityReservationUpTwo.unsuppliedCost = 11;
        tmpCapacityReservationUpTwo.referenceActivationHours = 12;
        tmpCapacityReservationUpTwo.powerActivationRatio = 13;
        tmpCapacityReservationUpTwo.energyActivationRatio = 14;

        tmpCapacityReservationUpThree.unsuppliedCost = 21;
        tmpCapacityReservationUpThree.referenceActivationHours = 22;
        tmpCapacityReservationUpThree.powerActivationRatio = 23;
        tmpCapacityReservationUpThree.energyActivationRatio = 24;

        tmpCapacityReservationDown.unsuppliedCost = 5;
        tmpCapacityReservationDown.referenceActivationHours = 6;
        tmpCapacityReservationDown.powerActivationRatio = 7;
        tmpCapacityReservationDown.energyActivationRatio = 8;

        tmpCapacityReservationDownTwo.unsuppliedCost = 15;
        tmpCapacityReservationDownTwo.referenceActivationHours = 16;
        tmpCapacityReservationDownTwo.powerActivationRatio = 17;
        tmpCapacityReservationDownTwo.energyActivationRatio = 18;

        areaA->allCapacityReservations = AllCapacityReservations();
        areaA->allCapacityReservations.value()
          .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUp);
        areaA->allCapacityReservations.value()
          .areaCapacityReservationsUp.emplace("ReserveUpTwo", tmpCapacityReservationUpTwo);
        areaA->allCapacityReservations.value()
          .areaCapacityReservationsUp.emplace("ReserveUpThree", tmpCapacityReservationUpThree);

        areaA->allCapacityReservations.value()
          .areaCapacityReservationsDown.emplace("ReserveDown", tmpCapacityReservationDown);
        areaA->allCapacityReservations.value()
          .areaCapacityReservationsDown.emplace("ReserveDownTwo", tmpCapacityReservationDownTwo);
    }

    std::unique_ptr<Study> study;
    Area* areaA;
    PROBLEME_HEBDO problem;
    CapacityReservation tmpCapacityReservationUp;
    CapacityReservation tmpCapacityReservationUpTwo;
    CapacityReservation tmpCapacityReservationUpThree;
    CapacityReservation tmpCapacityReservationDown;
    CapacityReservation tmpCapacityReservationDownTwo;
};

class OneProblemWithReservesOneArea_withlogger: public OneProblemWithReservesOneArea,
                                                public CaptureAntaresLogs
{
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
        areaA->allCapacityReservations.value()
          .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUp);
        areaA->allCapacityReservations.value()
          .areaCapacityReservationsDown.emplace("ReserveDown", tmpCapacityReservationDown);

        areaB->allCapacityReservations = AllCapacityReservations();
        areaB->allCapacityReservations.value()
          .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUpB);
        areaB->allCapacityReservations.value()
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
    areaA->allCapacityReservations.value()
      .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUp);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().contains("ReserveUp"), true);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .unsuppliedCost,
                      0);
}

BOOST_AUTO_TEST_CASE(reserve_add_double)
{
    auto study = std::make_unique<Study>();
    const auto areaA = study->areaAdd("A");
    CapacityReservation tmpCapacityReservationUp;

    CapacityReservation tmpCapacityReservationUpTwo;
    areaA->allCapacityReservations = AllCapacityReservations();
    areaA->allCapacityReservations.value()
      .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUp);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().contains("ReserveUp"), true);
    areaA->allCapacityReservations.value()
      .areaCapacityReservationsUp.emplace("ReserveUp", tmpCapacityReservationUpTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 1);
    areaA->allCapacityReservations.value()
      .areaCapacityReservationsUp.emplace("ReserveUpTwo", tmpCapacityReservationUpTwo);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 2);
}

BOOST_FIXTURE_TEST_CASE(reserve_one_area, OneProblemWithReservesOneArea)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().areaCapacityReservationsDown.size(),
                      2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().areaCapacityReservationsUp.size(), 3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .unsuppliedCost,
                      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .referenceActivationHours,
                      2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .powerActivationRatio,
                      3);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .energyActivationRatio,
                      4);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .unsuppliedCost,
                      5);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .referenceActivationHours,
                      6);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .powerActivationRatio,
                      7);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .energyActivationRatio,
                      8);
}

BOOST_FIXTURE_TEST_CASE(reserve_up_two_areas, OneProblemWithReservesTwoAreas)
{
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().size(), 2);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().areaCapacityReservationsDown.size(),
                      1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value().areaCapacityReservationsUp.size(), 1);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value().size(), 2);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value().areaCapacityReservationsDown.size(),
                      1);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value().areaCapacityReservationsUp.size(), 1);
    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .unsuppliedCost,
                      1);

    BOOST_CHECK_EQUAL(areaA->allCapacityReservations.value()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .unsuppliedCost,
                      5);

    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value()
                        .areaCapacityReservationsUp.at("ReserveUp")
                        .unsuppliedCost,
                      11);
    BOOST_CHECK_EQUAL(areaB->allCapacityReservations.value()
                        .areaCapacityReservationsDown.at("ReserveDown")
                        .unsuppliedCost,
                      15);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_One_No_Symmetries,
                        OneProblemWithReservesOneArea)
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
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveCost("ReserveUp"),
                      8.8);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveMaxPowerOff("ReserveUp"),
                      7.7);
    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .reserveCostOff("ReserveUp"),
                      6.6);
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");
    addThermalCluster(areaA, "cluster2");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "cluster1 = [ReserveUpThree, ReserveDown]\n";
    file << "cluster1 = [ReserveUpTwo, ReserveDownTwo]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveUpThree]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      areaA->thermal.list.findInAll("cluster1")->reserveParticipationContainer.has_value());
    BOOST_CHECK(
      !areaA->thermal.list.findInAll("cluster2")->reserveParticipationContainer.has_value());
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .isParticipatingInReserve("ReserveUp"));
    BOOST_CHECK(areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .isParticipatingInReserve("ReserveDown"));

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .getNbSymGroups(),
                      3);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveUp")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveDown")
                        .size(),
                      2);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveUpThree")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveUpTwo")
                        .size(),
                      1);

    BOOST_CHECK_EQUAL(areaA->thermal.list.findInAll("cluster1")
                        ->reserveParticipationContainer.value()
                        .symmetricalIndices("ReserveDownTwo")
                        .size(),
                      1);

    std::vector<int> symDown = areaA->thermal.list.findInAll("cluster1")
                                 ->reserveParticipationContainer.value()
                                 .symmetricalIndices("ReserveDown");

    int symUp = areaA->thermal.list.findInAll("cluster1")
                  ->reserveParticipationContainer.value()
                  .symmetricalIndices("ReserveUp")
                  .at(0);
    int symUpTwo = areaA->thermal.list.findInAll("cluster1")
                     ->reserveParticipationContainer.value()
                     .symmetricalIndices("ReserveUpTwo")
                     .at(0);
    int symUpThree = areaA->thermal.list.findInAll("cluster1")
                       ->reserveParticipationContainer.value()
                       .symmetricalIndices("ReserveUpThree")
                       .at(0);
    BOOST_CHECK_EQUAL(
      count(symDown.begin(), symDown.end(), symUp),
      1); // Value in symmetricalIndices("ReserveUp") is also in symmetricalIndices("ReserveDown")
    BOOST_CHECK_EQUAL(
      count(symDown.begin(), symDown.end(), symUpTwo),
      0); // Value in symmetricalIndices("ReserveUpTwo") is not in symmetricalIndices("ReserveDown")
    BOOST_CHECK_EQUAL(count(symDown.begin(), symDown.end(), symUpThree),
                      1);             // Value in symmetricalIndices("ReserveUpThree") is also in
                                      // symmetricalIndices("ReserveDown")
    BOOST_CHECK(symUp != symUpThree); // Value in symmetricalIndices("ReserveUp") is not in
                                      // symmetricalIndices("ReserveUpThree")
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Cluster_Symmetry,
                        OneProblemWithReservesOneArea_withlogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster3 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    BOOST_CHECK(
      getWarnings().contains("Thermal cluster cluster3 not participating to reserves of A"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Reserve_Symmetry,
                        OneProblemWithReservesOneArea_withlogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveNull, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("This entity is not participating to reserve ReserveNull"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Bad_Cluster_Participation,
                        OneProblemWithReservesOneArea_withlogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 9.9\n";
    file << "participation-cost = 8.8\n";
    file << "max-power-off = 7.7\n";
    file << "participation-cost-off = 6.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster3\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("This entity is not participating to reserve ReserveDown"));
}

BOOST_FIXTURE_TEST_CASE(test_thermal_loadReserveParticipations_Cluster_Participation_No_Init,
                        OneProblemWithReservesOneArea_withlogger)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addThermalCluster(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->thermal.list.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("Area A, cluster1 : trying to add symmetries without any reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "cluster1 = [ReserveUpThree, ReserveDown]\n";
    file << "cluster1 = [ReserveUpTwo, ReserveDownTwo]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveUpThree]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");

    BOOST_CHECK(areaA->hydro.reserveParticipationContainer.has_value());

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().isParticipatingInReserve(
                        "ReserveUp"),
                      true);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().isParticipatingInReserve(
                        "ReserveDown"),
                      true);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().getNbSymGroups(), 3);

    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().reserveCost("ReserveUp"),
                      9.9);
    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().reserveMaxStore(
                        "ReserveUp"),
                      8.8);
    BOOST_CHECK_EQUAL(areaA->hydro.reserveParticipationContainer.value().reserveMaxRelease(
                        "ReserveUp"),
                      7.7);
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_no_reserve,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->hydro.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("Area A, hydro : trying to add symmetries without any reserves"));
}

BOOST_FIXTURE_TEST_CASE(test_hydro_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "[ReserveUp]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file.close();
    BOOST_CHECK_EXCEPTION(areaA->hydro.loadReserveParticipations(*areaA,
                                                                 studyPath / "myreserve.ini"),
                          std::out_of_range,
                          checkMessage("This entity is not participating to reserve ReserveDown"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_Symmetries,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    addShortTermStorage(areaA, "cluster1");
    std::ofstream file(studyPath / "myreserve.ini");

    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "cluster1 = [ReserveUpThree, ReserveDown]\n";
    file << "cluster1 = [ReserveUpTwo, ReserveDownTwo]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file << "\n";
    file << "[ReserveUpTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveUpThree]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 19.9\n";
    file << "participation-cost = 18.8\n";
    file << "max-power-off = 17.7\n";
    file << "participation-cost-off = 16.6\n";
    file << "\n";
    file << "[ReserveDown]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();

    areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini");
    auto* resContainer = &areaA->shortTermStorage.findInAll("cluster1")
                            ->reserveParticipationContainer;

    BOOST_CHECK(resContainer->has_value());
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("ReserveUp"), true);
    BOOST_CHECK_EQUAL(resContainer->value().isParticipatingInReserve("ReserveDown"), true);
    BOOST_CHECK_EQUAL(resContainer->value().getNbSymGroups(), 3);
    BOOST_CHECK_EQUAL(resContainer->value().reserveCost("ReserveUp"), 9.9);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxStore("ReserveUp"), 8.8);
    BOOST_CHECK_EQUAL(resContainer->value().reserveMaxRelease("ReserveUp"), 7.7);
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_cluster,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveNull]\n";
    file << "participation-cost = 9.9\n";
    file << "max-store = 8.8\n";
    file << "max-release = 7.7\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage(
        "Area A, cluster1 : trying to add symmetries to a non existing cluster or participation"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_no_reserves,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage(
        "Area A, cluster1 : trying to add symmetries to a non existing cluster or participation"));
}

BOOST_FIXTURE_TEST_CASE(test_STS_loadReserveParticipations_bad_reserve,
                        OneProblemWithReservesOneArea)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    addShortTermStorage(areaA, "cluster1");

    std::ofstream file(studyPath / "myreserve.ini");
    file << "[symmetries]\n";
    file << "cluster1 = [ReserveUp, ReserveDown]\n";
    file << "\n";
    file << "[ReserveUp]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file << "\n";
    file << "[ReserveDownTwo]\n";
    file << "cluster-name = cluster1\n";
    file << "max-power = 1.1\n";
    file << "participation-cost = 2.2\n";
    file << "max-power-off = 3.3\n";
    file << "participation-cost-off = 4.4\n";
    file.close();
    BOOST_CHECK_EXCEPTION(
      areaA->shortTermStorage.loadReserveParticipations(*areaA, studyPath / "myreserve.ini"),
      std::out_of_range,
      checkMessage("This entity is not participating to reserve ReserveDown"));
}

BOOST_AUTO_TEST_SUITE_END() // version
