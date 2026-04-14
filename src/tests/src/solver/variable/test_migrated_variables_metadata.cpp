// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test migrated variables metadata"
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/variable/economy/STStorageInjectionByCluster.h"
#include "antares/solver/variable/economy/STStorageLevelsByCluster.h"
#include "antares/solver/variable/economy/STStorageWithdrawalByCluster.h"
#include "antares/solver/variable/economy/avail-dispatchable-generation.h"
#include "antares/solver/variable/economy/balance.h"
#include "antares/solver/variable/economy/dispatchable-generation-margin.h"
#include "antares/solver/variable/economy/domesticUnsuppliedEnergy.h"
#include "antares/solver/variable/economy/dtgMarginAfterCsr.h"
#include "antares/solver/variable/economy/hydroCost.h"
#include "antares/solver/variable/economy/hydrostorage.h"
#include "antares/solver/variable/economy/inflow.h"
#include "antares/solver/variable/economy/links/congestionFee.h"
#include "antares/solver/variable/economy/links/flowLinear.h"
#include "antares/solver/variable/economy/links/flowQuad.h"
#include "antares/solver/variable/economy/links/hurdleCosts.h"
#include "antares/solver/variable/economy/links/loopFlow.h"
#include "antares/solver/variable/economy/links/marginalCost.h"
#include "antares/solver/variable/economy/localMatchingRuleViolations.h"
#include "antares/solver/variable/economy/max-mrg-csr.h"
#include "antares/solver/variable/economy/max-mrg.h"
#include "antares/solver/variable/economy/nonProportionalCost.h"
#include "antares/solver/variable/economy/operatingCost.h"
#include "antares/solver/variable/economy/overallCostCsr.h"
#include "antares/solver/variable/economy/overflow.h"
#include "antares/solver/variable/economy/price.h"
#include "antares/solver/variable/economy/priceCSR.h"
#include "antares/solver/variable/economy/pumping.h"
#include "antares/solver/variable/economy/residual.h"
#include "antares/solver/variable/economy/spilledEnergy.h"
#include "antares/solver/variable/economy/thermalAirPollutantEmissions.h"

using namespace Antares::Solver::Variable::Economy;

// Static metadata checks for migrated variables.
//
// Goal: guarantee that the Traits-based migration preserved the exposed
// Caption / Unit / columnCount visible to the survey reports. Any drift in
// those strings would break the output format (column headers, digest
// captions) in a way that functional tests wouldn't catch early.

BOOST_AUTO_TEST_SUITE(migrated_variables_metadata)

BOOST_AUTO_TEST_CASE(area_variables_on_economy_base)
{
    BOOST_CHECK_EQUAL(VCardBalance::Caption(), "BALANCE");
    BOOST_CHECK_EQUAL(VCardBalance::Unit(), "MWh");
    BOOST_CHECK_EQUAL(static_cast<int>(VCardBalance::columnCount), 1);

    BOOST_CHECK_EQUAL(VCardHydroStorage::Caption(), "H. STOR");
    BOOST_CHECK_EQUAL(VCardHydroStorage::Unit(), "MWh");

    BOOST_CHECK_EQUAL(VCardInflows::Caption(), "H. INFL");
    BOOST_CHECK_EQUAL(VCardInflows::Unit(), "MWh");

    BOOST_CHECK_EQUAL(VCardOverflow::Caption(), "H. OVFL");
    BOOST_CHECK_EQUAL(VCardOverflow::Unit(), "%");

    BOOST_CHECK_EQUAL(VCardPumping::Caption(), "H. PUMP");
    BOOST_CHECK_EQUAL(VCardPumping::Unit(), "MWh");

    BOOST_CHECK_EQUAL(VCardOperatingCost::Caption(), "OP. COST");
    BOOST_CHECK_EQUAL(VCardOperatingCost::Unit(), "Euro");

    BOOST_CHECK_EQUAL(VCardNonProportionalCost::Caption(), "NP COST");
    BOOST_CHECK_EQUAL(VCardNonProportionalCost::Unit(), "Euro");

    BOOST_CHECK_EQUAL(VCardHydroCost::Caption(), "H. COST");
    BOOST_CHECK_EQUAL(VCardHydroCost::Unit(), "Euro");

    BOOST_CHECK_EQUAL(VCardPrice::Caption(), "MRG. PRICE");
    BOOST_CHECK_EQUAL(VCardPrice::Unit(), "Euro");

    BOOST_CHECK_EQUAL(VCardDomesticUnsuppliedEnergy::Caption(), "DENS");
    BOOST_CHECK_EQUAL(VCardDomesticUnsuppliedEnergy::Unit(), "MWh");

    BOOST_CHECK_EQUAL(VCardResidualLoad::Caption(), "RES LOAD");
    BOOST_CHECK_EQUAL(VCardResidualLoad::Unit(), "MWh");
    BOOST_CHECK_EQUAL(static_cast<int>(VCardResidualLoad::columnCount), 1);

    BOOST_CHECK_EQUAL(VCardAvailableDispatchGen::Caption(), "AVL DTG");
    BOOST_CHECK_EQUAL(VCardDispatchableGenMargin::Caption(), "DTG MRG");
    BOOST_CHECK_EQUAL(VCardNearPriceCap::Caption(), "NPCAP HOURS");
    BOOST_CHECK_EQUAL(VCardNearPriceCap::Unit(), "Hours");
    BOOST_CHECK_EQUAL(VCardSpilledEnergy::Caption(), "SPIL. ENRG");

    BOOST_CHECK_EQUAL(VCardMARGE::Caption(), "MAX MRG");
    BOOST_CHECK_EQUAL(VCardMARGE::Unit(), "MWh");
}

