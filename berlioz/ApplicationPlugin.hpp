#pragma once
#include <score/plugins/application/GUIApplicationPlugin.hpp>
#undef slots
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
namespace berlioz
{
class ApplicationPlugin : public score::GUIApplicationPlugin
{
    pybind11::scoped_interpreter m_python{};
    pybind11::module m_module{pybind11::module::import("generateOrchestration")};

  public:
    ApplicationPlugin(const score::GUIApplicationContext& ctx);

    pybind11::module& module();

    ~ApplicationPlugin() override;
};
}
