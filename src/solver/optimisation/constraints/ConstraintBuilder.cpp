// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ConstraintBuilder.h"

void ConstraintBuilder::build()
{
    if (nombreDeTermes_ > 0)
    {
        loadConstraintIntoMatrix();
    }
    nombreDeTermes_ = 0;
}

void ConstraintBuilder::addVariable(int varIndex, double coeff)
{
    if (varIndex >= 0)
    {
        data.Pi[nombreDeTermes_] = coeff;
        data.Colonne[nombreDeTermes_] = varIndex;
        nombreDeTermes_++;
    }
}

ConstraintBuilder& ConstraintBuilder::dispatchableProduction(unsigned int index,
                                                             double coeff,
                                                             int offset,
                                                             int delta)
{
    addVariable(variableManager_.dispatchableProduction(index, hourInWeek_, offset, delta), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::numberOfDispatchableUnits(unsigned int index, double coeff)
{
    addVariable(variableManager_.numberOfDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::numberStoppingDispatchableUnits(unsigned int index,
                                                                      double coeff)
{
    addVariable(variableManager_.numberStoppingDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::numberStartingDispatchableUnits(unsigned int index,
                                                                      double coeff)
{
    addVariable(variableManager_.numberStartingDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::numberBreakingDownDispatchableUnits(unsigned int index,
                                                                          double coeff)
{
    addVariable(variableManager_.numberBreakingDownDispatchableUnits(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ntcDirect(unsigned int index,
                                                double coeff,
                                                int offset,
                                                int delta)
{
    addVariable(variableManager_.ntcDirect(index, hourInWeek_, offset, delta), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::interconnectionDirectCost(unsigned int index, double coeff)
{
    addVariable(variableManager_.interconnectionDirectCost(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::interconnectionIndirectCost(unsigned int index, double coeff)
{
    addVariable(variableManager_.interconnectionIndirectCost(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::shortTermStorageInjection(unsigned int index,
                                                                double coeff,
                                                                int offset,
                                                                int delta)
{
    addVariable(variableManager_.shortTermStorageInjection(index, hourInWeek_, offset, delta),
                coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::shortTermStorageWithdrawal(unsigned int index,
                                                                 double coeff,
                                                                 int offset,
                                                                 int delta)
{
    addVariable(variableManager_.shortTermStorageWithdrawal(index, hourInWeek_, offset, delta),
                coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::shortTermStorageLevel(unsigned int index,
                                                            double coeff,
                                                            int offset,
                                                            int delta)
{
    addVariable(variableManager_.shortTermStorageLevel(index, hourInWeek_, offset, delta), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::shortTermStorageOverflow(unsigned int index,
                                                               double coeff,
                                                               int offset,
                                                               int delta)
{
    addVariable(variableManager_.shortTermStorageOverflow(index, hourInWeek_, offset, delta),
                coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::shortTermCostVariationInjection(unsigned int index,
                                                                      double coeff,
                                                                      int offset,
                                                                      int delta)
{
    addVariable(
      variableManager_.shortTermStorageCostVariationInjection(index, hourInWeek_, offset, delta),
      coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::shortTermCostVariationWithdrawal(unsigned int index,
                                                                       double coeff,
                                                                       int offset,
                                                                       int delta)
{
    addVariable(
      variableManager_.shortTermStorageCostVariationWithdrawal(index, hourInWeek_, offset, delta),
      coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::hydroPower(unsigned int index, double coeff)
{
    addVariable(variableManager_.hydroPower(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::hydroPowerDown(unsigned int index, double coeff)
{
    addVariable(variableManager_.hydroPowerDown(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::hydroPowerUp(unsigned int index, double coeff)
{
    addVariable(variableManager_.hydroPowerUp(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::pumping(unsigned int index, double coeff)
{
    addVariable(variableManager_.pumping(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::hydroLevel(unsigned int index, double coeff)
{
    addVariable(variableManager_.hydroLevel(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::overflow(unsigned int index, double coeff)
{
    addVariable(variableManager_.overflow(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::finalStorage(unsigned int index, double coeff)
{
    addVariable(variableManager_.finalStorage(index), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::unsuppliedEnergy(unsigned int index, double coeff)
{
    addVariable(variableManager_.unsuppliedEnergy(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::spillage(unsigned int index, double coeff)
{
    addVariable(variableManager_.spillage(index, hourInWeek_), coeff);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::layerStorage(unsigned area, unsigned layer, double coeff)
{
    addVariable(variableManager_.layerStorage(area, layer), coeff);
    return *this;
}

void ConstraintBuilder::loadConstraintIntoMatrix()
{
    auto& term = data.nombreDeTermesDansLaMatriceDeContrainte;

    data.IndicesDebutDeLigne[data.nombreDeContraintes] = term;
    data.CoefficientsDeLaMatriceDesContraintes.resize(term + nombreDeTermes_);
    data.IndicesColonnes.resize(term + nombreDeTermes_);

    for (int i = 0; i < nombreDeTermes_; i++)
    {
        data.CoefficientsDeLaMatriceDesContraintes[term] = data.Pi[i];
        data.IndicesColonnes[term] = data.Colonne[i];
        data.nombreDeTermesDansLaMatriceDeContrainte++;
    }
    data.NombreDeTermesDesLignes[data.nombreDeContraintes] = nombreDeTermes_;

    data.Sens[data.nombreDeContraintes] = operator_;
    data.nombreDeContraintes++;

    return;
}
