// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <fmt/format.h>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "antares/io/inputs/yml-model/Library.h"

#include "yaml-cpp/yaml.h"

inline std::string printPathTree(const std::filesystem::path& p)
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

// getBaseTreeOnce is defined in a single translation unit to avoid multiple
// copies of the printed-tags cache when this header is included in several TUs.
std::string getBaseTreeOnce(const std::filesystem::path& nodeTagPath);

// Implement convert specializations
namespace YAML
{

/**
 * @brief shortened to default construct a value when node is null
 * @tparam T Type to convert the node to
 * @param n node
 * @return Object of type T
 * It's just to simplify repetitive and verbose lines
 * as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(
node["parameters"]) is equivalent to
 node["parameters"].as<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(std::vector<Antares::IO::Inputs::YmlModel::Parameter>())
 */
template<typename T>
inline T as_fallback_default(const Node& n)
{
    return n.as<T>(T());
}

inline void checkMandatoryField(const Node& node, const std::string& nodeName)
{
    if (!node["id"].IsDefined() || node["id"].IsNull())
    {
        std::filesystem::path nodePath(node.Tag());
        throw KeyNotFound(node.Mark(),
                          fmt::format("{} id is mandatory in library\n{}",
                                      nodeName,
                                      printPathTree(nodePath)));
    }
}

// Inserted: helper to get a string field safely from a node
inline std::string getFieldFromNode(const Node& node, const std::string& fieldName)
{
    if (!node[fieldName].IsDefined() || node[fieldName].IsNull())
    {
        return {};
    }
    return node[fieldName].as<std::string>("");
}

// Utility to collect keys and build marked-fields messages for YAML maps
class YmlMapMarker
{
public:
    YmlMapMarker(const Node& node)
        : node_(node), nodeTagPath_(node.Tag())
    {
        // collect keys and line numbers
        for (const auto& entry: node_)
        {
            const Node keyNode = entry.first;
            std::string keyName = keyNode.IsDefined() ? keyNode.as<std::string>() : std::string("<unknown>");
            const auto mark = keyNode.Mark();
            const int line = static_cast<int>(mark.line) + 1;
            actualKeysLine_.emplace(keyName, line);
            actualSet_.insert(keyName);
        }

        // compute depth and indentation
        depthParts_ = static_cast<std::size_t>(std::distance(nodeTagPath_.begin(), nodeTagPath_.end()));
        if (depthParts_ == 0)
        {
            depthParts_ = 1;
        }
        indentSpaces_ = (depthParts_ - 1) * 4;

        baseTree_ = ::getBaseTreeOnce(nodeTagPath_);
    }

    const std::unordered_map<std::string, int>& actualKeysLine() const { return actualKeysLine_; }
    const std::unordered_set<std::string>& actualSet() const { return actualSet_; }
    const std::string& baseTree() const { return baseTree_; }
    std::size_t indentSpaces() const { return indentSpaces_; }

    // build marked tree for unexpected and missing lists
    std::string buildMarkedTreeForUnexpectedAndMissing(const std::vector<std::string>& unexpected,
                                                       const std::vector<std::string>& missing) const
    {
        std::string markedFieldsTree;
        for (const auto& keyName: unexpected)
        {
            const int line = actualKeysLine_.count(keyName) ? actualKeysLine_.at(keyName) : -1;
            markedFieldsTree += std::string(indentSpaces_, ' ');
            markedFieldsTree += "|__X ";
            markedFieldsTree += keyName;
            if (line > 0)
            {
                markedFieldsTree += " at line ";
                markedFieldsTree += std::to_string(line);
            }
            markedFieldsTree += '\n';
        }
        for (const auto& keyName: missing)
        {
            markedFieldsTree += std::string(indentSpaces_, ' ');
            markedFieldsTree += "|__? ";
            markedFieldsTree += keyName;
            markedFieldsTree += " (missing)\n";
        }
        return markedFieldsTree;
    }

