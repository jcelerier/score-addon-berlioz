#pragma once
#include <berlioz/Process.hpp>
#include <berlioz/Presenter.hpp>
#include <berlioz/View.hpp>
#include <Process/LayerModelPanelProxy.hpp>
#include <Process/GenericProcessFactory.hpp>

namespace berlioz
{
    using LayerFactory = Process::GenericLayerFactory<
    berlioz::Model,
    berlioz::Presenter,
    berlioz::View,
    Process::GraphicsViewLayerPanelProxy>;
}
