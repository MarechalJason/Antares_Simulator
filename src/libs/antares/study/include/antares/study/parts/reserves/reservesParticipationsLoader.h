/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#pragma once

#include <boost/algorithm/string.hpp>

#include <antares/inifile/inifile.h>
#include <antares/study/area/area.h>

namespace Antares::Data
{
template<typename Derived /*, typename ParticipationT*/>
class ReserveParticipationLoader
{
public:
    bool load(Area& area, const std::filesystem::path& file)
    {
        IniFile ini;
        if (!ini.open(file, false))
        {
            return false;
        }

        ini.each(
          [&](const IniFile::Section& section)
          {
              if (section.name != "symmetries")
              {
                  auto& derivedObj = derived();
                  auto [participation, clusterName] = derivedObj.readCapacityReservationSection(
                    area,
                    section);
                  derivedObj.validateCapacityInputs(participation, clusterName);
                  derivedObj.addCapacityReservation(area, section.name, participation);
              }
          });

        ini.each(
          [&](const IniFile::Section& section)
          {
              if (section.name == "symmetries")
              {
                  derived().readSymmetrySection(area, section);
              }
          });

        return true;
    }

private:
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const
    {
        return static_cast<const Derived&>(*this);
    }
};

void static throwIfNegativeValue(const std::string& propertyName,
                                 double value,
                                 const std::string& areaName,
                                 const std::optional<std::string>& clusterName,
                                 const std::string& reserveName)
{
    if (value < 0)
    {
        std::ostringstream os;
        os << "in area: " << areaName
           << (clusterName.has_value() ? ", cluster: " + clusterName.value() : "")
           << ", reservation capacity in reserve: " << reserveName << ", " << propertyName
           << " can not be negative";
        throw std::invalid_argument(os.str());
    }
}

template<typename Derived, typename ParticipationT>
class ReserveLoaderMixin: public ReserveParticipationLoader<Derived>
{
protected:
    using participation_type = ParticipationT;

    std::pair<ParticipationT, std::optional<std::string>> readCapacityReservationSection(
      const IniFile::Section& section)
    {
        participation_type rp{};
        std::optional<std::string> clusterName;

        derived().readProperties(section, clusterName, rp);
        return {rp, clusterName};
    }

    void validateCapacityInputs(const std::string& areaName,
                                const participation_type& rp,
                                const std::optional<std::string>& clusterName,
                                const std::string& reserveName)
    {
        if (clusterName.has_value() && clusterName.value().empty())
        {
            logs.error()
              << areaName
              << " : Please provide a cluster name when declaring a capacity reservation";
        }
        throwIfNegativeValue("participation-cost",
                             rp.participationCost,
                             areaName,
                             clusterName,
                             reserveName);
        derived().validateSpecificInputs(areaName, rp, clusterName, reserveName);
    }

    void addCapacityReservation(Area& area,
                                const participation_type& rp,
                                const std::string& clusterName,
                                const std::string& reserveName)
    {
        const auto* reserve = area.allCapacityReservations.value().getReserveByName(reserveName);
        const auto* cluster = derived().findCluster(area, clusterName);

        if (!reserve || !cluster)
        {
            derived().reportMissing(area, reserveName, clusterName, reserve, cluster);
            return;
        }

        rp.capacityReservation = reserve;

        auto& container = derived().getContainer(cluster);
        if (container && container.isParticipatingInReserve(reserveName))
        {
            derived().duplicateParticipation(area, clusterName, reserveName);
        }
        else
        {
            container.emplace();
        }

        container->addReserveParticipation(reserveName, rp);
    }

    void readSymmetrySection(Area& area, const IniFile::Section& section)
    {
        for (auto* p = section.firstProperty; p; p = p->next)
        {
            std::string clusterName;
            TransformNameIntoID(p->key, clusterName);

            auto symGroups = Symmetries::makeGroupsOfSymmetries(p->value);

            auto cluster = derived().findCluster(area, clusterName);
            if (!cluster)
            {
                derived().reportInvalidSymmetry(area, clusterName);
            }
            const auto* reserveContainer = derived().getContainer(cluster);
            if (!reserveContainer)
            {
                derived().reportLackOfReserveParticipation(area, clusterName);
            }

            for (auto& sym: symGroups)
            {
                reserveContainer->addReserveParticipationSymmetry(sym);
            }
        }
    }

private:
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    const Derived& derived() const
    {
        return static_cast<const Derived&>(*this);
    }
};

class ThermalReserveLoader
    : public ReserveLoaderMixin<ThermalReserveLoader, ThermalClusterReserveParticipation>
{
public:
    // lire les propriétés spécifiques au thermal
    static void readProperties(const IniFile::Section& section,
                               std::optional<std::string>& clusterName,
                               ThermalClusterReserveParticipation& rp)
    {
        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "cluster-name")
              {
                  TransformNameIntoID(p.value, clusterName.value());
              }
              else if (p.key == "participation-cost")
              {
                  p.value.to(rp.participationCost);
              }
              else if (p.key == "max-power")
              {
                  p.value.to(rp.maxPower);
              }
              else if (p.key == "max-power-off")
              {
                  p.value.to(rp.maxPowerOff);
              }
              else if (p.key == "participation-cost-off")
              {
                  p.value.to(rp.participationCostOff);
              }
              else
              {
                  logs.warning() << "invalid thermal reserve property " << p.key;
              }
          });
    }

    static void validateSpecificInputs(const std::string& areaName,
                                       const ThermalClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveName)
    {
        throwIfNegativeValue("max-power", rp.maxPower, areaName, clusterName, reserveName);
        throwIfNegativeValue("max-power-off", rp.maxPowerOff, areaName, clusterName, reserveName);
        throwIfNegativeValue("participation-cost-off",
                             rp.participationCostOff,
                             areaName,
                             clusterName,
                             reserveName);
    }

    static auto* findCluster(const Area& area, const std::string& name)
    {
        return area.thermal.list.findInAll(name);
    }

    static auto& getContainer(auto* cluster)
    {
        return cluster->reserveParticipationContainer;
    }

    static void reportMissing(const Area& area,
                              const std::string& reserveName,
                              const std::string& clusterName,
                              bool reserveOK,
                              bool clusterOK)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << ": missing reserve " << reserveName;
        }
        if (!clusterOK)
        {
            logs.error() << area.name << ": missing thermal cluster " << clusterName;
        }
    }

    static void reportInvalidSymmetry(const Area& area, const std::string& clusterName)
    {
        logs.error() << "Thermal cluster " << clusterName << " not participating to reserves of "
                     << area.name;
    }

    static void reportLackOfReserveParticipation(const Area& area, const std::string& clusterName)
    {
        throw std::out_of_range("Area " + area.name + ", " + clusterName
                                + " : trying to add symmetries without any reserve participation");
    }
};

