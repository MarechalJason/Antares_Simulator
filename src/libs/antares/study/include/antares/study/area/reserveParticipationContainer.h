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
#include <antares/study/area/reserveParticipationTraits.h>

template<typename T>
concept DerivedFromReserveParticipationBase = std::is_base_of_v<ReserveParticipationBase, T>;

/// @brief Container for reserve participation generic for each type of cluster (thermal, Short
/// term, Long term)
template<DerivedFromReserveParticipationBase T>
class ReserveParticipationContainerBase
{
    struct ReserveParticipationWithName
    {
        T* reserveParticipation;
        ReserveName reserveName;
    };

protected:
    std::map<ReserveName, T> reservesParticipations;
    std::vector<std::vector<ReserveParticipationWithName>> reserveParticipationsSymmetries;

public:
    /// @brief Add a reserve participation to the container for a given reserve name
    /// @param reserveName name of the reserve for which the participation is added
    /// @param reserveParticipation reserve participation to add
    void addReserveParticipation(ReserveName reserveName, const T& reserveParticipation)
    {
        reservesParticipations.emplace(reserveName, reserveParticipation);
    }

    /// @brief Add a reserve participation symmetry to the container
    /// @param names names of the reserves for which the participation is symmetrical
    void addReserveParticipationSymmetry(std::set<ReserveName> names)
    {
        auto symmetryRes = std::vector<ReserveParticipationWithName>();
        for (auto name: names)
        {
            if (reservesParticipations.contains(name))
            {
                symmetryRes.push_back(
                  ReserveParticipationWithName{&reservesParticipations.at(name), name});
            }
            else
            {
                throw std::out_of_range("This entity is not participating to reserve " + name);
            }
        }
        reserveParticipationsSymmetries.push_back(symmetryRes);
    }

    /// @brief Get the indices of the lists that contains reserveParticipation to the reserve name
    /// @param name name of the reserve for which the participation is searched
    /// @return indices of the lists that contains reserveParticipation to the reserve name
    std::vector<int> symmetricalIndices(ReserveName name) const
    {
        std::vector<int> indices;
        for (int i = 0; i < reserveParticipationsSymmetries.size(); i++)
        {
            for (auto reserveParticipation: reserveParticipationsSymmetries.at(i))
            {
                if (reserveParticipation.reserveName == name)
                {
                    indices.push_back(i);
                }
            }
        }

        return indices;
    }

    /// @brief Returns the number of symmetrical groups
    /// @return the number of symmetrical groups
    int getNbSymGroups() const
    {
        return reserveParticipationsSymmetries.size();
    }

    /// @brief Returns the number of reserves participations in the container
    std::size_t reserveParticipationsCount() const
    {
        return reservesParticipations.size();
    }

    /// @brief Returns true if cluster participates in a reserve with this name
    /// @param name name of the reserve
    /// @return true if cluster participates in a reserve with this name
    bool isParticipatingInReserve(ReserveName name) const
    {
        return reservesParticipations.contains(name);
    }

protected:
    /// @brief Returns the value of a member of the reserve participation struct for a given reserve
    /// name
    /// @param name name of the reserve
    /// @param member member of the reserve participation struct
    /// @return the value of the member
    double getValue(ReserveName name, double T::* member) const
    {
        if (isParticipatingInReserve(name))
        {
            return reservesParticipations.at(name).*member;
        }
        else
        {
            throw std::out_of_range("reserve " + name + " not found in cluster participations");
        }
    }
};

/// @brief Container for reserve participation specific to a cluster type
/// @tparam T type of the cluster
/// @details The methods to access the fields are only callable if the cluster type traits have the
/// corresponding field
template<typename T>
class ReserveParticipationContainer: public ReserveParticipationContainerBase<T>
{
    using Base = ReserveParticipationContainerBase<T>;

public:
    using Base::reservesParticipations;

    /// @brief Returns the cost of participation in a reserve
    /// @param name name of the reserve
    /// @return the cost of participation in the reserve
    double reserveCost(ReserveName name) const
    {
        return this->getValue(name, ReserveParticipationTraits<T>::participationCost);
    }

    /// @brief Returns the cost of participation in a reserve when the cluster is off
    /// @param name name of the reserve
    /// @return the cost of participation in the reserve when the cluster is off
    double reserveCostOff(ReserveName name) const
    {
        return this->getValue(name, ReserveParticipationTraits<T>::participationCostOff);
    }

    /// @brief Returns the maximum power that can be reserved when the cluster is off
    /// @param name name of the reserve
    /// @return the maximum power that can be reserved when the cluster is off
    double reserveMaxPowerOff(ReserveName name) const
    {
        return this->getValue(name, ReserveParticipationTraits<T>::maxPowerOff);
    }

    /// @brief Returns the maximum power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum power that can be reserved
    double reserveMaxPower(ReserveName name) const
    {
        return this->getValue(name, ReserveParticipationTraits<T>::maxPower);
    }

    /// @brief Returns the maximum turbining power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum turbining power that can be reserved
    double reserveMaxTurbining(ReserveName name) const
    {
        return this->getValue(name, ReserveParticipationTraits<T>::maxTurbining);
    }

    /// @brief Returns the maximum pumping power that can be reserved
    /// @param name name of the reserve
    /// @return the maximum pumping power that can be reserved
    double reserveMaxPumping(ReserveName name) const
    {
        return this->getValue(name, ReserveParticipationTraits<T>::maxPumping);
    }
};
