#pragma once
#include <berlioz/Process.hpp>
#include <berlioz/Presenter.hpp>
#include <berlioz/Inspector.hpp>
#include <berlioz/View.hpp>
#include <Process/LayerModelPanelProxy.hpp>
#include <Process/GenericProcessFactory.hpp>
#include <Process/WidgetLayer/WidgetProcessFactory.hpp>

namespace berlioz
{
using LayerFactory = WidgetLayer::LayerFactory<berlioz::Model, berlioz::InspectorWidget>;

}
