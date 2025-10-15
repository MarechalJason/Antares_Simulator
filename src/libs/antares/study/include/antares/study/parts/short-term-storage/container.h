/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <string>

#include <antares/study/fwd.h>
#include <antares/study/version.h>

#include "cluster.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageInput final
{
public:
    bool validate(StudyVersion studyVersion) const;

    /// 1. Read list.ini
    bool createSTStorageClustersFromIniFile(const std::filesystem::path& path);

    /// 2. Read ALL series
    bool loadSeriesFromFolder(const std::filesystem::path& folder, StudyVersion studyVersion) const;

    bool loadReserveParticipations(Area& area, const std::filesystem::path& file);

    /// Number of enabled ST storages, ignoring disabled ST storages
    std::size_t count() const;

    bool loadAdditionalConstraints(const std::filesystem::path& filePath);

    /// erase disabled cluster from the vector
    uint removeDisabledClusters();

    /// Get the names of the cluster and reserve of the participation
    std::pair<std::string, ReserveName> reserveParticipationClusterAt(const Area* area,
                                                                      unsigned int index) const;

    /// Get the names of the group and reserve of the participation
    std::pair<std::string, ReserveName> reserveParticipationGroupAt(const Area* area,
                                                                    unsigned int index) const;

    bool saveToFolder(const std::string& folder) const;

    bool saveDataSeriesToFolder(const std::string& folder) const;

    STStorageCluster* findInAll(const std::string& name);

    size_t getClusterIdx(STStorageCluster& cluster) const;

    // returns the number of reserve participations of all clusters
    uint reserveParticipationsCount() const;

    std::vector<STStorageCluster> storagesByIndex;
    void resizeTimeseriesNumbers(unsigned int nbYears);

    /// Number cumulative - constraint
    std::size_t cumulativeConstraintCount() const;
};
} // namespace Antares::Data::ShortTermStorage
