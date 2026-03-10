// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "decoders.h"

#include <mutex>
#include <unordered_set>

namespace
{
// Thread-safe accessor for the per-process printed-tags cache.
// Using a function-local static avoids mutable global state.
std::unordered_set<std::string>& printedTags()
{
    static std::unordered_set<std::string> cache;
    return cache;
}

std::mutex& printedTagsMutex()
{
    static std::mutex mtx;
    return mtx;
}
} // namespace

// Implement printPathTree here (moved out of header to a single TU)
std::string printPathTree(const std::filesystem::path& p)
{
    std::string treeStr;
    std::size_t depth = 0;
    for (const auto& part: p)
    {
        if (depth == 0)
        {
            treeStr += part.string();
            treeStr += '\n';
        }
        else
        {
            treeStr += std::string((depth - 1) * 4, ' ');
            // "└── " is a u8 and it does not display correctly
            treeStr += "|__ ";
            treeStr += part.string();
            treeStr += '\n';
        }
        ++depth;
    }
    return treeStr;
}

std::string getBaseTreeOnce(const std::filesystem::path& nodeTagPath)
{
    const std::string tagStr = nodeTagPath.string();
    const std::lock_guard<std::mutex> lock(printedTagsMutex());
    if (!printedTags().insert(tagStr).second)
    {
        return {};
    }
    return printPathTree(nodeTagPath);
}

// Implement convert<Library>::decode in this translation unit to ensure all
// other convert<> specializations (e.g., Model) are available and avoid
// premature template instantiation in other TUs.
namespace YAML
{
bool convert<Antares::IO::Inputs::YmlModel::Library>::decode(
  const Node& node,
  Antares::IO::Inputs::YmlModel::Library& rhs)
{
    rhs.id = node["id"].as<std::string>();
    rhs.description = node["description"].as<std::string>("");
    rhs.port_types = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::PortType>>(
      node["port-types"]);
    rhs.models = node["models"].as<std::vector<Antares::IO::Inputs::YmlModel::Model>>();
    return true;
}
} // namespace YAML