BOOST_AUTO_TEST_CASE(csr_variables)
{
    BOOST_CHECK_EQUAL(VCardPriceCSR::Caption(), "MRG. PRICE CSR");
    BOOST_CHECK_EQUAL(VCardPriceCSR::Unit(), "Euro");

    BOOST_CHECK_EQUAL(VCardDtgMarginCsr::Caption(), "DTG MRG CSR");
    BOOST_CHECK_EQUAL(VCardDtgMarginCsr::Unit(), "MWh");

    BOOST_CHECK_EQUAL(VCardLMRViolations::Caption(), "LMR VIOL.");
    BOOST_CHECK_EQUAL(VCardLMRViolations::Unit(), " ");

    BOOST_CHECK_EQUAL(VCardMAX_MRG_CSR::Caption(), "MAX MRG CSR");
    BOOST_CHECK_EQUAL(VCardMAX_MRG_CSR::Unit(), "MWh");
}

BOOST_AUTO_TEST_CASE(multi_column_variables)
{
    BOOST_CHECK_EQUAL(VCardThermalAirPollutantEmissions::Caption(), "");
    BOOST_CHECK_EQUAL(VCardThermalAirPollutantEmissions::Unit(), "Tons");
    BOOST_CHECK_EQUAL(static_cast<int>(VCardThermalAirPollutantEmissions::columnCount), 13);
}

BOOST_AUTO_TEST_CASE(sts_by_cluster_variables)
{
    BOOST_CHECK_EQUAL(VCardSTstorageInjectionByCluster::Caption(), "STS inj by plant");
    BOOST_CHECK_EQUAL(VCardSTstorageInjectionByCluster::Unit(), "P-injection - MW");
    BOOST_CHECK_EQUAL(VCardSTstorageWithdrawalByCluster::Caption(), "STS withdrawal by plant");
    BOOST_CHECK_EQUAL(VCardSTstorageLevelsByCluster::Caption(), "STS lvl by plant");
}

BOOST_AUTO_TEST_CASE(dispatchable_plant_by_cluster_variables)
{
    BOOST_CHECK_EQUAL(VCardMinDispatchableGenByPlant::Caption(), "MIN DTG by plant");
    BOOST_CHECK_EQUAL(VCardMinDispatchableGenByPlant::Unit(), "MIN GEN - MWh");
    BOOST_CHECK_EQUAL(VCardProfitByPlant::Caption(), "Profit by plant");
    BOOST_CHECK_EQUAL(VCardProfitByPlant::Unit(), "Profit - Euro");
}

BOOST_AUTO_TEST_CASE(link_variables)
{
    BOOST_CHECK_EQUAL(VCardFlowLinear::Caption(), "FLOW LIN.");
    BOOST_CHECK_EQUAL(VCardFlowLinear::Unit(), "MWh");
    BOOST_CHECK_EQUAL(static_cast<int>(VCardFlowLinear::decimal), 0);

    using CongestionFeeVCard = VCard_LinkBase<CongestionFeeTraits>;
    using MarginalCostVCard = VCard_LinkBase<MarginalCostTraits>;
    using HurdleCostsVCard = VCard_LinkBase<HurdleCostsTraits>;

    BOOST_CHECK_EQUAL(CongestionFeeVCard::Caption(), "CONG. FEE (ALG.)");
    BOOST_CHECK_EQUAL(CongestionFeeVCard::Unit(), "Euro");
    BOOST_CHECK_EQUAL(MarginalCostVCard::Caption(), "MARG. COST");
    BOOST_CHECK_EQUAL(MarginalCostVCard::Unit(), "Euro/MW");
    BOOST_CHECK_EQUAL(static_cast<int>(MarginalCostVCard::decimal), 2);
    BOOST_CHECK_EQUAL(HurdleCostsVCard::Caption(), "HURDLE COST");
    BOOST_CHECK_EQUAL(VCardFlowQuad::Caption(), "FLOW QUAD.");
    BOOST_CHECK_EQUAL(VCardLoopFlow::Caption(), "LOOP FLOW");
}

BOOST_AUTO_TEST_CASE(renewable_by_plant_variable)
{
    BOOST_CHECK_EQUAL(VCardProductionByRenewablePlant::Caption(), "RES generation by plant");
    BOOST_CHECK_EQUAL(VCardProductionByRenewablePlant::Unit(), "MWh");
}

BOOST_AUTO_TEST_SUITE_END()
