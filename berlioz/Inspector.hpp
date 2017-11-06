#pragma once
#include <berlioz/Process.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>

namespace berlioz
{
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<berlioz::Model>
{
    public:
        explicit InspectorWidget(
                const berlioz::Model& object,
                const score::DocumentContext& context,
                QWidget* parent);
        ~InspectorWidget();

    private:
        CommandDispatcher<> m_dispatcher;
};


class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<Model, InspectorWidget>
{
        SCORE_CONCRETE("ae9a5d2c-3cb7-4106-8219-43b8966e2613")
};
}
