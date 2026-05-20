// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test dynamic economy traits"
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/variable/economy/STSbyGroup.h"
#include "antares/solver/variable/economy/dispatchableGeneration.h"
#include "antares/solver/variable/economy/renewableGeneration.h"
#include "antares/solver/variable/state.h"
#include "antares/solver/variable/storage/intermediate.h"

#include "in-memory-study.h"

using namespace Antares::Solver::Variable;
using namespace Antares::Solver::Variable::Economy;
using namespace Antares::Data;
using Antares::Constants::nbHoursInAWeek;

namespace
{

std::unique_ptr<Study> makeStudyWithOneArea(Area*& outArea)
{
    auto study = std::make_unique<Study>();
    study->parameters.reset();

    outArea = study->areaAdd("a");
    outArea->index = 0;
    return study;
}

} // namespace

// =============================================================================
// DispatchableGenerationTraits
// =============================================================================
BOOST_AUTO_TEST_SUITE(dispatchable_generation_traits)

BOOST_AUTO_TEST_CASE(metadata)
{
    BOOST_CHECK_EQUAL(VCardDispatchableGeneration::Caption(), "Dispatch. Gen.");
    BOOST_CHECK_EQUAL(VCardDispatchableGeneration::Unit(), "MWh");
    BOOST_CHECK_EQUAL(static_cast<int>(VCardDispatchableGeneration::decimal), 0);
}

BOOST_AUTO_TEST_CASE(column_descriptors_are_unique_groups_sorted)
{
    Area* area = nullptr;
    auto study = makeStudyWithOneArea(area);

    auto c1 = addClusterToArea(area, "c1");
    auto c2 = addClusterToArea(area, "c2");
    auto c3 = addClusterToArea(area, "c3");
    c1->setGroup("GROUP_B");
    c2->setGroup("GROUP_A");
    c3->setGroup("GROUP_B"); // duplicate group on purpose

    auto descriptors = DispatchableGenerationTraits::buildColumnDescriptors(area);
    BOOST_REQUIRE_EQUAL(descriptors.size(), 2);
    BOOST_CHECK_EQUAL(descriptors[0].caption, "GROUP_A");
    BOOST_CHECK_EQUAL(descriptors[0].unit, "MWh");
    BOOST_CHECK_EQUAL(descriptors[1].caption, "GROUP_B");
    BOOST_CHECK_EQUAL(descriptors[1].unit, "MWh");
}

BOOST_AUTO_TEST_CASE(column_descriptors_empty_when_no_clusters)
{
    Area* area = nullptr;
    auto study = makeStudyWithOneArea(area);

    auto descriptors = DispatchableGenerationTraits::buildColumnDescriptors(area);
    BOOST_CHECK(descriptors.empty());
}

BOOST_AUTO_TEST_CASE(set_hourly_value_accumulates_per_group)
{
    Area* area = nullptr;
    auto study = makeStudyWithOneArea(area);

    auto c1 = addClusterToArea(area, "c1");
    auto c2 = addClusterToArea(area, "c2");
    auto c3 = addClusterToArea(area, "c3");
    c1->setGroup("A");
    c2->setGroup("B");
    c3->setGroup("A");

    auto descriptors = DispatchableGenerationTraits::buildColumnDescriptors(area);
    BOOST_REQUIRE_EQUAL(descriptors.size(), 2); // [A, B]

    State state(*study);
    state.area = area;
    state.year = 0;
    state.hourInTheYear = 42; // "At" hour 42, not 42 hours

    // ThermalState was sized by State's ctor from area.thermal.list.enabledCount() == 3.
    // Indices align with the cluster->enabledIndex set by addToCompleteList.
    auto& production = state.thermal[area->index].thermalClustersProductions;
    BOOST_REQUIRE_EQUAL(production.size(), 3);
    production[c1->enabledIndex] = 10.0;
    production[c2->enabledIndex] = 20.0;
    production[c3->enabledIndex] = 30.0;

    std::vector<IntermediateValues> pValues(descriptors.size());
    DispatchableGenerationTraits::setHourlyValue(pValues, state, 0, descriptors);

    BOOST_CHECK_EQUAL(pValues[0][42], 40.0); // group A: c1 + c3
    BOOST_CHECK_EQUAL(pValues[1][42], 20.0); // group B: c2
    // Other hours untouched
    BOOST_CHECK_EQUAL(pValues[0][0], 0.0);
    BOOST_CHECK_EQUAL(pValues[1][0], 0.0);
}

