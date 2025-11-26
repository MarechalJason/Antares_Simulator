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

#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <vector>

#include <antares/study/fwd.h>

namespace fs = std::filesystem;

//! Name of a reserve
using ReserveName = std::string;

/// @brief Represents an area capacity reservation using it's name, it's failure cost and it's
/// spillage cost
struct CapacityReservation
{
    reserve::Type direction{reserve::Type::DOWN};
    double unsuppliedCost = 0.;
    double spillageCost = 0.;
    double powerActivationRatio = 0.;
    double energyActivationRatio = 1.;
    int referenceActivationHours = 1.;
    std::vector<double> need = {};

    void loadNeedFromFile(const fs::path& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open " + path.string());
        }
        double x;

        while (file >> x)
        {
            need.push_back(x);
        }
    }
};

/// @brief Stores all the Capacity reservations in two maps for the up and down reserves
struct AllCapacityReservations
{
    reserve::ReserveDirectionData<double> maxGlobalEnergyActivationRatio{1., 1.};
    reserve::ReserveDirectionData<int> referenceGlobalActivationDuration{1, 1};

    std::map<std::string /*reserveName*/, std::set<std::string /*name of the group*/>>
      reserveGroupPartThermal;
    std::map<std::string /*reserveName*/, std::set<std::string /*name of the group*/>>
      reserveGroupPartSTS;

    std::map<ReserveName, CapacityReservation> areaCapacityReservations;

    /// @brief Check if the capacity reservation name already exist in the reserves
    /// @param name
    /// @return true if the capacity reservation already existed
    bool contains(ReserveName name) const
    {
        return areaCapacityReservations.contains(name);
    }

    /// @brief Get a capacity reservation from both the up and down reserves using its name
    /// @param name
    /// @return the capacity reservation reference if the reserve was found, and a nullptr
    /// otherwise
    const CapacityReservation* getReserveByName(std::string name) const
    {
        if (areaCapacityReservations.contains(name))
        {
            return &areaCapacityReservations.at(name);
        }
        return nullptr;
    }

    /// @brief Get the number of capacityReservations in the area
    /// @return the number of capacityReservations in the area
    size_t size() const
    {
        return areaCapacityReservations.size();
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
