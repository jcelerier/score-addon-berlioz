#include "LocalTree.hpp"
#include <berlioz/Process.hpp>
#include <Engine/LocalTree/Property.hpp>

namespace berlioz
{
LocalTreeProcessComponent::LocalTreeProcessComponent(
        const Id<score::Component>& id,
        ossia::net::node_base& parent,
        berlioz::Model& proc,
        Engine::LocalTree::DocumentPlugin& sys,
        QObject* parent_obj):
    Engine::LocalTree::ProcessComponent_T<berlioz::Model>{parent, proc, sys, id, "berliozComponent", parent_obj}
{
}

LocalTreeProcessComponent::~LocalTreeProcessComponent()
{

}

}
