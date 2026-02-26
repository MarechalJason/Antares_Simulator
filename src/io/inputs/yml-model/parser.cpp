// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/parser.h"

#include "antares/io/inputs/yml-model/Library.h"

#include "decoders.hxx"

namespace Antares::IO::Inputs::YmlModel
{
void tagNodes(YAML::Node& root)
{
    for (auto child: root)
    {
        if (auto parentId = root["id"]; parentId.IsDefined() && !parentId.IsNull())
        {
            child.SetTag(parentId.as<std::string>() + "->");
            tagNodes(child);
        }
    }
}

Library Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    tagNodes(root);
    Library library = root["library"].as<Library>();

    return library;
}
} // namespace Antares::IO::Inputs::YmlModel
