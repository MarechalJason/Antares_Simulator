#include "VariableManagement.h"

using namespace VariableManagement;

VariableManager::VariableManager(
  std::vector<CORRESPONDANCES_DES_VARIABLES>& CorrespondanceVarNativesVarOptim,
  std::vector<int>& NumeroDeVariableStockFinal,
  std::vector<std::vector<int>>& NumeroDeVariableDeTrancheDeStock,
  const int32_t& NombreDePasDeTempsPourUneOptimisation):
    CorrespondanceVarNativesVarOptim_(CorrespondanceVarNativesVarOptim),
    NumeroDeVariableStockFinal_(NumeroDeVariableStockFinal),
    NumeroDeVariableDeTrancheDeStock_(NumeroDeVariableDeTrancheDeStock),
    NombreDePasDeTempsPourUneOptimisation_(NombreDePasDeTempsPourUneOptimisation)
{
}

int VariableManager::getShiftedTimeStep(int offset, int delta, unsigned int hourInWeek) const
{
    int pdt = hourInWeek + offset;
    const int nbTimeSteps = NombreDePasDeTempsPourUneOptimisation_;

    if (offset != 0)
    {
        if (offset >= 0)
        {
            pdt = pdt % nbTimeSteps;
        }
        else
        {
            pdt = (pdt + delta) % nbTimeSteps;
        }
    }
    return pdt;
}

int& VariableManager::dispatchableProduction(unsigned int index,
                                             unsigned int hourInWeek,
                                             int offset,
                                             int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);

    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDuPalierThermique[index];
}

int& VariableManager::numberOfDispatchableUnits(unsigned int index,
                                                unsigned int hourInWeek,
                                                int offset,
                                                int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[index];
}

int& VariableManager::numberStoppingDispatchableUnits(unsigned int index,
                                                      unsigned int hourInWeek,
                                                      int offset,
                                                      int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[index];
}

int& VariableManager::numberStartingDispatchableUnits(unsigned int index,
                                                      unsigned int hourInWeek,
                                                      int offset,
                                                      int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[index];
}

int& VariableManager::numberBreakingDownDispatchableUnits(unsigned int index,
                                                          unsigned int hourInWeek,
                                                          int offset,
                                                          int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[index];
}

int& VariableManager::ntcDirect(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDeLInterconnexion[index];
}

int& VariableManager::interconnectionDirectCost(unsigned int index,
                                        unsigned int hourInWeek,
                                        int offset,
                                        int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[index];
}

int& VariableManager::interconnectionIndirectCost(unsigned int index,
                                          unsigned int hourInWeek,
                                          int offset,
                                          int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[index];
}

int& VariableManager::shortTermStorageInjection(unsigned int index,
                                                unsigned int hourInWeek,
                                                int offset,
                                                int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].SIM_ShortTermStorage.InjectionVariable[index];
}

int& VariableManager::shortTermStorageWithdrawal(unsigned int index,
                                                 unsigned int hourInWeek,
                                                 int offset,
                                                 int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].SIM_ShortTermStorage.WithdrawalVariable[index];
}

int& VariableManager::shortTermStorageLevel(unsigned int index,
                                            unsigned int hourInWeek,
                                            int offset,
                                            int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].SIM_ShortTermStorage.LevelVariable[index];
}

int& VariableManager::shortTermStorageOverflow(unsigned int index,
                                               unsigned int hourInWeek,
                                               int offset,
                                               int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].SIM_ShortTermStorage.OverflowVariable[index];
}

int& VariableManager::shortTermStorageCostVariationInjection(unsigned int index,
                                                             unsigned int hourInWeek,
                                                             int offset,
                                                             int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .SIM_ShortTermStorage.CostVariationInjection[index];
}

int& VariableManager::shortTermStorageCostVariationWithdrawal(unsigned int index,
                                                              unsigned int hourInWeek,
                                                              int offset,
                                                              int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt]
      .SIM_ShortTermStorage.CostVariationWithdrawal[index];
}

int& VariableManager::hydroPower(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDeLaProdHyd[index];
}

int& VariableManager::hydroPowerDown(unsigned int index,
                                  unsigned int hourInWeek,
                                  int offset,
                                  int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesVariationHydALaBaisse[index];
}

int& VariableManager::hydroPowerUp(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesVariationHydALaHausse[index];
}

int& VariableManager::pumping(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDePompage[index];
}

int& VariableManager::hydroLevel(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDeNiveau[index];
}

int& VariableManager::overflow(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariablesDeDebordement[index];
}

int& VariableManager::finalStorage(unsigned int index)
{
    return NumeroDeVariableStockFinal_[index];
}

int& VariableManager::layerStorage(unsigned area, unsigned layer)
{
    return NumeroDeVariableDeTrancheDeStock_[area][layer];
}

int& VariableManager::unsuppliedEnergy(unsigned int index,
                                       unsigned int hourInWeek,
                                       int offset,
                                       int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDefaillancePositive[index];
}

int& VariableManager::spillage(unsigned int index, unsigned int hourInWeek, int offset, int delta)
{
    auto pdt = getShiftedTimeStep(offset, delta, hourInWeek);
    return CorrespondanceVarNativesVarOptim_[pdt].NumeroDeVariableDefaillanceNegative[index];
}