    // build marked tree that lists all present keys with their lines (used when node.size()>expected)
    std::string buildMarkedTreeAllPresent() const
    {
        std::string markedFieldsTree;
        for (const auto& pair_kv: actualKeysLine_)
        {
            markedFieldsTree += std::string(indentSpaces_, ' ');
            markedFieldsTree += "|__X ";
            markedFieldsTree += pair_kv.first;
            markedFieldsTree += " at line ";
            markedFieldsTree += std::to_string(pair_kv.second);
            markedFieldsTree += '\n';
        }
        return markedFieldsTree;
    }

private:
    const Node& node_;
    std::filesystem::path nodeTagPath_;
    std::unordered_map<std::string, int> actualKeysLine_;
    std::unordered_set<std::string> actualSet_;
    std::size_t depthParts_ = 1;
    std::size_t indentSpaces_ = 0;
    std::string baseTree_;
};

inline bool isValidMap(const Node& node,
                       const unsigned& nbFields,
                       const std::vector<std::string>& allowedFields = {})
{
    if (!node.IsMap())
    {
        return false;
    }

    YmlMapMarker marker(node);

    std::unordered_set<std::string> allowedSet;
    for (const auto& f: allowedFields)
    {
        allowedSet.insert(f);
    }

    // If allowedFields provided, check equality of key sets
    if (!allowedSet.empty())
    {
        if (marker.actualSet() == allowedSet)
        {
            return true;
        }

        // compute unexpected = actual - allowed
        std::vector<std::string> unexpected;
        for (const auto& actual_field_local: marker.actualSet())
        {
            if (allowedSet.find(actual_field_local) == allowedSet.end())
            {
                unexpected.push_back(actual_field_local);
            }
        }

        // compute missing = allowed - actual
        std::vector<std::string> missing;
        for (const auto& allowed_field_local: allowedSet)
        {
            if (marker.actualSet().find(allowed_field_local) == marker.actualSet().end())
            {
                missing.push_back(allowed_field_local);
            }
        }

        const std::string markedFieldsTree = marker.buildMarkedTreeForUnexpectedAndMissing(unexpected, missing);

        throw KeyNotFound(
          node.Mark(),
          fmt::format("Unexpected or missing field(s) (expected {} field(s)).\n{}{}",
                      allowedSet.size(),
                      marker.baseTree(),
                      markedFieldsTree));
    }

    // No allowedFields given: use size-based check
    if (node.size() == nbFields)
    {
        return true;
    }

    const std::string& baseTree = marker.baseTree();

    std::string markedFieldsTree;
    if (node.size() > nbFields)
    {
        markedFieldsTree = marker.buildMarkedTreeAllPresent();

        throw KeyNotFound(
          node.Mark(),
          fmt::format("Unexpected field(s) found (expected {} field(s), got {}).\n{}{}",
                      nbFields,
                      node.size(),
                      baseTree,
                      markedFieldsTree));
    }
    else // node.size() < nbFields
    {
        markedFieldsTree += std::string(marker.indentSpaces(), ' ');
        markedFieldsTree += "|__? missing field(s) (expected ";
        markedFieldsTree += std::to_string(nbFields);
        markedFieldsTree += ")\n";

        throw KeyNotFound(node.Mark(),
                          fmt::format("Missing field(s) (expected {} field(s), got {}).\n{}{}",
                                      nbFields,
                                      node.size(),
                                      baseTree,
                                      markedFieldsTree));
    }
}

static constexpr unsigned expectedNbFields = 3;

template<>
struct convert<Antares::IO::Inputs::YmlModel::PortType>
{
    static bool convertAreaConnectionFields(const Node& node,
                                            Antares::IO::Inputs::YmlModel::PortType& rhs)
    {
        auto area_conn_node = node["area-connection"];
        if (!area_conn_node.IsDefined())
        {
            return true;
        }

        if (!area_conn_node.IsMap())
        {
            return false;
        }

        // If map contains unexpected number of fields, ensure required fields exist (even if empty)
        if (!isValidMap(area_conn_node,
                        expectedNbFields,
                        std::vector<std::string>{"injection-to-balance",
                                                 "spillage-bound",
                                                 "unsupplied-energy-bound"}))
        {
            const Node injectionNode = area_conn_node["injection-to-balance"];
            if (!injectionNode.IsDefined())
            {
                throw KeyNotFound(
                  area_conn_node.Mark(),
                  fmt::format("{} field must be specified even if it has not a value.\n{}",
                              "injection-to-balance",
                              ::getBaseTreeOnce(std::filesystem::path(area_conn_node.Tag()))));
            }
            const Node spillageNode = area_conn_node["spillage-bound"];
            if (!spillageNode.IsDefined())
            {
                throw KeyNotFound(
                  area_conn_node.Mark(),
                  fmt::format("{} field must be specified even if it has not a value.\n{}",
                              "spillage-bound",
                              ::getBaseTreeOnce(std::filesystem::path(area_conn_node.Tag()))));
            }
            const Node unsuppliedNode = area_conn_node["unsupplied-energy-bound"];
            if (!unsuppliedNode.IsDefined())
            {
                throw KeyNotFound(
                  area_conn_node.Mark(),
                  fmt::format("{} field must be specified even if it has not a value.\n{}",
                              "unsupplied-energy-bound",
                              ::getBaseTreeOnce(std::filesystem::path(area_conn_node.Tag()))));
            }
        }

        rhs.area_connection.inject_to_balance = getFieldFromNode(area_conn_node,
                                                                 "injection-to-balance");
        rhs.area_connection.spillage_bound = getFieldFromNode(area_conn_node, "spillage-bound");
        rhs.area_connection.unsupplied_energy_bound = getFieldFromNode(area_conn_node,
                                                                       "unsupplied-energy-bound");
        return true;
    }

