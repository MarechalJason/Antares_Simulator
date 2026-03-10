// Small translation unit that provides shared utilities for decoders.hxx
// We keep the printed-tags cache here so the tree is printed only once per process

#include "decoders.hxx"

#include <unordered_set>

namespace
{
    // single process-wide cache of tags we've already printed
    std::unordered_set<std::string> g_printed_tags;
}

std::string getBaseTreeOnce(const std::filesystem::path& nodeTagPath)
{
    const std::string tagStr = nodeTagPath.string();
    if (g_printed_tags.find(tagStr) != g_printed_tags.end())
    {
        return {};
    }
    g_printed_tags.insert(tagStr);
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
