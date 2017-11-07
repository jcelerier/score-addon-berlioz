#pragma once
#include <Engine/Executor/ProcessComponent.hpp>
#include <berlioz/Process.hpp>
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/node_process.hpp>
namespace Device
{
class DeviceList;
}
namespace berlioz
{
class Model;
class ApplicationPlugin;
class berlioz_node : public std::enable_shared_from_this<berlioz_node>, public ossia::graph_node
{
  public:
    berlioz::Model& element;
    berlioz::ApplicationPlugin& plug;
    std::size_t rate = 11025;
    ossia::time_value last_tick{0};
    std::string instrus;
    moodycamel::ReaderWriterQueue<std::pair<std::string, std::shared_ptr<ossia::sound_node>>> retrieve_files;
    ossia::token_request req{};
    ossia::time_value req_prev_date{};

    std::shared_ptr<ossia::sound_node> cur_node;

    std::map<float, std::string> start_sounds;
    std::map<float, std::string> end_sounds;
    std::string start_attrib, end_attrib;
    optional<std::pair<float,float>> start_range;
    optional<std::pair<float,float>> end_range;

    berlioz_node(ossia::time_value dur, berlioz::Model& e, berlioz::ApplicationPlugin& ap):
      element{e},
      plug{ap}
    {
      auto ip = ossia::make_inlet<ossia::value_port>();
      m_inlets.push_back(ip);
      auto op = ossia::make_outlet<ossia::audio_port>();
      m_outlets.push_back(op);
    }
    ~berlioz_node()
    {
    }
    void run(ossia::token_request tk, ossia::execution_state& st) override;
};

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
