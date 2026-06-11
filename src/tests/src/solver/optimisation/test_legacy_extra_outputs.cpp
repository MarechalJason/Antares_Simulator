// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacyExtraOutputs.h"

using Antares::IO::Outputs::SimulationTable;
using Antares::Optimisation::LinearProblemApi::FillContext;
using Antares::Optimization::AddLegacyExtraOutputs;
using Antares::Optimization::LegacyVariableInfo;

namespace
{
// Rows as produced by SimulationTable::storageIntoRows(), column order:
// block, component, output, absolute_time_index, block_time_index,
// scenario_index, value, basis_status.
struct Row
{
    explicit Row(const std::vector<std::string>& columns):
        block(columns[0]),
        component(columns[1]),
        output(columns[2]),
        absoluteTimeIndex(columns[3]),
        blockTimeIndex(columns[4]),
        scenarioIndex(columns[5]),
        value(std::stod(columns[6])),
        basisStatus(columns[7])
    {
    }

    std::string block;
    std::string component;
    std::string output;
    std::string absoluteTimeIndex;
    std::string blockTimeIndex;
    std::string scenarioIndex;
    double value;
    std::string basisStatus;
};

std::vector<Row> RowsForOutput(const SimulationTable& table, const std::string& output)
{
    std::vector<Row> rows;
    for (const auto& columns: table.storageIntoRows())
    {
        if (columns[2] == output)
        {
            rows.emplace_back(columns);
        }
    }
    return rows;
}

struct Fixture
{
    // One thermal generation variable, one unsupplied/spillage pair for
    // "area1", an unsupplied variable without a spillage counterpart for
    // "area2", and one unnamed slot.
    Fixture()
    {
        info.resize(5);
        info[0] = LegacyVariableInfo{"DispatchableProduction", "cluster1", 168};
        info[1] = LegacyVariableInfo{"UnsuppliedEnergy", "area1", 168};
        info[2] = LegacyVariableInfo{"Spillage", "area1", 168};
        info[3] = LegacyVariableInfo{"UnsuppliedEnergy", "area2", 168};
    }

    std::vector<std::optional<LegacyVariableInfo>> info;
    std::vector<double> values = {3600., 52., 7., 13., -1.};
    std::vector<double> costs = {35., 10000., 4., 20000., -1.};
    // Block covering timesteps [168, 335], year 2.
    FillContext fillContext{0, 167, 168, 335, 2};
    unsigned currentBlock = 1;
    SimulationTable table;
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(test_legacy_extra_outputs, Fixture)

BOOST_AUTO_TEST_CASE(prop_cost_is_generation_cost_times_generation_power)
{
    AddLegacyExtraOutputs(table, info, values, costs, fillContext, currentBlock);

    const auto rows = RowsForOutput(table, "prop_cost");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "cluster1");
    BOOST_CHECK_CLOSE(rows[0].value, 35. * 3600., 1e-9);
}

BOOST_AUTO_TEST_CASE(extra_output_entries_carry_block_time_and_scenario)
{
    AddLegacyExtraOutputs(table, info, values, costs, fillContext, currentBlock);

    const auto rows = RowsForOutput(table, "prop_cost");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].block, "2");              // currentBlock + 1
    BOOST_CHECK_EQUAL(rows[0].absoluteTimeIndex, "169"); // timeIndex + 1
    BOOST_CHECK_EQUAL(rows[0].blockTimeIndex, "1");      // first timestep of the block
    BOOST_CHECK_EQUAL(rows[0].scenarioIndex, "2");
    BOOST_CHECK_EQUAL(rows[0].basisStatus, "None");
}

BOOST_AUTO_TEST_CASE(imbalance_cost_combines_unsupplied_and_spilled_energy)
{
    AddLegacyExtraOutputs(table, info, values, costs, fillContext, currentBlock);

    const auto rows = RowsForOutput(table, "imbalance_cost");
    BOOST_REQUIRE_EQUAL(rows.size(), 1);
    BOOST_CHECK_EQUAL(rows[0].component, "area1");
    BOOST_CHECK_CLOSE(rows[0].value, 10000. * 52. + 4. * 7., 1e-9);
}

BOOST_AUTO_TEST_CASE(imbalance_cost_is_skipped_when_spillage_is_missing)
{
    AddLegacyExtraOutputs(table, info, values, costs, fillContext, currentBlock);

    // "area2" has UnsuppliedEnergy but no Spillage variable: no entry.
    for (const auto& row: RowsForOutput(table, "imbalance_cost"))
    {
        BOOST_CHECK_NE(row.component, "area2");
    }
}

BOOST_AUTO_TEST_CASE(other_variables_produce_no_extra_output)
{
    AddLegacyExtraOutputs(table, info, values, costs, fillContext, currentBlock);

    BOOST_CHECK_EQUAL(table.rowCount(), 2); // one prop_cost + one imbalance_cost
}

BOOST_AUTO_TEST_SUITE_END()