BOOST_AUTO_TEST_SUITE_END()

// =============================================================================
// RenewableGenerationTraits
// =============================================================================
BOOST_AUTO_TEST_SUITE(renewable_generation_traits)

namespace
{

std::shared_ptr<RenewableCluster> addRenewableCluster(Area* area,
                                                      const std::string& name,
                                                      const std::string& group,
                                                      double tsValue)
{
    auto cluster = std::make_shared<RenewableCluster>(area);
    cluster->setName(name);
    cluster->reset();
    cluster->setGroup(group);
    cluster->tsMode = RenewableCluster::powerGeneration;
    cluster->series.timeSeries.resize(1, HOURS_PER_YEAR);
    cluster->series.timeSeries.fillColumn(0, tsValue);
    area->renewable.list.addToCompleteList(cluster);
    return cluster;
}

} // namespace

BOOST_AUTO_TEST_CASE(metadata)
{
    BOOST_CHECK_EQUAL(VCardRenewableGeneration::Caption(), "Renewable Gen.");
    BOOST_CHECK_EQUAL(VCardRenewableGeneration::Unit(), "MWh");
    BOOST_CHECK_EQUAL(static_cast<int>(VCardRenewableGeneration::decimal), 0);
}

BOOST_AUTO_TEST_CASE(column_descriptors_are_unique_groups_sorted)
{
    Area* area = nullptr;
    auto study = makeStudyWithOneArea(area);

    addRenewableCluster(area, "w1", "WIND", 0.0);
    addRenewableCluster(area, "w2", "WIND", 0.0); // duplicate group
    addRenewableCluster(area, "s1", "SOLAR", 0.0);

    auto descriptors = RenewableGenerationTraits::buildColumnDescriptors(area);
    BOOST_REQUIRE_EQUAL(descriptors.size(), 2);
    BOOST_CHECK_EQUAL(descriptors[0].caption, "SOLAR");
    BOOST_CHECK_EQUAL(descriptors[0].unit, "MWh");
    BOOST_CHECK_EQUAL(descriptors[1].caption, "WIND");
    BOOST_CHECK_EQUAL(descriptors[1].unit, "MWh");
}

BOOST_AUTO_TEST_CASE(set_hourly_value_accumulates_per_group)
{
    Area* area = nullptr;
    auto study = makeStudyWithOneArea(area);

    addRenewableCluster(area, "w1", "WIND", 100.0);
    addRenewableCluster(area, "w2", "WIND", 50.0);
    addRenewableCluster(area, "s1", "SOLAR", 30.0);

    auto descriptors = RenewableGenerationTraits::buildColumnDescriptors(area);
    BOOST_REQUIRE_EQUAL(descriptors.size(), 2); // [SOLAR, WIND]

    State state(*study);
    state.area = area;
    state.year = 0;
    state.hourInTheYear = 7;

    std::vector<IntermediateValues> pValues(descriptors.size());
    RenewableGenerationTraits::setHourlyValue(pValues, state, 0, descriptors);

    BOOST_CHECK_EQUAL(pValues[0][7], 30.0);  // SOLAR: s1
    BOOST_CHECK_EQUAL(pValues[1][7], 150.0); // WIND: w1 + w2
    BOOST_CHECK_EQUAL(pValues[0][0], 0.0);
    BOOST_CHECK_EQUAL(pValues[1][0], 0.0);
}

BOOST_AUTO_TEST_SUITE_END()

// =============================================================================
// STSbyGroupTraits
// =============================================================================
BOOST_AUTO_TEST_SUITE(sts_by_group_traits)

