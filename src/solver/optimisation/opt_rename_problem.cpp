// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_rename_problem.h"

#include <algorithm>
#include <map>

const std::string SEP = "::";
const std::string AREA_SEP = "$$";

const std::string HOUR("hour");
const std::string DAY("day");
const std::string WEEK("week");
const std::string LINK("link");
const std::string AREA("area");

std::string ShortTermStorageCumulationIdentifier(const std::string& name)
{
    return "Constraint<" + name + ">";
}

std::string LocationIdentifier(const std::string& location, const std::string& locationType)
{
    return locationType + "<" + location + ">";
}

std::string BuildName(const std::string& name,
                      const std::string& location,
                      const std::string& additional_identifier)
{
    std::string result = name + SEP + location + SEP + additional_identifier;
    std::replace(result.begin(), result.end(), ' ', '*');
    return result;
}

Namer::Namer(std::vector<std::string>& target_names):
    names_(target_names)
{
}

void Namer::updateTimeStep(unsigned timeStep)
{
    timeStep_ = timeStep;
}

void Namer::updateArea(const std::string& area)
{
    area_ = area;
}

void Namer::updateExtremities(const std::string& origin, const std::string& destination)
{
    origin_ = origin;
    destination_ = destination;
}

std::string Namer::timeIdentifier(const std::string& timeGranularity)
{
    return timeGranularity + "<" + std::to_string(timeStep_) + ">";
}

std::string Namer::linkLocation()
{
    return LocationIdentifier(origin_ + AREA_SEP + destination_, LINK);
}

std::string Namer::areaLocation()
{
    return LocationIdentifier(area_, AREA);
}

std::vector<std::string>& Namer::names()
{
    return names_;
}

void Namer::setLinkElementName(unsigned elementIndex, const std::string& elementType)
{
    std::string location = linkLocation();
    std::string time = timeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[elementIndex] = name;
}

void Namer::setAreaElementNameHour(unsigned elementIndex, const std::string& elementType)
{
    setAreaElementName(elementIndex, elementType, HOUR);
}

void Namer::setAreaElementNameWeek(unsigned elementIndex, const std::string& elementType)
{
    setAreaElementName(elementIndex, elementType, WEEK);
}

void Namer::setAreaElementName(unsigned elementIndex,
                               const std::string& elementType,
                               const std::string& timeGranularity)
{
    std::string location = LocationIdentifier(area_, AREA);
    std::string time = timeIdentifier(timeGranularity);
    std::string name = BuildName(elementType, location, time);
    names_[elementIndex] = name;
}

void VariableNamer::setAreaVariableName(unsigned varIndex,
                                        const std::string& variableType,
                                        int layerIndex)
{
    std::string location = areaLocation() + SEP + "Layer<" + std::to_string(layerIndex) + ">";
    std::string time = timeIdentifier(HOUR);
    std::string name = BuildName(variableType, location, time);
    names()[varIndex] = name;
}

void Namer::setThermalClusterElementName(unsigned varIndex,
                                         const std::string& elementType,
                                         const std::string& clusterName)
{
    std::string location = areaLocation() + SEP + "ThermalCluster" + "<" + clusterName + ">";
    std::string time = timeIdentifier(HOUR);
    std::string name = BuildName(elementType, location, time);
    names_[varIndex] = name;
}

void VariableNamer::dispatchableProduction(unsigned varIndex, const std::string& clusterName)
{
    setThermalClusterElementName(varIndex, "DispatchableProduction", clusterName);
}

void VariableNamer::nodu(unsigned varIndex, const std::string& clusterName)
{
    setThermalClusterElementName(varIndex, "NODU", clusterName);
}

void VariableNamer::numberStoppingDispatchableUnits(unsigned varIndex,
                                                    const std::string& clusterName)
{
    setThermalClusterElementName(varIndex, "NumberStoppingDispatchableUnits", clusterName);
}

void VariableNamer::numberStartingDispatchableUnits(unsigned varIndex,
                                                    const std::string& clusterName)
{
    setThermalClusterElementName(varIndex, "NumberStartingDispatchableUnits", clusterName);
}

void VariableNamer::numberBreakingDownDispatchableUnits(unsigned varIndex,
                                                        const std::string& clusterName)
{
    setThermalClusterElementName(varIndex, "NumberBreakingDownDispatchableUnits", clusterName);
}

