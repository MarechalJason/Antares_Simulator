// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>

#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

#include "parser.h"

namespace Antares::IO::Inputs::SystemConverter
{

// Specific exception types expected by tests
struct TwoFieldsOfSameRole: public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

struct ConnectingPortToItSelf: public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

ModelerStudy::SystemModel::System convert(
  const YmlSystem::System& ymlSystem,
  const std::vector<ModelerStudy::SystemModel::Library>& libraries);

} // namespace Antares::IO::Inputs::SystemConverter
