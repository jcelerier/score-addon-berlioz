#pragma once
#include <Engine/Executor/ProcessComponent.hpp>
#include <ossia/dataflow/node_process.hpp>
namespace Device
{
class DeviceList;
}
namespace berlioz
{
class Model;

class ProcessExecutorComponent final :
        public Engine::Execution::ProcessComponent_T<
            berlioz::Model,
            ossia::node_process>
{
        COMPONENT_METADATA("0440205f-0606-4dd3-a0f6-a8f163fc8d94")
    public:
        ProcessExecutorComponent(
                Engine::Execution::IntervalComponent& parentInterval,
                Model& element,
                const Engine::Execution::Context& ctx,
                const Id<score::Component>& id,
                QObject* parent);
};


using ProcessExecutorComponentFactory =
    Engine::Execution::ProcessComponentFactory_T<ProcessExecutorComponent>;

}