void VariableNamer::ntcDirect(unsigned varIndex)
{
    setLinkElementName(varIndex, "NTCDirect");
}

void VariableNamer::interconnectionDirectCost(unsigned varIndex)
{
    setLinkElementName(varIndex, "IntercoDirectCost");
}

void VariableNamer::interconnectionIndirectCost(unsigned varIndex)
{
    setLinkElementName(varIndex, "IntercoIndirectCost");
}

void VariableNamer::setShortTermStorageVariableName(unsigned varIndex,
                                                    const std::string& variableType,
                                                    const std::string& sts_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = timeIdentifier(HOUR);
    std::string name = BuildName(variableType, location, time);
    names()[varIndex] = name;
}

void VariableNamer::shortTermStorageInjection(unsigned varIndex, const std::string& sts_name)
{
    setShortTermStorageVariableName(varIndex, "Injection", sts_name);
}

void VariableNamer::shortTermStorageWithdrawal(unsigned varIndex, const std::string& sts_name)
{
    setShortTermStorageVariableName(varIndex, "Withdrawal", sts_name);
}

void VariableNamer::shortTermStorageLevel(unsigned varIndex, const std::string& sts_name)
{
    setShortTermStorageVariableName(varIndex, "Level", sts_name);
}

void VariableNamer::shortTermStorageOverflow(unsigned varIndex, const std::string& sts_name)
{
    setShortTermStorageVariableName(varIndex, "Overflow", sts_name);
}

void VariableNamer::shortTermStorageCostVariationInjection(unsigned varIndex,
                                                           const std::string& sts_name)
{
    setShortTermStorageVariableName(varIndex, "CostVariationInjection", sts_name);
}

void VariableNamer::shortTermStorageCostVariationWithdrawal(unsigned varIndex,
                                                            const std::string& sts_name)
{
    setShortTermStorageVariableName(varIndex, "CostVariationWithdrawal", sts_name);
}

void VariableNamer::hydroPower(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "HydProd");
}

void VariableNamer::hydroPowerDown(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "HydProdDown");
}

void VariableNamer::hydroPowerUp(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "HydProdUp");
}

void VariableNamer::pumping(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "Pumping");
}

void VariableNamer::hydroLevel(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "HydroLevel");
}

void VariableNamer::overflow(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "Overflow");
}

void VariableNamer::layerStorage(unsigned varIndex, int layerIndex)
{
    setAreaVariableName(varIndex, "LayerStorage", layerIndex);
}

void VariableNamer::finalStorage(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "FinalStorage");
}

void VariableNamer::unsuppliedEnergy(unsigned varIndex)
{
    // TODO : what is "PositiveUnsuppliedEnergy" ?
    // We mean "UnsuppliedEnergy". To be replaced.
    setAreaElementNameHour(varIndex, "PositiveUnsuppliedEnergy");
}

void VariableNamer::spillage(unsigned varIndex)
{
    // TODO : what is "NegativeUnsuppliedEnergy" ?
    // Should be replaced with "Spillage".
    setAreaElementNameHour(varIndex, "NegativeUnsuppliedEnergy");
}

void VariableNamer::areaBalance(unsigned varIndex)
{
    setAreaElementNameHour(varIndex, "AreaBalance");
}

void ConstraintNamer:: FlowDissociation(unsigned constrIndex)
{
    setLinkElementName(constrIndex, "flowDissociation");
}

void ConstraintNamer::csrFlowDissociation(unsigned constrIndex)
{
    setLinkElementName(constrIndex, "csrFlowDissociation");
}

void ConstraintNamer::csrAreaBalance(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "csrAreaBalance");
}

void ConstraintNamer::areaBalance(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "AreaBalance");
}

void ConstraintNamer:: FictiveLoads(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "fictiveLoads");
}

void ConstraintNamer:: MaxUnsuppliedEnergy(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "MaxUnsupEnergy");
}

void ConstraintNamer::hydroPower(unsigned constrIndex)
{
    setAreaElementNameWeek(constrIndex, "HydroPower");
}

void ConstraintNamer:: HydroPowerSmoothingUsingVariationSum(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "hydroPowerSmoothingUsingVariationSum");
}

void ConstraintNamer:: HydroPowerSmoothingUsingVariationMaxDown(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "hydroPowerSmoothingUsingVariationMaxDown");
}

