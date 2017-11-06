#pragma once
#include <Engine/LocalTree/Scenario/ProcessComponent.hpp>
#include <Engine/LocalTree/LocalTreeComponent.hpp>

namespace berlioz
{
class Model;

class LocalTreeProcessComponent :
    public Engine::LocalTree::ProcessComponent_T<Model>
{
    COMPONENT_METADATA("2c8039e5-40b9-41c6-9a6c-2cac6019f784")

    public:
      LocalTreeProcessComponent(
        const Id<score::Component>& id,
        ossia::net::node_base& parent,
        berlioz::Model& scenario,
        Engine::LocalTree::DocumentPlugin& doc,
        QObject* parent_obj);

    ~LocalTreeProcessComponent();
};

using LocalTreeProcessComponentFactory = Engine::LocalTree::ProcessComponentFactory_T<LocalTreeProcessComponent>;
}
