#pragma once
#include <berlioz/Process.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/command/PropertyCommand.hpp>
#include <berlioz/CommandFactory.hpp>

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

class SetStartAttribute final : public score::PropertyCommand
{
      SCORE_COMMAND_DECL(
        CommandFactoryName(), SetStartAttribute, "Set start attribute")
  public:
    SetStartAttribute(
        const Model& event,
        QString newval): score::PropertyCommand{event, "startAttrib",
                                                QVariant::fromValue(newval)}
    {

    }
};

class SetEndAttribute final : public score::PropertyCommand
{
      SCORE_COMMAND_DECL(
        CommandFactoryName(), SetEndAttribute, "Set end attribute")
  public:
    SetEndAttribute(
        const Model& event,
        QString newval): score::PropertyCommand{event, "endAttrib",
                                                QVariant::fromValue(newval)}
    {

    }
};


class SetInstrus final : public score::PropertyCommand
{
    SCORE_COMMAND_DECL(
        CommandFactoryName(), SetInstrus, "Set instrus")
    public:
      SetInstrus(
        const Model& event,
        QString newval): score::PropertyCommand{event, "instrus",
          QVariant::fromValue(newval)}
{

}
};


class SetRate final : public score::PropertyCommand
{
    SCORE_COMMAND_DECL(
        CommandFactoryName(), SetRate, "Set rate")
    public:
      SetRate(
        const Model& event,
        int newval): score::PropertyCommand{event, "rate",
          QVariant::fromValue(newval)}
{

}
};
}
