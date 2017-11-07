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
#include <valgrind/callgrind.h>
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
        std::string attribute = "Brightness";
        int nInstruments{2};


    };
    moodycamel::ReaderWriterQueue<chord_request> request_chords;
    moodycamel::ReaderWriterQueue<std::shared_ptr<ossia::sound_node>> retrieve_files;
    std::thread thread;
    std::atomic_bool running{true};
    ossia::token_request req{};
    ossia::time_value req_prev_date{};

    std::shared_ptr<ossia::sound_node> cur_node;

    static pybind11::module& module()
    {
      static pybind11::scoped_interpreter guard{};
      static pybind11::module go = pybind11::module::import("generateOrchestration");
      return go;
    }
    berlioz_node()
    {
      namespace py = pybind11;
      auto op = ossia::make_outlet<ossia::audio_port>();
      m_outlets.push_back(op);
      thread = std::thread{[=] {
        using namespace pybind11::literals;
        while(running)
        {
          chord_request cur;
          while(request_chords.try_dequeue(cur)) {
            // call python function
            std::map<float, std::string> files;

            try {
              auto fun = module().attr("generateBrass");
              for(int i = 0; i < 1; i++)
              {
                pybind11::tuple res = fun(cur.attribute, cur.nInstruments).cast<pybind11::tuple>();
                files.insert({res[0].cast<float>(), res[1].cast<std::string>()});
              }

              auto decoded = Media::AudioDecoder::decode_synchronous(QString::fromStdString(files.begin()->second));
              if(decoded)
              {
                auto node = std::make_shared<ossia::sound_node>();
                node->set_sound(std::move(decoded->second));
                retrieve_files.enqueue(node);
              }
            } catch(const std::exception& e)
            {
              qDebug() << "PyBind error: " << e.what();
            }

          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }};
    }
    ~berlioz_node()
    {
      running = false;
      thread.join();
    }
    void run(ossia::token_request tk, ossia::execution_state& st) override
    {
      using namespace std::chrono;
      using clk = steady_clock;
      if(tk.date > (last_tick + rate) || tk.date == ossia::Zero)
      {
        last_tick = tk.date;
        request_chords.enqueue({});
      }

      if(cur_node)
      {
        cur_node->outputs() = outputs();
        req.date += tk.date - m_prev_date;
        if(req.date < cur_node->duration())
        {
          req.offset = tk.offset;
          //qDebug() << "Runnning with " << req.date << req.offset << req_prev_date;
          cur_node->run(tk, st);
          cur_node->set_prev_date(req.date);
          req_prev_date = req.date;
        }
        else
        {
          cur_node = {};
        }
      }

      if(!cur_node)
      while(retrieve_files.try_dequeue(cur_node))
      {
        qDebug() << "Changing node";
        req.date = 0;
        req_prev_date = 0;
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
