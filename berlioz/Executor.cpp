#include "Executor.hpp"

#include <berlioz/ApplicationPlugin.hpp>
#include <berlioz/Process.hpp>
#include <Device/Protocol/DeviceList.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Engine/score2OSSIA.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/dataflow/audio_parameter.hpp>
#include <Media/AudioDecoder.hpp>
#include <berlioz/ApplicationPlugin.hpp>
#include <Engine/Executor/IntervalComponent.hpp>
namespace berlioz
{
ProcessExecutorComponent::ProcessExecutorComponent(
    Engine::Execution::IntervalComponent& parentInterval,
    berlioz::Model& element,
    const Engine::Execution::Context& ctx,
    const Id<score::Component>& id,
    QObject* parent):
  ProcessComponent_T{
    parentInterval, element, ctx, id, "berliozExecutorComponent", parent}
{
  auto dur = parentInterval.OSSIAInterval()->get_nominal_duration();
  auto node = std::make_shared<berlioz_node>(dur, element, ctx.doc.app.guiApplicationPlugin<ApplicationPlugin>());
  node->rate = element.rate();
  node->instrus = element.instrus().toStdString();
  node->start_attrib = element.startAttrib().toStdString();
  node->end_attrib = element.endAttrib().toStdString();

  if(auto dest = Engine::score_to_ossia::makeDestination(ctx.devices.list(), element.inlet->address()))
    node->inputs()[0]->address = &dest->address();
  if(auto dest = Engine::score_to_ossia::makeDestination(ctx.devices.list(), element.outlet->address()))
    node->outputs()[0]->address = &dest->address();

  m_ossia_process = std::make_shared<ossia::node_process>(node);
  ctx.plugin.outlets.insert({element.outlet.get(), std::make_pair(node, node->outputs()[0])});
  ctx.plugin.execGraph->add_node(node);
  element.clearFiles();
}

void berlioz_node::run(ossia::token_request tk, ossia::execution_state& st)
{
  if(tk.date > (last_tick + rate) || tk.date == ossia::Zero)
  {
    last_tick = tk.date;
    chord_request req;

    req.node = shared_from_this();
    req.position = tk.position < 0.5 ? 1 - tk.position : tk.position;
    req.attribute = tk.position < 0.5 ? start_attrib : end_attrib;
    req.nInstruments = ossia::clamp(ossia::convert<int>(inputs()[0]->data.target<ossia::value_port>()->data), 2, 4);
    plug.request_chords.enqueue(req);
  }

  if(cur_node)
  {
    req.date += tk.date - m_prev_date;
    if(req.date < cur_node->duration())
    {
      req.offset = tk.offset;
      cur_node->run(req, st);
      cur_node->set_prev_date(req.date);
      req_prev_date = req.date;
    }
    else
    {
      cur_node = {};
    }
  }

  if(!cur_node)
  {
    std::pair<std::string, std::shared_ptr<ossia::sound_node>> p;
    while(retrieve_files.try_dequeue(p))
    {
      element.sig_addFile(QString::fromStdString(p.first));
      cur_node = p.second;
      req.date = 0;
      req_prev_date = 0;
      cur_node->outputs() = outputs();
    }
  }
}

}