BOOST_AUTO_TEST_CASE(metadata)
{
    BOOST_CHECK_EQUAL(VCardSTSbyGroup::Caption(), "STS by group");
    BOOST_CHECK_EQUAL(VCardSTSbyGroup::Unit(), "MWh");
    BOOST_CHECK_EQUAL(static_cast<int>(VCardSTSbyGroup::decimal), 0);
}

BOOST_AUTO_TEST_CASE(column_descriptors_three_per_group_sorted)
{
    Area* area = nullptr;
    auto study = makeStudyWithOneArea(area);

    addSTSToArea(area, "s1")->properties.groupName = "G1";
    addSTSToArea(area, "s2")->properties.groupName = "G2";
    addSTSToArea(area, "s3")->properties.groupName = "G1"; // duplicate group

    auto descriptors = STSbyGroupTraits::buildColumnDescriptors(area);
    BOOST_REQUIRE_EQUAL(descriptors.size(), 6); // 3 cols * 2 groups

    BOOST_CHECK_EQUAL(descriptors[0].caption, "G1_INJECTION");
    BOOST_CHECK_EQUAL(descriptors[0].unit, "MW");
    BOOST_CHECK_EQUAL(descriptors[1].caption, "G1_WITHDRAWAL");
    BOOST_CHECK_EQUAL(descriptors[1].unit, "MW");
    BOOST_CHECK_EQUAL(descriptors[2].caption, "G1_LEVEL");
    BOOST_CHECK_EQUAL(descriptors[2].unit, "MWh");

    BOOST_CHECK_EQUAL(descriptors[3].caption, "G2_INJECTION");
    BOOST_CHECK_EQUAL(descriptors[4].caption, "G2_WITHDRAWAL");
    BOOST_CHECK_EQUAL(descriptors[5].caption, "G2_LEVEL");
}

BOOST_AUTO_TEST_CASE(set_hourly_value_routes_injection_withdrawal_level)
{
    Area* area = nullptr;
    auto study = makeStudyWithOneArea(area);

    addSTSToArea(area, "s1")->properties.groupName = "G1";
    addSTSToArea(area, "s2")->properties.groupName = "G2";

    auto descriptors = STSbyGroupTraits::buildColumnDescriptors(area);
    BOOST_REQUIRE_EQUAL(descriptors.size(), 6);

    State state(*study);
    state.area = area;
    state.year = 0;
    state.hourInTheYear = 5;
    state.hourInTheWeek = 0;

    RESULTATS_HORAIRES rh;
    rh.ShortTermStorage.resize(2);
    rh.ShortTermStorage[0].injection.assign(nbHoursInAWeek, 1.0);
    rh.ShortTermStorage[0].withdrawal.assign(nbHoursInAWeek, 2.0);
    rh.ShortTermStorage[0].level.assign(nbHoursInAWeek, 10.0);
    rh.ShortTermStorage[1].injection.assign(nbHoursInAWeek, 3.0);
    rh.ShortTermStorage[1].withdrawal.assign(nbHoursInAWeek, 4.0);
    rh.ShortTermStorage[1].level.assign(nbHoursInAWeek, 20.0);
    state.hourlyResults = &rh;

    std::vector<IntermediateValues> pValues(descriptors.size());
    STSbyGroupTraits::setHourlyValue(pValues, state, 0, descriptors);

    BOOST_CHECK_EQUAL(pValues[0][5], 1.0);  // G1 injection
    BOOST_CHECK_EQUAL(pValues[1][5], 2.0);  // G1 withdrawal
    BOOST_CHECK_EQUAL(pValues[2][5], 10.0); // G1 level
    BOOST_CHECK_EQUAL(pValues[3][5], 3.0);  // G2 injection
    BOOST_CHECK_EQUAL(pValues[4][5], 4.0);  // G2 withdrawal
    BOOST_CHECK_EQUAL(pValues[5][5], 20.0); // G2 level
    BOOST_CHECK_EQUAL(pValues[0][0], 0.0);
}

BOOST_AUTO_TEST_SUITE_END()
