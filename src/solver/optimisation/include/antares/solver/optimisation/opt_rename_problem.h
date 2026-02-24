// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <vector>

class Namer
{
public:
    explicit Namer(std::vector<std::string>& target_names);
    void updateTimeStep(unsigned timeStep);
    void updateArea(const std::string& area);
    void updateExtremities(const std::string& origin, const std::string& destination);

protected:
    void setLinkElementName(unsigned varIndex, const std::string& variableType);
    void setAreaElementNameHour(unsigned varIndex, const std::string& variableType);
    void setAreaElementNameWeek(unsigned varIndex, const std::string& variableType);
    void setAreaElementName(unsigned varIndex,
                            const std::string& variableType,
                            const std::string& timeGranularity);
    void setThermalClusterElementName(unsigned varIndex,
                                      const std::string& variableType,
                                      const std::string& clusterName);
    std::string timeIdentifier(const std::string& timeGranularity);
    std::string linkLocation();
    std::string areaLocation();
    std::vector<std::string>& names();

private:
    std::string origin_;
    std::string destination_;
    std::string area_;
    unsigned timeStep_ = 0;
    std::vector<std::string>& names_;
};

class VariableNamer: public Namer
{
public:
    using Namer::Namer;
    void dispatchableProduction(unsigned varIndex, const std::string& clusterName);
    void nodu(unsigned varIndex, const std::string& clusterName);
    void numberStoppingDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void numberStartingDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void numberBreakingDownDispatchableUnits(unsigned varIndex, const std::string& clusterName);
    void ntcDirect(unsigned varIndex);
    void interconnectionDirectCost(unsigned varIndex);
    void interconnectionIndirectCost(unsigned varIndex);
    void shortTermStorageInjection(unsigned varIndex, const std::string& sts_name);
    void shortTermStorageWithdrawal(unsigned varIndex, const std::string& sts_name);
    void shortTermStorageLevel(unsigned varIndex, const std::string& sts_name);
    void shortTermStorageOverflow(unsigned varIndex, const std::string& sts_name);
    void shortTermStorageCostVariationInjection(unsigned varIndex, const std::string& sts_name);
    void shortTermStorageCostVariationWithdrawal(unsigned varIndex, const std::string& sts_name);
    void hydroPower(unsigned varIndex);
    void hydroPowerDown(unsigned varIndex);
    void hydroPowerUp(unsigned varIndex);
    void pumping(unsigned varIndex);
    void hydroLevel(unsigned varIndex);
    void overflow(unsigned varIndex);
    void finalStorage(unsigned varIndex);
    void layerStorage(unsigned varIndex, int layerIndex);
    void unsuppliedEnergy(unsigned varIndex);
    void spillage(unsigned varIndex);
    void areaBalance(unsigned varIndex);

private:
    void setAreaVariableName(unsigned varIndex, const std::string& variableType, int layerIndex);
    void setShortTermStorageVariableName(unsigned varIndex,
                                         const std::string& variableType,
                                         const std::string& sts_name);
};

class ConstraintNamer: public Namer
{
public:
    using Namer::Namer;

    void flowDissociation(unsigned constrIndex);
    void areaBalance(unsigned constrIndex);
    void fictiveLoads(unsigned constrIndex);
    void maxUnsuppliedEnergy(unsigned constrIndex);
    void hydroPower(unsigned constrIndex);
    void hydroPowerSmoothingUsingVariationSum(unsigned constrIndex);
    void hydroPowerSmoothingUsingVariationMaxDown(unsigned constrIndex);
    void hydroPowerSmoothingUsingVariationMaxUp(unsigned constrIndex);
    void minHydroPower(unsigned constrIndex);
    void maxHydroPower(unsigned constrIndex);
    void maxPumping(unsigned constrIndex);
    void areaHydroLevel(unsigned constrIndex);
    void finalStockEquivalent(unsigned constrIndex);
    void finalStockExpression(unsigned constrIndex);
    void nbUnitsOutageLessThanNbUnitsStop(unsigned constrIndex, const std::string& clusterName);
    void nbDispUnitsMinBoundSinceMinUpTime(unsigned constrIndex, const std::string& clusterName);
    void minDownTime(unsigned constrIndex, const std::string& clusterName);
    void pMaxDispatchableGeneration(unsigned constrIndex, const std::string& clusterName);
    void pMinDispatchableGeneration(unsigned constrIndex, const std::string& clusterName);
    void consistenceNODU(unsigned constrIndex, const std::string& clusterName);
    void shortTermStorageLevel(unsigned constrIndex, const std::string& name);
    void bindingConstraintHour(unsigned constrIndex, const std::string& name);
    void bindingConstraintDay(unsigned constrIndex, const std::string& name);
    void bindingConstraintWeek(unsigned constrIndex, const std::string& name);
    void csrFlowDissociation(unsigned constrIndex);
    void csrAreaBalance(unsigned constrIndex);
    void shortTermStorageCostVariation(const std::string& constrIndex_name,
                                       unsigned constrIndex,
                                       const std::string& sts_name);
    void shortTermStorageCumulation(const std::string& constraint_type,
                                    unsigned constrIndex,
                                    const std::string& sts_name,
                                    const std::string& constrIndex_name);

private:
    void bindingConstraint(unsigned constrIndex,
                           const std::string& name,
                           const std::pair<std::string, std::string>& timeGranularity);
};