class STStorageReserveLoader
    : public ReserveLoaderMixin<STStorageReserveLoader, StorageClusterReserveParticipation>
{
public:
    static void readProperties(const IniFile::Section& section,
                               std::optional<std::string>& clusterName,
                               StorageClusterReserveParticipation& rp)
    {
        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "cluster-name")
              {
                  TransformNameIntoID(p.value, clusterName.value());
              }
              else if (p.key == "participation-cost")
              {
                  p.value.to(rp.participationCost);
              }
              else if (p.key == "max-release")
              {
                  p.value.to(rp.maxRelease);
              }
              else if (p.key == "max-store")
              {
                  p.value.to(rp.maxStore);
              }
              else
              {
                  logs.warning() << "invalid STS reserve property " << p.key;
              }
          });
    }

    static void validateSpecificInputs(const std::string& areaName,
                                       const StorageClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveName)
    {
        throwIfNegativeValue("max-release", rp.maxRelease, areaName, clusterName, reserveName);
        throwIfNegativeValue("max-store", rp.maxStore, areaName, clusterName, reserveName);
    }

    static auto* findCluster(Area& area, const std::string& name)
    {
        return area.shortTermStorage.findInAll(name);
    }

    static auto& getContainer(auto* cluster)
    {
        return cluster->reserveParticipationContainer;
    }

    static void reportMissing(const Area& area,
                              const std::string& reserveName,
                              const std::string& clusterName,
                              bool reserveOK,
                              bool clusterOK)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << ": missing reserve " << reserveName
                         << " when loading STS reserve participation";
        }
        if (!clusterOK)
        {
            logs.error() << area.name << " : missing STStorage cluster " << clusterName
                         << " when loading STS reserve participation";
        }
    }

    static void reportInvalidSymmetry(Area& area, const std::string& clusterName)
    {
        logs.error() << "STS cluster " << clusterName
                     << " invalid or missing when adding symmetries";
    }

    static void reportLackOfReserveParticipation(const Area& area, const std::string& clusterName)
    {
        throw std::out_of_range("Area " + area.name + ", " + clusterName
                                + " : trying to add symmetries without any reserve participation");
    }
};

class HydroReserveLoader
    : public ReserveLoaderMixin<HydroReserveLoader, StorageClusterReserveParticipation>
{
public:
    static void readProperties(const Area&,
                               const IniFile::Section& section,
                               std::optional<std::string>& /*hydro: ignore clusterName*/,
                               StorageClusterReserveParticipation& rp)
    {
        section.each(
          [&](const IniFile::Property& p)
          {
              if (p.key == "participation-cost")
              {
                  p.value.to(rp.participationCost);
              }
              else if (p.key == "max-release")
              {
                  p.value.to(rp.maxRelease);
              }
              else if (p.key == "max-store")
              {
                  p.value.to(rp.maxStore);
              }
              else
              {
                  logs.warning() << "invalid hydro reserve property " << p.key;
              }
          });
    }

    static void validateSpecificInputs(const std::string& areaName,
                                       const StorageClusterReserveParticipation& rp,
                                       const std::string& clusterName,
                                       const std::string& reserveName)
    {
        throwIfNegativeValue("max-release", rp.maxRelease, areaName, clusterName, reserveName);
        throwIfNegativeValue("max-store", rp.maxStore, areaName, clusterName, reserveName);
    }

    static auto* findCluster(Area& area, const std::string&)
    {
        return &area.hydro; // unique cluster
    }

    static auto& getContainer(auto* hydro)
    {
        return hydro->reserveParticipationContainer;
    }

    static void reportMissing(const Area& area,
                              const std::string& reserveName,
                              const std::string&,
                              bool reserveOK,
                              bool)
    {
        if (!reserveOK)
        {
            logs.error() << area.name << ": missing reserve " << reserveName;
        }
    }

    void reportInvalidSymmetry(Area& area, const std::string& clusterName)
    {
        logs.error() << area.name << " : invalid hydro symmetry cluster " << clusterName;
    }
};

} // namespace Antares::Data
