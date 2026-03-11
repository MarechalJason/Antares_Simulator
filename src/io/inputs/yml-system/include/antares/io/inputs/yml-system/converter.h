// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>
#include <string>

#include <antares/exception/Result.hpp>
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

Antares::Result<ModelerStudy::SystemModel::System> convert(
  const YmlSystem::System& ymlSystem,
  const std::vector<ModelerStudy::SystemModel::Library>& libraries);

template<typename T>
void throwIfError(const Antares::Result<T>& result)
{
    if (!result)
    {
        const auto& err = result.error();
        if (err.kind == Antares::Error::InputErrorKind::ValidationError)
        {
            if (err.message.find("same role") != std::string::npos)
            {
                throw TwoFieldsOfSameRole(err.message);
            }
            if (err.message.find("to itself") != std::string::npos)
            {
                throw ConnectingPortToItSelf(err.message);
            }
        }
        if (err.kind == Antares::Error::InputErrorKind::NotFound
            || err.kind == Antares::Error::InputErrorKind::MissingField)
        {
            throw std::runtime_error(err.message);
        }
        throw std::invalid_argument(err.message);
    }
}

} // namespace Antares::IO::Inputs::SystemConverter
