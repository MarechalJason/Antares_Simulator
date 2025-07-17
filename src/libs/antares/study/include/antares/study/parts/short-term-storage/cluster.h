/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include <filesystem>
#include <list>
#include <memory>
#include <string>

#include <antares/inifile/inifile.h>
#include <antares/study/area/ReserveOpt.h>
#include <antares/study/area/reserveParticipationContainer.h>
#include <antares/study/version.h>

#include "additionalConstraints.h"
#include "properties.h"
#include "series.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageCluster
{
private:
    std::string group_ = "OTHER";

public:
    //! \brief Get the group name string
    std::string getGroup();

    bool enabled() const;

    bool validate(StudyVersion studyVersion) const;

    bool loadFromSection(const IniFile::Section& section);

    bool loadSeries(const std::filesystem::path& folder, StudyVersion studyVersion) const;

    void saveProperties(IniFile& ini) const;

    bool saveSeries(const std::string& path) const;

    std::string id;

    std::shared_ptr<Series> series = std::make_shared<Series>();
    mutable Properties properties;
    std::vector<std::shared_ptr<AdditionalConstraints>> additionalConstraints;

    //! Reserve participation container to store the participation of the cluster in the reserves
    //! and the symmetries
    ReserveOpt<ReserveParticipationContainer<STStorageClusterReserveParticipation>>
      reserveParticipationContainer;
};
} // namespace Antares::Data::ShortTermStorage
