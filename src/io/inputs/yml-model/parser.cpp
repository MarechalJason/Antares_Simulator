// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-model/parser.h"

#include "antares/io/inputs/yml-model/Library.h"

#include "decoders.hxx"

namespace Antares::IO::Inputs::YmlModel
{

struct NodeItem
{
    YAML::Node& node;
    std::string name;
};

void tagNodes(NodeItem& nodeItem);

void visitMap(NodeItem& nodeItem)
{
    std::string id;

    if (auto parentId = nodeItem.node["id"]; parentId.IsDefined() && !parentId.IsNull())
    {
        id = parentId.as<std::string>() + "->";
    }
    else
    {
        id = nodeItem.name + "->";
    }
    for (auto it = nodeItem.node.begin(); it != nodeItem.node.end(); ++it)
    {
        YAML::Node child = it->second;

        if (!id.empty())
        {
            child.SetTag(id);
        }
        NodeItem childItem{child, it->first.as<std::string>()};
        tagNodes(childItem);
    }
}

void visitSequence(NodeItem& nodeItem)
{
    std::string id;

    if (auto parentId = nodeItem.node["id"]; parentId.IsDefined() && !parentId.IsNull())
    {
        id = parentId.as<std::string>() + "->";
    }
    else
    {
        id = nodeItem.name + "->";
    }
    for (std::size_t i = 0; i < nodeItem.node.size(); ++i)
    {
        YAML::Node child = nodeItem.node[i];

        if (!id.empty())
        {
            child.SetTag(id);
        }
        NodeItem childItem{child, std::to_string(i)};
        tagNodes(childItem);
    }
}

void tagNodes(NodeItem& nodeItem)
{
    if (!nodeItem.node || nodeItem.node.IsNull())
    {
        return;
    }

    if (nodeItem.node.IsMap())
    {
        visitMap(nodeItem);
    }
    else if (nodeItem.node.IsSequence())
    {
        visitSequence(nodeItem);
    }
}

Library Parser::parse(const std::string& content)
{
    YAML::Node root = YAML::Load(content);
    auto libraryNode = root["library"];
    if (libraryNode.IsDefined() && !libraryNode.IsNull())
    {
        tagNodes(libraryNode);
    }
    Library library = root["library"].as<Library>();

    return library;
}
} // namespace Antares::IO::Inputs::YmlModel
