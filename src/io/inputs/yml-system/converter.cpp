// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-system/converter.h"

#include <algorithm>
#include <functional>
#include <sstream>

#include "antares/exception/Result.hpp"
#include "antares/io/inputs/yml-system/system.h"
#include "antares/logs/logs.h"
#include "antares/study/system-model/connection.h"
#include "antares/study/system-model/system.h"

using namespace Antares::ModelerStudy;
using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::IO::Inputs::SystemConverter
{

namespace
{

using SplitResult = std::pair<std::string, std::string>;

Antares::Result<SplitResult> splitLibraryModelString(const std::string& s)
{
    size_t pos = s.find('.');
    if (pos == std::string::npos)
    {
        return Antares::Result<SplitResult>::err(
          Antares::Error::InputError::invalidValue(s, "missing '.' separator"));
    }

    std::string library = s.substr(0, pos);
    std::string model = s.substr(pos + 1);
    return Antares::Result<SplitResult>::ok({std::move(library), std::move(model)});
}

Antares::Result<std::reference_wrapper<const Model>> getModel(const std::vector<Library>& libraries,
                                                              const std::string& libraryId,
                                                              const std::string& modelId)
{
    auto lib = std::ranges::find_if(libraries,
                                    [&libraryId](const auto& l) { return l.Id() == libraryId; });
    if (lib == libraries.end())
    {
        return Antares::Result<std::reference_wrapper<const Model>>::err(
          Antares::Error::InputError::notFound("library", libraryId));
    }

    auto search = lib->Models().find(modelId);
    if (search == lib->Models().end())
    {
        return Antares::Result<std::reference_wrapper<const Model>>::err(
          Antares::Error::InputError::notFound("model", modelId));
    }

    return Antares::Result<std::reference_wrapper<const Model>>::ok(std::ref(search->second));
}

Antares::Result<Component> createComponent(const YmlSystem::Component& c,
                                           const std::vector<Library>& libraries,
                                           unsigned int componentIndex)
{
    auto splitResult = splitLibraryModelString(c.model);
    if (!splitResult)
    {
        return Antares::Result<Component>::err(splitResult.error());
    }

    auto modelResult = getModel(libraries, splitResult->first, splitResult->second);
    if (!modelResult)
    {
        return Antares::Result<Component>::err(modelResult.error());
    }
    const Model& model = modelResult->get();

    ComponentBuilder component_builder;

    std::map<std::string, ParameterTypeAndValue> parameters;
    for (const auto& [id, time_dependent, scenario_dependent, value]: c.parameters)
    {
        parameters.try_emplace(
          id,
          ParameterTypeAndValue{.id = id,
                                .type = Optimisation::variability(time_dependent,
                                                                  scenario_dependent),
                                .value = value});
    }

    auto component = component_builder.withId(c.id)
                       .withIndex(componentIndex)
                       .withModel(&model)
                       .withScenarioGroupId(c.scenarioGroup)
                       .withParameterValues(parameters)
                       .build();
    return Antares::Result<Component>::ok(std::move(component));
}

Antares::Result<std::reference_wrapper<Component>> findComponent(const std::string& id,
                                                                 std::vector<Component>& components)
{
    const auto it = std::ranges::find_if(components,
                                         [&id](const Component& c) { return c.Id() == id; });
    if (it == components.end())
    {
        return Antares::Result<std::reference_wrapper<Component>>::err(
          Antares::Error::InputError::notFound("component", id));
    }
    return Antares::Result<std::reference_wrapper<Component>>::ok(std::ref(*it));
}

Antares::Result<void> checkPortSelfConnection(const std::string& firstComponentId,
                                              const std::string& firstPortId,
                                              const std::string& secondComponentId,
                                              const std::string& secondPortId)
{
    if (firstComponentId == secondComponentId && firstPortId == secondPortId)
    {
        return Antares::Result<void>::err(Antares::Error::InputError::validationError(
          fmt::format("Cannot connect port '{}' from component '{}' to itself",
                      firstPortId,
                      firstComponentId)));
    }
    return Antares::Result<void>::ok(0);
}

Antares::Result<void> checkPortsType(const Port& firstPort, const Port& secondPort)
{
    if (firstPort.Type() != secondPort.Type())
    {
        return Antares::Result<void>::err(
          Antares::Error::InputError::typeMismatch(firstPort.Type().Id(), secondPort.Type().Id()));
    }
    return Antares::Result<void>::ok(0);
}

FieldRole exposeFieldRole(const std::string& portId,
                          const std::string& field,
                          const PortFieldMap& portFieldDefinitions)
{
    const auto& it = portFieldDefinitions.find(PortFieldKey{.portId = portId, .fieldId = field});
    if (it == portFieldDefinitions.end())
    {
        return FieldRole::Receiver;
    }
    return FieldRole::Sender;
}

Antares::Result<std::pair<PortFieldsRole, PortFieldsRole>> resolveFieldsRole(
  const Component& firstComponent,
  const Port& firstPort,
  const Component& secondComponent,
  const Port& secondPort)
{
    PortFieldsRole firstPortFieldsRole;
    PortFieldsRole secondPortFieldsRole;

    const auto& firstPortDefs = firstComponent.getModel()->PortFieldDefinitions();
    const auto& secondPortDefs = secondComponent.getModel()->PortFieldDefinitions();
    for (const auto& field: firstPort.Type().Fields())
    {
        const auto firstPortFieldRole = exposeFieldRole(firstPort.Id(), field.Id(), firstPortDefs);
        const auto secondPortFieldRole = exposeFieldRole(secondPort.Id(),
                                                         field.Id(),
                                                         secondPortDefs);

        if (firstPortFieldRole == secondPortFieldRole)
        {
            return Antares::Result<std::pair<PortFieldsRole, PortFieldsRole>>::err(
              Antares::Error::InputError::validationError(
                fmt::format("Field '{}' has same role in both ports '{}' and '{}'",
                            field.Id(),
                            firstPort.Id(),
                            secondPort.Id())));
        }
        firstPortFieldsRole.emplace(field, firstPortFieldRole);
        secondPortFieldsRole.emplace(field, secondPortFieldRole);
    }
    return Antares::Result<std::pair<PortFieldsRole, PortFieldsRole>>::ok(
      {std::move(firstPortFieldsRole), std::move(secondPortFieldsRole)});
}

Antares::Result<void> connectComponents(const YmlSystem::Connection& connection,
                                        std::vector<Component>& components)
{
    const auto& firstComponentId = connection.firstEntry.componentId;
    const auto& firstPortId = connection.firstEntry.portId;
    const auto& secondComponentId = connection.secondEntry.componentId;
    const auto& secondPortId = connection.secondEntry.portId;

    auto selfCheck = checkPortSelfConnection(firstComponentId,
                                             firstPortId,
                                             secondComponentId,
                                             secondPortId);
    if (!selfCheck)
    {
        return selfCheck;
    }

    auto firstCompResult = findComponent(firstComponentId, components);
    if (!firstCompResult)
    {
        return Antares::Result<void>::err(firstCompResult.error());
    }
    Component& firstComponent = firstCompResult->get();

    auto secondCompResult = findComponent(secondComponentId, components);
    if (!secondCompResult)
    {
        return Antares::Result<void>::err(secondCompResult.error());
    }
    Component& secondComponent = secondCompResult->get();

    const auto& firstPort = firstComponent.findPort(firstPortId, "");
    const auto& secondPort = secondComponent.findPort(secondPortId, "");
    auto fieldsResult = resolveFieldsRole(firstComponent, firstPort, secondComponent, secondPort);
    if (!fieldsResult)
    {
        return Antares::Result<void>::err(fieldsResult.error());
    }
    const auto& [firstPortFieldsRole, secondPortFieldsRole] = fieldsResult.get();

    firstComponent.addComponentConnection(firstPort.Id(),
                                          ConnectionEnd(&secondComponent, &secondPort));
    secondComponent.addComponentConnection(secondPort.Id(),
                                           ConnectionEnd(&firstComponent, &firstPort));

    return Antares::Result<void>::ok(0);
}

Antares::Result<void> connectAreas(const YmlSystem::AreaConnection& connection,
                                   std::vector<Component>& components)
{
    auto componentResult = findComponent(connection.componentId, components);
    if (!componentResult)
    {
        return Antares::Result<void>::err(componentResult.error());
    }
    Component& component = componentResult->get();
    component.addAreaConnection(connection.portId, connection.areaId);
    return Antares::Result<void>::ok(0);
}

Antares::Result<void> connectThermalCapacity(const YmlSystem::ThermalCapacityConnection& connection,
                                             std::vector<Component>& components)
{
    auto componentResult = findComponent(connection.componentId, components);
    if (!componentResult)
    {
        return Antares::Result<void>::err(componentResult.error());
    }
    Component& component = componentResult->get();

    component.addThermalCapacityConnection(connection.portId,
                                           connection.thermalComponent.areaId,
                                           connection.thermalComponent.clusterId);
    return Antares::Result<void>::ok(0);
}

} // anonymous namespace

Antares::Result<ModelerStudy::SystemModel::System> convert(const YmlSystem::System& ymlSystem,
                                                           const std::vector<Library>& libraries)
{
    std::vector<Component> components;
    unsigned int componentIndex = 0;
    for (const auto& c: ymlSystem.components)
    {
        auto it = std::ranges::find_if(std::as_const(components),
                                       [&c](const Component& compo) { return compo.Id() == c.id; });
        if (it != components.end())
        {
            return Antares::Result<ModelerStudy::SystemModel::System>::err(
              Antares::Error::InputError::alreadyExists("component", c.id));
        }
        auto componentResult = createComponent(c, libraries, componentIndex);
        if (!componentResult)
        {
            return Antares::Result<ModelerStudy::SystemModel::System>::err(componentResult.error());
        }
        components.push_back(std::move(componentResult.value()));
        ++componentIndex;
        logs.debug() << "Loaded component `" << c.id << "`";
    }

    for (const auto& connection: ymlSystem.connections)
    {
        auto connResult = connectComponents(connection, components);
        if (!connResult)
        {
            return Antares::Result<ModelerStudy::SystemModel::System>::err(connResult.error());
        }
        logs.debug() << "Loaded connection (component1=`" << connection.firstEntry.componentId
                     << "` component2=`" << connection.secondEntry.componentId << "`)";
    }

    for (const auto& connection: ymlSystem.areaConnections)
    {
        auto connResult = connectAreas(connection, components);
        if (!connResult)
        {
            return Antares::Result<ModelerStudy::SystemModel::System>::err(connResult.error());
        }
        logs.debug() << "Loaded area connection (component=`" << connection.componentId
                     << "` area=`" << connection.areaId << "`)";
    }
    for (const auto& connection: ymlSystem.thermalCapacityConnections)
    {
        auto connResult = connectThermalCapacity(connection, components);
        if (!connResult)
        {
            return Antares::Result<ModelerStudy::SystemModel::System>::err(connResult.error());
        }
        logs.debug() << "Loaded thermal-capacity connection (component=`" << connection.componentId
                     << "` area=`" << connection.thermalComponent.areaId << "` clusterId=`"
                     << connection.thermalComponent.clusterId << "`)";
    }

    SystemBuilder builder;
    return Antares::Result<ModelerStudy::SystemModel::System>::ok(
      builder.withId(ymlSystem.id).withComponents(std::move(components)).build());
}

} // namespace Antares::IO::Inputs::SystemConverter
