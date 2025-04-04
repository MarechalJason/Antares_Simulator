#include "antares/solver/optimisation/constraints/SymmetryReserveParticipation.h"

template<ValidReserveParticipation T>
void SymmetryReserveParticipation::add(
  int pays,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>>& symmetry,
  int pdt)
{
    if (!data.Simulation)
    {
        auto& reserveParticipationRefWithName = symmetry[0];

        for (size_t i = 1; i < symmetry.size(); ++i)
        {
            auto& reserveParticipationWithName = symmetry[i];

            applyReserveParticipationSymmetry<T>(pdt,
                                                 reserveParticipationRefWithName,
                                                 reserveParticipationWithName);
            builder.equalTo().build();

            setupConstraintNamer(pays,
                                 pdt,
                                 reserveParticipationRefWithName,
                                 reserveParticipationWithName);
        }
    }
    else
    {
        builder.data.nombreDeContraintes += symmetry.size() - 1;
    }
}

template<ValidReserveParticipation T>
void SymmetryReserveParticipation::applyReserveParticipationSymmetry(
  int pdt,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationRefWithName,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationWithName)
{
    builder.updateHourWithinWeek(pdt);
    auto refIndex = reserveParticipationRefWithName.reserveParticipation.get()
                      .globalIndexClusterParticipation;
    auto targetIndex = reserveParticipationWithName.reserveParticipation.get()
                         .globalIndexClusterParticipation;

    if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_THERMAL>)
    {
        builder
          .ThermalClusterReserveParticipation(
            refIndex,
            1 / reserveParticipationRefWithName.reserveParticipation.get().maxPower)
          .ThermalClusterReserveParticipation(
            targetIndex,
            -1 / reserveParticipationWithName.reserveParticipation.get().maxPower);
    }
    else if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_STSTORAGE>)
    {
        builder
          .STStorageTurbiningClusterReserveParticipation(
            refIndex,
            1 / reserveParticipationRefWithName.reserveParticipation.get().maxTurbining)
          .STStorageTurbiningClusterReserveParticipation(
            targetIndex,
            -1 / reserveParticipationWithName.reserveParticipation.get().maxTurbining)
          .STStoragePumpingClusterReserveParticipation(
            refIndex,
            1 / reserveParticipationRefWithName.reserveParticipation.get().maxPumping)
          .STStoragePumpingClusterReserveParticipation(
            targetIndex,
            -1 / reserveParticipationWithName.reserveParticipation.get().maxPumping);
    }
    else if constexpr (std::is_same_v<T, RESERVE_PARTICIPATION_LTSTORAGE>)
    {
        builder
          .LTStorageTurbiningClusterReserveParticipation(
            refIndex,
            1 / reserveParticipationRefWithName.reserveParticipation.get().maxTurbining)
          .LTStorageTurbiningClusterReserveParticipation(
            targetIndex,
            -1 / reserveParticipationWithName.reserveParticipation.get().maxTurbining)
          .LTStoragePumpingClusterReserveParticipation(
            refIndex,
            1 / reserveParticipationRefWithName.reserveParticipation.get().maxPumping)
          .LTStoragePumpingClusterReserveParticipation(
            targetIndex,
            -1 / reserveParticipationWithName.reserveParticipation.get().maxPumping);
    }
}

template<ValidReserveParticipation T>
void SymmetryReserveParticipation::setupConstraintNamer(
  int pays,
  int pdt,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationRefWithName,
  const RESERVE_PARTICIPATION_WITH_RESERVE_NAME<T>& reserveParticipationWithName)
{
    ConstraintNamer namer(builder.data.NomDesContraintes);
    const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;

    namer.UpdateTimeStep(hourInTheYear);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.SymmetryReserveParticipation(
      builder.data.nombreDeContraintes,
      reserveParticipationRefWithName.reserveParticipation.get().clusterName,
      reserveParticipationRefWithName.reserveName,
      reserveParticipationWithName.reserveName);
}

template void SymmetryReserveParticipation::add<RESERVE_PARTICIPATION_THERMAL>(
  int,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<RESERVE_PARTICIPATION_THERMAL>>&,
  int);

template void SymmetryReserveParticipation::add<RESERVE_PARTICIPATION_STSTORAGE>(
  int,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<RESERVE_PARTICIPATION_STSTORAGE>>&,
  int);

template void SymmetryReserveParticipation::add<RESERVE_PARTICIPATION_LTSTORAGE>(
  int,
  const std::vector<RESERVE_PARTICIPATION_WITH_RESERVE_NAME<RESERVE_PARTICIPATION_LTSTORAGE>>&,
  int);
