#pragma once

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace VariableManagement
{

/*!
Factory class that hold variables indices
*/
class VariableManager final
{
public:
    VariableManager(std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
                    std::vector<int>& NumeroDeVariableStockFinal,
                    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
                    const int32_t& NombreDePasDeTempsPourUneOptimisation);

    int& dispatchableProduction(unsigned int index,
                                unsigned int hourInWeek,
                                int offset = 0,
                                int delta = 0);

    int& numberOfDispatchableUnits(unsigned int index,
                                   unsigned int hourInWeek,
                                   int offset = 0,
                                   int delta = 0);

    int& numberStoppingDispatchableUnits(unsigned int index,
                                         unsigned int hourInWeek,
                                         int offset = 0,
                                         int delta = 0);
    int& numberStartingDispatchableUnits(unsigned int index,
                                         unsigned int hourInWeek,
                                         int offset = 0,
                                         int delta = 0);

    int& numberBreakingDownDispatchableUnits(unsigned int index,
                                             unsigned int hourInWeek,
                                             int offset = 0,
                                             int delta = 0);

    int& ntcDirect(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& interconnectionDirectCost(unsigned int index,
                           unsigned int hourInWeek,
                           int offset = 0,
                           int delta = 0);

    int& interconnectionIndirectCost(unsigned int index,
                             unsigned int hourInWeek,
                             int offset = 0,
                             int delta = 0);

    int& shortTermStorageInjection(unsigned int index,
                                   unsigned int hourInWeek,
                                   int offset = 0,
                                   int delta = 0);

    int& shortTermStorageWithdrawal(unsigned int index,
                                    unsigned int hourInWeek,
                                    int offset = 0,
                                    int delta = 0);

    int& shortTermStorageLevel(unsigned int index,
                               unsigned int hourInWeek,
                               int offset = 0,
                               int delta = 0);

    int& shortTermStorageOverflow(unsigned int index,
                                  unsigned int hourInWeek,
                                  int offset = 0,
                                  int delta = 0);

    int& shortTermStorageCostVariationInjection(unsigned int index,
                                                unsigned int hourInWeek,
                                                int offset = 0,
                                                int delta = 0);
    int& shortTermStorageCostVariationWithdrawal(unsigned int index,
                                                 unsigned int hourInWeek,
                                                 int offset = 0,
                                                 int delta = 0);

    int& hydroPower(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& hydroPowerDown(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& hydroPowerUp(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& pumping(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& hydroLevel(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& overflow(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

    int& finalStorage(unsigned int index);

    int& layerStorage(unsigned area, unsigned layer);

    int& unsuppliedEnergy(unsigned int index,
                          unsigned int hourInWeek,
                          int offset = 0,
                          int delta = 0);

    int& spillage(unsigned int index, unsigned int hourInWeek, int offset = 0, int delta = 0);

private:
    std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim_;
    std::vector<int>& NumeroDeVariableStockFinal_;
    std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock_;
    const int32_t& NombreDePasDeTempsPourUneOptimisation_;
    int getShiftedTimeStep(int offset, int delta, unsigned int hourInWeek) const;
};

} // namespace VariableManagement
