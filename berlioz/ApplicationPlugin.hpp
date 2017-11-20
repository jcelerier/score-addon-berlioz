#pragma once
#include <score/plugins/application/GUIApplicationPlugin.hpp>

#include <berlioz/Process.hpp>
#include <readerwriterqueue.h>
#include <Device/Protocol/DeviceList.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Engine/score2OSSIA.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/dataflow/audio_parameter.hpp>
#include <Media/AudioDecoder.hpp>
#include <Scenario/Application/ScenarioActions.hpp>
#include <score/actions/ActionManager.hpp>
#include <Engine/Executor/IntervalComponent.hpp>
#include <berlioz/Executor.hpp>
#include <thread>
#include <map>
#include <string>
#include <vector>
#include <utility>
#undef slots
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
namespace berlioz
{
class ApplicationPlugin : public QObject, public score::GUIApplicationPlugin
{
    Q_OBJECT
  public:
    std::atomic<bool> must_stop{false};
    moodycamel::ReaderWriterQueue<chord_request> request_chords;
    ApplicationPlugin(const score::GUIApplicationContext& ctx);

    pybind11::module& module();

    ApplicationPlugin() = delete;
    ApplicationPlugin(const ApplicationPlugin&) = delete;
    ApplicationPlugin(ApplicationPlugin&&) = delete;
    ApplicationPlugin& operator=(const ApplicationPlugin&) = delete;
    ApplicationPlugin& operator=(ApplicationPlugin&&) = delete;
    ~ApplicationPlugin() override;

    const std::pair<const float, sound_data>& closest(const std::map<float, sound_data>& map, float pos);
    void do_fade(std::vector<float>& ap);
    void generateNextChord(chord_request cur);

signals:
    void sig_addChord(std::vector<chord_info>);

public Q_SLOTS:
    void addChord(std::vector<chord_info>);

private:
    std::vector<std::vector<chord_info>> m_playedChords;

    pybind11::scoped_interpreter m_python{};
    pybind11::module m_module{pybind11::module::import("generateOrchestration")};

    std::thread thread;
    std::atomic_bool running{true};

};
}
