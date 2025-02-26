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

#include "antares/study/parts/short-term-storage/container.h"

#include <algorithm>
#include <numeric>
#include <string>

#include <antares/utils/utils.h>
#include "antares/study/parts/short-term-storage/container.h"
#include "antares/study/study.h"

#define SEP Yuni::IO::Separator

namespace fs = std::filesystem;

namespace Antares::Data::ShortTermStorage
{
bool STStorageInput::validate() const
{
    return std::ranges::all_of(storagesByIndex, [](auto& cluster) { return cluster.validate(); });
}

bool STStorageInput::createSTStorageClustersFromIniFile(const fs::path& path)
{
    const fs::path pathIni = path / "list.ini";
    IniFile ini;
    if (!ini.open(pathIni))
    {
        return false;
    }

    if (!ini.firstSection)
    {
        return true;
    }

    logs.debug() << "  :: loading `" << pathIni << "`";

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        STStorageCluster cluster;
        if (!cluster.loadFromSection(*section))
        {
            return false;
        }
        cluster.properties.clusterGlobalIndex = storagesByIndex.size();
        storagesByIndex.push_back(cluster);
    }

    std::ranges::sort(storagesByIndex,
                      [](const auto& a, const auto& b)
                      { return a.properties.name < b.properties.name; });

    return true;
}

bool STStorageInput::LoadConstraintsFromIniFile(const fs::path& parent_path)
{
    IniFile ini;
    const auto pathIni = parent_path / "additional-constraints.ini";
    if (!ini.open(pathIni, false))
    {
        logs.info() << "There is no: " << pathIni;
        return true;
    }

    for (auto* section = ini.firstSection; section; section = section->next)
    {
        AdditionalConstraint constraint;
        constraint.name = section->name.c_str();
        for (auto* property = section->firstProperty; property; property = property->next)
        {
            const std::string key = property->key;
            const auto value = property->value;

            if (key == "cluster")
            {
                // TODO do i have to transform the name to id? TransformNameIntoID
                std::string clusterName;
                value.to<std::string>(clusterName);
                constraint.cluster_id = transformNameIntoID(clusterName);
            }
            else if (key == "variable")
            {
                value.to<std::string>(constraint.variable);
            }
            else if (key == "operator")
            {
                value.to<std::string>(constraint.operatorType);
            }
            else if (key == "hours")
            {
                std::stringstream ss(value.c_str());
                std::string hour;
                while (std::getline(ss, hour, ','))
                {
                    int hourVal = std::stoi(hour);
                    constraint.hours.insert(hourVal);
                }
            }
            else if (key == "rhs")
            {
                property->value.to<double>(constraint.rhs);
            }
        }

        if (auto ret = constraint.validate(); !ret.ok)
        {
            logs.error() << "Invalid constraint in section: " << section->name;
            logs.error() << ret.error_msg;
            return false;
        }

        auto it = std::find_if(storagesByIndex.begin(),
                               storagesByIndex.end(),
                               [&constraint](const STStorageCluster& cluster)
                               { return cluster.id == constraint.cluster_id; });
        if (it == storagesByIndex.end())
        {
            logs.warning() << " from file " << pathIni;
            logs.warning() << "Constraint " << section->name
                           << " does not reference an existing cluster";
            return false;
        }
        else
        {
            it->additional_constraints.push_back(constraint);
        }
    }

    return true;
}

bool STStorageInput::loadSeriesFromFolder(const fs::path& folder) const
{
    if (folder.empty())
    {
        return false;
    }

    bool ret = true;

    for (auto& cluster: storagesByIndex)
    {
        fs::path seriesFolder = folder / cluster.id;
        ret = cluster.loadSeries(seriesFolder) && ret;
    }

    return ret;
}

