#include "Executor.hpp"

#include <berlioz/Process.hpp>
#include <Device/Protocol/DeviceList.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Engine/score2OSSIA.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/dataflow/audio_parameter.hpp>
#include <Media/AudioDecoder.hpp>
#undef slots
#include <pybind11/embed.h>
namespace berlioz
{
class berlioz_node : public ossia::graph_node
{
  public:
    int64_t rate = 11025;
    ossia::time_value last_tick{0};
    struct chord_request
    {
        std::vector<std::string> instruments;
        std::string attribute;
        int nInstruments{4};


    };
    moodycamel::ReaderWriterQueue<chord_request> request_chords;
    moodycamel::ReaderWriterQueue<std::shared_ptr<ossia::sound_node>> retrieve_files;
    std::thread thread;
    std::atomic_bool running{true};
    ossia::token_request req{};

    std::shared_ptr<ossia::sound_node> cur_node;

    berlioz_node()
    {
      namespace py = pybind11;
      auto op = ossia::make_outlet<ossia::audio_port>();
      m_outlets.push_back(op);
      thread = std::thread{[=] {
        pybind11::scoped_interpreter guard{};
        py::eval_file("/home/jcelerier/generateOrchestration/generateOrchestration.py");
        while(true)
        {
          chord_request cur;
          while(request_chords.try_dequeue(cur)) {
            // call python function
            std::cout << std::flush;
            pybind11::tuple res = py::eval("generateOrchestration(instru=instruments, vattribute='Brightness', nInstruments=4)").cast<pybind11::tuple>();


            auto file = "/home/jcelerier/build-i-score-Desktop-Debug/sounds/generation/9triad._sounds_tuba_staccato_F_F2._sounds_trumpet_sforzando_A_A5._sounds_trombone_sforzando_C_C4.wav";
            auto decoded = Media::AudioDecoder::decode_synchronous(file);
            if(decoded)
            {
              auto node = std::make_shared<ossia::sound_node>();
              node->set_sound(std::move(decoded->second));
              retrieve_files.enqueue(node);
            }
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }};
    }
    ~berlioz_node()
    {
      running = false;
      thread.join();
    }
    void run(ossia::token_request tk, ossia::execution_state& st) override
    {
      if(tk.date > (last_tick + rate))
      {
        last_tick = tk.date;
        request_chords.enqueue({});
      }

      if(cur_node)
      {
        cur_node->outputs() = outputs();
        req.date += tk.date - m_prev_date;
        req.offset = tk.offset;
        cur_node->run(tk, st);
      }

      std::shared_ptr<ossia::sound_node> n;
      while(retrieve_files.try_dequeue(n))
      {
        cur_node = n;
      }
    }
};
ProcessExecutorComponent::ProcessExecutorComponent(
    Engine::Execution::IntervalComponent& parentInterval,
    berlioz::Model& element,
    const Engine::Execution::Context& ctx,
    const Id<score::Component>& id,
    QObject* parent):
  ProcessComponent_T{
    parentInterval, element, ctx, id, "berliozExecutorComponent", parent}
{
  auto node = std::make_shared<berlioz_node>();
  node->rate = element.rate();

  m_ossia_process = std::make_shared<ossia::node_process>(node);

  ctx.plugin.outlets.insert({element.outlet.get(), std::make_pair(node, node->outputs()[0])});
  ctx.plugin.execGraph->add_node(node);
}

}