void ConstraintNamer:: HydroPowerSmoothingUsingVariationMaxUp(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "hydroPowerSmoothingUsingVariationMaxUp");
}

void ConstraintNamer:: MinHydroPower(unsigned constrIndex)
{
    setAreaElementNameWeek(constrIndex, "minHydroPower");
}

void ConstraintNamer:: MaxHydroPower(unsigned constrIndex)
{
    setAreaElementNameWeek(constrIndex, "maxHydroPower");
}

void ConstraintNamer:: MaxPumping(unsigned constrIndex)
{
    setAreaElementNameWeek(constrIndex, "maxPumping");
}

void ConstraintNamer:: AreaHydroLevel(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "areaHydroLevel");
}

void ConstraintNamer:: FinalStockEquivalent(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "finalStockEquivalent");
}

void ConstraintNamer:: FinalStockExpression(unsigned constrIndex)
{
    setAreaElementNameHour(constrIndex, "finalStockExpression");
}

void ConstraintNamer::bindingConstraint(unsigned constrIndex,
                                        const std::string& name,
                                        const std::pair<std::string, std::string>& timeGranularity)
{
    std::string time = timeIdentifier(timeGranularity.first);
    std::string new_name = BuildName(name, timeGranularity.second, time);
    names()[constrIndex] = new_name;
}

void ConstraintNamer:: BindingConstraintHour(unsigned constrIndex, const std::string& name)
{
    BindingConstraint(constrIndex, name, {HOUR, "hourly"});
}

void ConstraintNamer:: BindingConstraintDay(unsigned constrIndex, const std::string& name)
{
    BindingConstraint(constrIndex, name, {DAY, "daily"});
}

void ConstraintNamer:: BindingConstraintWeek(unsigned constrIndex, const std::string& name)
{
    BindingConstraint(constrIndex, name, {WEEK, "weekly"});
}

void ConstraintNamer:: NbUnitsOutageLessThanNbUnitsStop(unsigned constrIndex,
                                                       const std::string& clusterName)
{
    setThermalClusterElementName(constrIndex, "nbUnitsOutageLessThanNbUnitsStop", clusterName);
}

void ConstraintNamer:: NbDispUnitsMinBoundSinceMinUpTime(unsigned constrIndex,
                                                        const std::string& clusterName)
{
    setThermalClusterElementName(constrIndex, "nbDispUnitsMinBoundSinceMinUpTime", clusterName);
}

void ConstraintNamer:: MinDownTime(unsigned constrIndex, const std::string& clusterName)
{
    setThermalClusterElementName(constrIndex, "minDownTime", clusterName);
}

void ConstraintNamer:: PMaxDispatchableGeneration(unsigned constrIndex,
                                                 const std::string& clusterName)
{
    setThermalClusterElementName(constrIndex, "pMaxDispatchableGeneration", clusterName);
}

void ConstraintNamer:: PMinDispatchableGeneration(unsigned constrIndex,
                                                 const std::string& clusterName)
{
    setThermalClusterElementName(constrIndex, "pMinDispatchableGeneration", clusterName);
}

void ConstraintNamer:: ConsistenceNODU(unsigned constrIndex, const std::string& clusterName)
{
    setThermalClusterElementName(constrIndex, "consistenceNODU", clusterName);
}

void ConstraintNamer::shortTermStorageLevel(unsigned constrIndex, const std::string& sts_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = timeIdentifier(HOUR);
    std::string name = BuildName("Level", location, time);
    names()[constrIndex] = name;
}

void ConstraintNamer:: ShortTermStorageCostVariation(const std::string& constraint_name,
                                                    unsigned constrIndex,
                                                    const std::string& sts_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">";
    std::string time = timeIdentifier(HOUR);
    std::string name = BuildName(constraint_name, location, time);
    names()[constrIndex] = name;
}

void ConstraintNamer:: ShortTermStorageCumulation(const std::string& constraint_type,
                                                 unsigned constrIndex,
                                                 const std::string& sts_name,
                                                 const std::string& constraint_name)
{
    std::string location = areaLocation() + SEP + "ShortTermStorage" + "<" + sts_name + ">" + SEP
                           + "Constraint" + "<" + constraint_name + ">";
    std::string time = timeIdentifier(WEEK);
    std::string name = BuildName(constraint_type, location, time);
    names()[constrIndex] = name;
}
