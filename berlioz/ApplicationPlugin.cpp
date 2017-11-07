#include "ApplicationPlugin.hpp"
#include <berlioz/Executor.hpp>
namespace berlioz
{

ApplicationPlugin::ApplicationPlugin(
    const score::GUIApplicationContext& ctx):
  score::GUIApplicationPlugin{ctx}
{

  auto& play_action = ctx.actions.action<Actions::Play>();

  connect(
      play_action.action(), &QAction::triggered, this,
      [&](bool b) { must_stop = false; }, Qt::QueuedConnection);

  auto& stop_action = ctx.actions.action<Actions::Stop>();
  connect(
      stop_action.action(), &QAction::triggered, this,
      [&](bool b) { must_stop = true; }, Qt::QueuedConnection);

  thread = std::thread{[=] {
    using namespace pybind11::literals;
    while(running)
    {
      if(!must_stop)
      {
        chord_request cur;
        while(request_chords.try_dequeue(cur)) {
          generateNextChord(cur);
        }
      }
      else
      {
        while(request_chords.pop());
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};
}

pybind11::module&ApplicationPlugin::module()
{
  return m_module;
}

ApplicationPlugin::~ApplicationPlugin()
{
  running = false;
  if(thread.joinable())
    thread.join();

}

const std::pair<const float, std::string>&ApplicationPlugin::closest(const std::map<float, std::string>& map, float pos)
{
  auto low = map.lower_bound(pos);
  if (low == map.end()) {
    return *map.rbegin();
  } else if (low == map.begin()) {
    return *low;
  } else {
    auto prev = low;
    --prev;
    if ((pos - prev->first) < (low->first - pos))
      return *prev;
    else
      return *low;
  }
}

void ApplicationPlugin::do_fade(std::vector<float>& ap)
{
  using namespace std;
  const auto decrement = (1. / 1024);
  double gain = 1.0;

  for(std::size_t j = ap.size() - 1024; j < ap.size(); j++)
  {
    ap[j] *= std::pow(2., gain) - 1.;
    gain -= decrement;
  }
}

void ApplicationPlugin::generateNextChord(chord_request cur)
{
  berlioz_node& node = *cur.node;
  bool is_start = (cur.attribute == node.start_attrib);
  // call python function

  try {
    auto fun = m_module.attr("generate");
    std::string file;

    if(is_start)
    {
      auto t0 = std::chrono::steady_clock::now();
      for(int i = 0; i < 10; i++)
      {
        pybind11::tuple res = fun(cur.attribute, cur.nInstruments, node.instrus).cast<pybind11::tuple>();
        auto attr = res[0].cast<float>();
        if(!node.start_range || (attr > node.start_range->first && attr < node.start_range->second))
        {
          node.start_sounds.insert({attr, res[1].cast<std::string>()});
        }
        auto t1 = std::chrono::steady_clock::now();
        if(t1 - t0 > std::chrono::milliseconds(500))
          break;
      }
      if(!node.start_range)
      {
        node.start_range = std::make_pair(node.start_sounds.begin()->first, node.start_sounds.rbegin()->first);
      }
      auto sr = *node.start_range;
      // value is between 1 and 0.5, we want it between max and min
      float scaled_position = (sr.second - sr.first) * (cur.position - 0.5) / 0.5 + sr.first;
      const auto& cls = closest(node.start_sounds, scaled_position);
      file = cls.second;
    }
    else
    {
      auto t0 = std::chrono::steady_clock::now();
      for(int i = 0; i < 10; i++)
      {
        pybind11::tuple res = fun(cur.attribute, cur.nInstruments, node.instrus).cast<pybind11::tuple>();
        auto attr = res[0].cast<float>();
        if(!node.end_range || (attr > node.end_range->first && attr < node.end_range->second))
        {
          node.end_sounds.insert({attr, res[1].cast<std::string>()});
        }
        auto t1 = std::chrono::steady_clock::now();
        if(t1 - t0 > std::chrono::milliseconds(500))
          break;
      }
      if(!node.end_range)
      {
        node.end_range = std::make_pair(node.end_sounds.begin()->first, node.end_sounds.rbegin()->first);
      }

      auto sr = *node.end_range;
      // value is between 1 and 0.5, we want it between max and min
      float scaled_position = (sr.second - sr.first) * (cur.position - 0.5) / 0.5 + sr.first;
      const auto& cls = closest(node.end_sounds, scaled_position);
      file = cls.second;
    }

    if(!file.empty())
    {
      auto decoded = Media::AudioDecoder::decode_synchronous(QString::fromStdString(file));
      if(decoded)
      {
        auto sndnode = std::make_shared<ossia::sound_node>();
        sndnode->set_upmix(2);
        for(auto& chan : decoded->second)
        {
          auto sz = std::min(chan.size(), node.rate);
          chan.resize(sz);
          do_fade(chan);
        }
        sndnode->set_sound(std::move(decoded->second));
        node.retrieve_files.enqueue({file, sndnode});
      }
    }

  } catch(const std::exception& e)
  {
    qDebug() << "PyBind error: " << e.what();
  }

}

}
