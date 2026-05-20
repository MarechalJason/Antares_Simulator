// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "surveyresults_test_helpers.h"

BOOST_AUTO_TEST_SUITE(digest_with_variables)

BOOST_AUTO_TEST_CASE(exports_digest_from_residual_load_variable_tree)
{
    auto study = makeStudyForResidualDigest();
    ResidualDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_residual_load_variable_tree_with_two_areas)
{
    auto study = makeStudyForResidualDigestWithTwoAreas();
    ResidualDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea2\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_one_column)
{
    auto study = makeStudyForOneColumnDynamicDigest();
    OneColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_one_column_with_two_areas)
{
    auto study = makeStudyForOneColumnDynamicDigestWithTwoAreas();
    OneColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea2\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_for_static_and_dynamic_one_column_variables)
{
    auto study = makeStudyForMixedResidualAndOneColumnDynamicDigest();
    ResidualDigestVariables residualVariables;
    const auto& residualDigest = runSimulationAndExportDigest(*study, residualVariables, 5.0);
    OneColumnDynamicDigestVariables dynamicVariables;
    const auto& dynamicDigest = runSimulationAndExportDigest(*study, dynamicVariables, 5.0);

    BOOST_CHECK_NE(residualDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\tarea1\t840\n"), std::string::npos);

    BOOST_CHECK_NE(dynamicDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\tarea1\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_for_static_and_dynamic_one_column_variables_with_two_areas)
{
    auto study = makeStudyForMixedResidualAndOneColumnDynamicDigest(2);
    ResidualDigestVariables residualVariables;
    const auto& residualDigest = runSimulationAndExportDigest(*study, residualVariables, 5.0);
    OneColumnDynamicDigestVariables dynamicVariables;
    const auto& dynamicDigest = runSimulationAndExportDigest(*study, dynamicVariables, 5.0);

    BOOST_CHECK_NE(residualDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tRES LOAD\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(residualDigest.find("\tarea2\t840\n"), std::string::npos);

    BOOST_CHECK_NE(dynamicDigest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t1\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tDYN_COL_1\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\t\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\tarea1\t840\n"), std::string::npos);
    BOOST_CHECK_NE(dynamicDigest.find("\tarea2\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_two_columns)
{
    auto study = makeStudyForTwoColumnDynamicDigest();
    TwoColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t2\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\tDYN_COL_2\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t1008\t2016\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_dynamic_variable_with_two_columns_with_two_areas)
{
    auto study = makeStudyForTwoColumnDynamicDigestWithTwoAreas();
    TwoColumnDynamicDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t2\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tDYN_COL_1\tDYN_COL_2\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t1008\t2016\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea2\t1008\t2016\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(exports_digest_from_two_static_and_one_dynamic_two_columns_variables)
{
    auto study = makeStudyForTwoStaticAndTwoColumnDynamicDigest();
    TwoStaticOneDynamicTwoColumnsDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 3.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t4\t1\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tAVL DTG\tRES LOAD\tDYN_COL_1\tDYN_COL_2\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tMWh\tMWh\tMWh\tMWh\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tEXP\tEXP\tEXP\tEXP\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t0\t504\t672\t1344\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(digest_column_order_with_multiple_variables)
{
    auto study = makeStudyForTwoStaticAndTwoColumnDynamicDigest();
    TwoStaticOneDynamicTwoColumnsDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 3.0);

    auto avlDtgPos = digest.find("\tAVL DTG\t");
    auto resLoadPos = digest.find("\tRES LOAD\t");
    auto dynCol1Pos = digest.find("\tDYN_COL_1\t");

    BOOST_REQUIRE(avlDtgPos != std::string::npos);
    BOOST_REQUIRE(resLoadPos != std::string::npos);
    BOOST_REQUIRE(dynCol1Pos != std::string::npos);

    BOOST_CHECK(avlDtgPos < resLoadPos);
    BOOST_CHECK(resLoadPos < dynCol1Pos);
}

BOOST_AUTO_TEST_CASE(digest_column_order_with_thermal_pollutant_co2)
{
    auto study = makeStudyWithAvlDtgCo2AndResLoad();
    AvlDtgWithCo2ResLoadVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 3.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t3\t1\t0\n"),
                   std::string::npos);

    auto co2Pos = digest.find("\tCO2 EMIS.\t");
    auto avlDtgPos = digest.find("\tAVL DTG\t");
    auto resLoadPos = digest.find("\tRES LOAD\n");

    BOOST_REQUIRE(avlDtgPos != std::string::npos);
    BOOST_REQUIRE(co2Pos != std::string::npos);
    BOOST_REQUIRE(resLoadPos != std::string::npos);

    BOOST_CHECK(co2Pos < avlDtgPos);
    BOOST_CHECK(avlDtgPos < resLoadPos);
}

BOOST_AUTO_TEST_CASE(digest_values_with_thermal_pollutant_avl_dtg_res_load_and_dynamic)
{
    auto study = makeStudyWithAllVariables();
    AllVariablesDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 3.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t5\t1\t0\n"),
                   std::string::npos);

    auto co2Pos = digest.find("\tCO2 EMIS.\t");
    auto avlDtgPos = digest.find("\tAVL DTG\t");
    auto resLoadPos = digest.find("\tRES LOAD\t");
    auto dynCol1Pos = digest.find("\tDYN_COL_1\t");
    auto dynCol2Pos = digest.find("\tDYN_COL_2\n");

    BOOST_REQUIRE(co2Pos != std::string::npos);
    BOOST_REQUIRE(avlDtgPos != std::string::npos);
    BOOST_REQUIRE(resLoadPos != std::string::npos);
    BOOST_REQUIRE(dynCol1Pos != std::string::npos);
    BOOST_REQUIRE(dynCol2Pos != std::string::npos);

    BOOST_CHECK(co2Pos < avlDtgPos);
    BOOST_CHECK(avlDtgPos < resLoadPos);
    BOOST_CHECK(resLoadPos < dynCol1Pos);
    BOOST_CHECK(dynCol1Pos < dynCol2Pos);

    BOOST_CHECK_NE(digest.find("\tarea1\t16800\t33600\t504\t672\t1344\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(digest_values_with_thermal_pollutant_avl_dtg_res_load_and_dynamic_two_areas)
{
    auto study = makeStudyWithAllVariables(2);
    AllVariablesDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 3.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t5\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea1\t16800\t33600\t504\t672\t1344\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(digest_stays_aligned_when_dynamic_columns_vary_by_area)
{
    auto study = makeStudyForVaryingColumnsThenResidualDigest(2);
    VaryingColumnsThenResidualDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t3\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tVAR_A\tVAR_B\tRES LOAD\n"), std::string::npos);

    // Area 1 has no VAR_B contribution: RES LOAD must remain in the third column.
    BOOST_CHECK_NE(digest.find("\tarea1\t2520\t0\t840\n"), std::string::npos);
    BOOST_CHECK_NE(digest.find("\tarea2\t2520\t4200\t840\n"), std::string::npos);
}

BOOST_AUTO_TEST_CASE(digest_aligned_with_static_and_disjoint_groups_across_two_areas)
{
    // Regression: 2 areas, 2 static variables (AVL DTG, RES LOAD), and a dynamic
    // variable whose groups are disjoint per area (GAS only in area1, LIGNITE only
    // in area2). Header must show the union {GAS, LIGNITE} and each area's row must
    // place the value under its own group while leaving the other group's column at 0.
    auto study = makeStudyForStaticAndDisjointGroupsDigest(2);
    StaticAndDisjointGroupsDigestVariables variables;
    const auto& digest = runSimulationAndExportDigest(*study, variables, 5.0);

    BOOST_CHECK_NE(digest.find("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n\t4\t2\t0\n"),
                   std::string::npos);
    BOOST_CHECK_NE(digest.find("\t\tAVL DTG\tRES LOAD\tGAS\tLIGNITE\n"), std::string::npos);

    // area1: only GAS contributes (15 * 168 = 2520); LIGNITE column stays 0.
    BOOST_CHECK_NE(digest.find("\tarea1\t0\t840\t2520\t0\n"), std::string::npos);
    // area2: only LIGNITE contributes (25 * 168 = 4200); GAS column stays 0.
    BOOST_CHECK_NE(digest.find("\tarea2\t0\t840\t0\t4200\n"), std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
