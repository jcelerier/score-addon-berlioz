#include <berlioz/Presenter.hpp>
#include <berlioz/View.hpp>
#include <berlioz/Process.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>


namespace berlioz
{
Presenter::Presenter(
    const Model& layer,
    View* view,
    const Process::ProcessPresenterContext& ctx,
    QObject* parent):
  Process::LayerPresenter{ctx, parent},
  m_model{layer},
  m_view{view}
{
}

void Presenter::setWidth(qreal val)
{
  m_view->setWidth(val);
}

void Presenter::setHeight(qreal val)
{
  m_view->setHeight(val);
}

void Presenter::putToFront()
{
  m_view->setOpacity(1);
}

void Presenter::putBehind()
{
  m_view->setOpacity(0.2);
}

void Presenter::on_zoomRatioChanged(ZoomRatio)
{
}

void Presenter::parentGeometryChanged()
{
}

const Process::ProcessModel& Presenter::model() const
{
  return m_model;
}

const Id<Process::ProcessModel>& Presenter::modelId() const
{
  return m_model.id();
}
}
