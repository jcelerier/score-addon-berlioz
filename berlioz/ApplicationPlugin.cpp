#include "ApplicationPlugin.hpp"


namespace berlioz
{

ApplicationPlugin::ApplicationPlugin(
    const score::GUIApplicationContext& ctx):
  score::GUIApplicationPlugin{ctx}
{

}

pybind11::module&ApplicationPlugin::module()
{
  return m_module;
}

ApplicationPlugin::~ApplicationPlugin()
{

}

}
