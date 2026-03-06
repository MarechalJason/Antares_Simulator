// Small translation unit that provides shared utilities for decoders.hxx
// We keep the printed-tags cache here so the tree is printed only once per process

#include "decoders.hxx"

#include <unordered_set>

namespace
{
    // single process-wide cache of tags we've already printed
    static std::unordered_set<std::string> g_printed_tags;
}

std::string getBaseTreeOnce(const std::filesystem::path& nodeTagPath)
{
    const std::string tagStr = nodeTagPath.string();
    if (g_printed_tags.find(tagStr) != g_printed_tags.end())
    {
        return std::string();
    }
    g_printed_tags.insert(tagStr);
    return printPathTree(nodeTagPath);
}

