/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <antares/study/area/reserveParticipation.h>

/// @brief  Traits for reserve participation used to access the data members of the reserve
/// participation structures
/// @tparam T The type of the reserve participation structure
template<typename T>
struct ReserveParticipationTraits;

/// @brief  Traits for thermal cluster reserve participation
template<>
struct ReserveParticipationTraits<ThermalClusterReserveParticipation>
{
    static constexpr auto participationCost = &ThermalClusterReserveParticipation::
                                                participationCost;
    static constexpr auto participationCostOff = &ThermalClusterReserveParticipation::
                                                   participationCostOff;
    static constexpr auto maxPowerOff = &ThermalClusterReserveParticipation::maxPowerOff;
    static constexpr auto maxPower = &ThermalClusterReserveParticipation::maxPower;
};

/// @brief  Traits for storage cluster or hydro reserve participation
template<>
struct ReserveParticipationTraits<StorageClusterReserveParticipation>
{
    static constexpr auto participationCost = &StorageClusterReserveParticipation::
                                                participationCost;
    static constexpr auto maxTurbining = &StorageClusterReserveParticipation::maxTurbining;
    static constexpr auto maxPumping = &StorageClusterReserveParticipation::maxPumping;
};