    static bool convertThermalCapacityField(const Node& node,
                                            Antares::IO::Inputs::YmlModel::PortType& rhs)
    {
        auto child_node = node["thermal-capacity-connection"];
        if (!child_node.IsDefined())
        {
            return true;
        }

        const unsigned expectedNbFields_local = 1;
        if (!isValidMap(child_node, expectedNbFields_local, std::vector<std::string>{"capacity-field"}))
        {
            return false;
        }

        rhs.thermal_capacity_connection_field = getFieldFromNode(child_node, "capacity-field");
        return true;
    }

    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::PortType& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>("");
        for (const auto& field: node["fields"])
        {
            rhs.fields.push_back(field["id"].as<std::string>());
        }
        if (!convertThermalCapacityField(node, rhs))
        {
            return false;
        }
        if (!convertAreaConnectionFields(node, rhs))
        {
            return false;
        }

        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Library>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Library& rhs);
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Parameter>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Parameter& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        checkMandatoryField(node, "parameter");
        rhs.id = node["id"].as<std::string>();
        rhs.time_dependent = node["time-dependent"].as<bool>(true);
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::ValueType>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::ValueType& rhs)
    {
        if (!node.IsScalar())
        {
            return false;
        }
        const auto value = node.as<std::string>();
        if (value == "continuous")
        {
            rhs = Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS;
        }
        else if (value == "integer")
        {
            rhs = Antares::IO::Inputs::YmlModel::ValueType::INTEGER;
        }
        else if (value == "boolean")
        {
            rhs = Antares::IO::Inputs::YmlModel::ValueType::BOOL;
        }
        else
        {
            return false;
        }
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Variable>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Variable& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        checkMandatoryField(node, "variable");
        rhs.id = node["id"].as<std::string>();
        rhs.lower_bound = node["lower-bound"].as<std::string>("");
        rhs.upper_bound = node["upper-bound"].as<std::string>("");
        rhs.variable_type = node["variable-type"].as<Antares::IO::Inputs::YmlModel::ValueType>(
          Antares::IO::Inputs::YmlModel::ValueType::CONTINUOUS);
        rhs.time_dependent = node["time-dependent"].as<bool>(true);
        rhs.scenario_dependent = node["scenario-dependent"].as<bool>(true);
        rhs.location = node["location"].as<std::string>("subproblems");
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Port>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Port& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        checkMandatoryField(node, "Port");
        rhs.id = node["id"].as<std::string>();
        rhs.type = node["type"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::PortFieldDefinition>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::PortFieldDefinition& rhs)
    {
        if (!node.IsMap())
        {
            // port field definition not mandatory
            return false;
        }
        rhs.port = node["port"].as<std::string>();
        rhs.field = node["field"].as<std::string>();
        rhs.definition = node["definition"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Constraint>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Constraint& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        checkMandatoryField(node, "constraint");
        rhs.id = node["id"].as<std::string>();
        rhs.expression = node["expression"].as<std::string>();
        rhs.location = node["location"].as<std::string>("subproblems");
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::ExtraOutput>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::ExtraOutput& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        checkMandatoryField(node, "extra-output");
        rhs.id = node["id"].as<std::string>();
        rhs.expression = node["expression"].as<std::string>();
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Objective>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Objective& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        checkMandatoryField(node, "objective");
        rhs.id = node["id"].as<std::string>();
        rhs.expression = node["expression"].as<std::string>();
        rhs.location = node["location"].as<std::string>("subproblems");
        return true;
    }
};

template<>
struct convert<Antares::IO::Inputs::YmlModel::Model>
{
    static bool decode(const Node& node, Antares::IO::Inputs::YmlModel::Model& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        checkMandatoryField(node, "model");
        rhs.id = node["id"].as<std::string>();
        rhs.description = node["description"].as<std::string>("");
        rhs.parameters = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Parameter>>(node["parameters"]);
        rhs.variables = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Variable>>(node["variables"]);
        rhs.ports = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Port>>(node["ports"]);
        rhs.port_field_definitions = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::PortFieldDefinition>>(node["port-field-definitions"]);
        rhs.constraints = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Constraint>>(node["constraints"]);
        rhs.binding_constraints = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Constraint>>(node["binding-constraints"]);
        rhs.objectives = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::Objective>>(node["objective-contributions"]);
        rhs.extra_outputs = as_fallback_default<std::vector<Antares::IO::Inputs::YmlModel::ExtraOutput>>(node["extra-outputs"]);
        return true;
    }
};
} // namespace YAML
