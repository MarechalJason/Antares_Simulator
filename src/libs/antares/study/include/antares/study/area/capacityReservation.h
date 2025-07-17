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

#include <optional>
#include <set>

#include <antares/series/series.h>

//! Name of a reserve
using ReserveName = std::string;

/// @brief Represents an area capacity reservation using it's name, it's failure cost and it's
/// spillage cost
struct CapacityReservation
{
    CapacityReservation():
        need(timeseriesNumbers)
    {
    }

    double failureCost = 0.;
    double spillageCost = 0.;
    double maxActivationRatio = 0.;
    double maxEnergyActivationRatio = 1.;
    int maxActivationHours = 1.;
    Antares::Data::TimeSeries need;

private:
    Data::TimeSeriesNumbers timeseriesNumbers;
};

/// @brief Stores all the Capacity reservations in two maps for the up and down reserves
struct AllCapacityReservations
{
    double maxGlobalEnergyActivationRatioUp = 1.;
    double maxGlobalEnergyActivationRatioDown = 1.;
    int maxGlobalActivationDurationUp = 1;
    int maxGlobalActivationDurationDown = 1;
    std::map<std::string /*reserveName*/, std::set<std::string /*name of the group*/>>
      reserveGroupPart;

    std::map<ReserveName, CapacityReservation> areaCapacityReservationsUp;
    std::map<ReserveName, CapacityReservation> areaCapacityReservationsDown;

    /// @brief Check if the capacity reservation name already exist in both the up and down reserves
    /// @param name
    /// @return true if the capacity reservation already existed
    bool contains(ReserveName name) const
    {
        return areaCapacityReservationsUp.contains(name)
               || areaCapacityReservationsDown.contains(name);
    }

    /// @brief Get a capacity reservation from both the up and down reserves using its name
    /// @param name
    /// @return an optional of the capacity reservation reference if the reserve was found, and a
    /// nullopt otherwise
    std::optional<std::reference_wrapper<const CapacityReservation>> getReserveByName(
      std::string name) const
    {
        if (areaCapacityReservationsUp.contains(name))
        {
            return areaCapacityReservationsUp.at(name);
        }
        else if (areaCapacityReservationsDown.contains(name))
        {
            return areaCapacityReservationsDown.at(name);
        }
        return std::nullopt;
    }

    /// @brief Get the number of capacityReservations in the area
    /// @return the number of capacityReservations in the area
    size_t size() const
    {
        return areaCapacityReservationsUp.size() + areaCapacityReservationsDown.size();
    }

    /// @brief Returns lower case, no space string
    /// @param name
    /// @return A string usable for file naming
    static std::string toFilename(std::string name)
    {
        std::string file_name = name;
        std::replace(file_name.begin(), file_name.end(), ' ', '_');
        std::transform(file_name.begin(), file_name.end(), file_name.begin(), ::tolower);
        return file_name;
    }
};