bool STStorageInput::loadReserveParticipations(Area& area, const std::filesystem::path& file)
{
    IniFile ini;
    if (!ini.open(file, false))
    {
        return false;
    }

    std::vector<std::reference_wrapper<const IniFile::Section>> clustersSections;
    std::vector<std::reference_wrapper<const IniFile::Section>> symmetriesSections;

    // Collect sections into clustersSections and symmetriesSections (because we need to process
    // symmetries after reserve participations)
    ini.each(
      [&](const IniFile::Section& section)
      {
          if (section.name == "symmetries")
          {
              symmetriesSections.emplace_back(section);
          }
          else
          {
              clustersSections.emplace_back(section);
          }
      });

    // Process clusters reserves participations
    for (const auto& section: clustersSections)
    {
        std::string tmpClusterName;
        float tmpMaxTurbining = 0;
        float tmpMaxPumping = 0;
        float tmpParticipationCost = 0;

        for (auto* p = section.get().firstProperty; p; p = p->next)
        {
            auto tmp = p->key;
            tmp.toLower();

            if (tmp == "cluster-name")
            {
                TransformNameIntoID(p->value, tmpClusterName);
            }
            else if (tmp == "max-turbining")
            {
                if (!p->value.to<float>(tmpMaxTurbining))
                {
                    logs.warning() << area.name << ": invalid max turbining power for reserve "
                                   << section.get().name;
                }
            }
            else if (tmp == "max-pumping")
            {
                if (!p->value.to<float>(tmpMaxPumping))
                {
                    logs.warning() << area.name << ": invalid max pumping power for reserve "
                                   << section.get().name;
                }
            }
            else if (tmp == "participation-cost")
            {
                if (!p->value.to<float>(tmpParticipationCost))
                {
                    logs.warning() << area.name << ": invalid participation cost for reserve "
                                   << section.get().name;
                }
            }
        }

        auto reserve = area.allCapacityReservations().getReserveByName(section.get().name);
        auto cluster = getClusterByName(tmpClusterName);
        if (reserve && cluster)
        {
            STStorageClusterReserveParticipation tmpReserveParticipation{reserve.value(),
                                                                         tmpMaxTurbining,
                                                                         tmpMaxPumping,
                                                                         tmpParticipationCost};
            cluster.value().get().addReserveParticipation(section.get().name,
                                                          tmpReserveParticipation);
        }
        else
        {
            if (!reserve)
            {
                logs.warning() << area.name << ": does not contain this reserve "
                               << section.get().name;
            }
            if (!cluster)
            {
                logs.warning() << area.name << ": does not contain this cluster " << tmpClusterName;
            }
        }
    }

    // Process symmetries
    for (const auto& section: symmetriesSections)
    {
        for (auto* p = section.get().firstProperty; p; p = p->next)
        {
            std::string tmpClusterName;
            TransformNameIntoID(p->key, tmpClusterName);
            auto symmetries = Antares::parseStringToVectorOfVectorOfStrings(p->value);
            for (auto& sym: symmetries)
            {
                auto cluster = area.shortTermStorage.getClusterByName(tmpClusterName);
                if (cluster)
                {
                    cluster.value().get().addReserveParticipationSymmetry(sym);
                }
                else
                {
                    logs.warning() << "Short-term storage cluster " << tmpClusterName
                                   << " is not participating to reserves of area " << area.name;
                }
            }
        }
    }
    return true;
}

bool STStorageInput::saveToFolder(const std::string& folder) const
{
    // create empty list.ini if there's no sts in this area
    Yuni::IO::Directory::Create(folder);
    const std::string pathIni(folder + SEP + "list.ini");
    IniFile ini;

    logs.debug() << "saving file " << pathIni;
    std::ranges::for_each(storagesByIndex,
                          [&ini](auto& storage) { return storage.saveProperties(ini); });

    return ini.save(pathIni);
}

bool STStorageInput::saveDataSeriesToFolder(const std::string& folder) const
{
    Yuni::IO::Directory::Create(folder);
    return std::ranges::all_of(storagesByIndex,
                               [&folder](auto& storage)
                               { return storage.saveSeries(folder + SEP + storage.id); });
}

std::size_t STStorageInput::cumulativeConstraintCount() const
{
    return std::accumulate(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           0,
                           [](int acc, const auto& cluster)
                           { return acc + cluster.additional_constraints.size(); });
}

std::size_t STStorageInput::count() const
{
    return std::ranges::count_if(storagesByIndex,
                                 [](const STStorageCluster& st) { return st.properties.enabled; });
}

uint STStorageInput::removeDisabledClusters()
{
    return std::erase_if(storagesByIndex, [](const auto& c) { return !c.enabled(); });
}

std::pair<Data::ClusterName, Data::ReserveName> STStorageInput::reserveParticipationClusterAt(
  const Area* area,
  unsigned int index) const
{
    int globalReserveParticipationIdx = 0;

    for (const auto& [reserveUpName, _]: area->allCapacityReservations().areaCapacityReservationsUp)
    {
        for (auto& cluster: storagesByIndex)
        {
            if (cluster.clusterReservesParticipations().find(reserveUpName)
                != cluster.clusterReservesParticipations().end())
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster.id, reserveUpName};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    for (const auto& [reserveDownName, _]:
         area->allCapacityReservations().areaCapacityReservationsDown)
    {
        for (auto& cluster: storagesByIndex)
        {
            if (cluster.clusterReservesParticipations().find(reserveDownName)
                != cluster.clusterReservesParticipations().end())
            {
                if (globalReserveParticipationIdx == index)
                {
                    return {cluster.id, reserveDownName};
                }
                globalReserveParticipationIdx++;
            }
        }
    }

    throw std::out_of_range("This cluster reserve participation index has not been found in all "
                            "the reserve participations");
}

std::pair<Data::ShortTermStorage::Group, Data::ReserveName>
STStorageInput::reserveParticipationGroupAt(const Area* area, unsigned int index) const
{
    int column = 0;
    for (const auto& [reserveName, _]: area->allCapacityReservations().areaCapacityReservationsUp)
    {
        for (int indexGroup = 0; indexGroup < Data::ShortTermStorage::groupMax; indexGroup++)
        {
            if (column == index)
            {
                return {static_cast<Data::ShortTermStorage::Group>(indexGroup), reserveName};
            }
            column++;
        }
    }
    for (const auto& [reserveName, _]: area->allCapacityReservations().areaCapacityReservationsDown)
    {
        for (int indexGroup = 0; indexGroup < Data::ShortTermStorage::groupMax; indexGroup++)
        {
            if (column == index)
            {
                return {static_cast<Data::ShortTermStorage::Group>(indexGroup), reserveName};
            }
            column++;
        }
    }
    throw std::out_of_range("This group reserve participation index has not been found in all the "
                            "reserve participations");
}

std::optional<std::reference_wrapper<STStorageCluster>> STStorageInput::getClusterByName(
  const std::string& name)
{
    auto it = std::find_if(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           [&name](STStorageCluster& cluster) { return cluster.id == name; });
    if (it != storagesByIndex.end())
    {
        return std::ref(*it);
    }
    else
    {
        return std::nullopt;
    }
}

size_t STStorageInput::getClusterIdx(STStorageCluster& cluster)
{
    auto it = std::find_if(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           [&cluster](STStorageCluster& elem) { return &elem == &cluster; });
    if (it != storagesByIndex.end())
    {
        return std::distance(storagesByIndex.begin(), it);
    }
    else
    {
        throw std::out_of_range("This Short Term Storage is not in the list");
    }
}

uint STStorageInput::reserveParticipationsCount()
{
    return std::accumulate(storagesByIndex.begin(),
                           storagesByIndex.end(),
                           0,
                           [](int total, STStorageCluster& cluster)
                           { return total + cluster.reserveParticipationsCount(); });
}
} // namespace Antares::Data::ShortTermStorage
