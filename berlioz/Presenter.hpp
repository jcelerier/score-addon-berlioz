#pragma once
#include <Process/LayerPresenter.hpp>
#include <Process/ZoomHelper.hpp>
#include <Process/Focus/FocusDispatcher.hpp>
#include <score/model/Identifier.hpp>

namespace berlioz
{
class Model;
class View;
class Presenter final :
    public Process::LayerPresenter
{
  public:
    explicit Presenter(
        const Model& model,
        View* view,
        const Process::ProcessPresenterContext& ctx,
        QObject* parent);

    void setWidth(qreal width) override;
    void setHeight(qreal height) override;

    void putToFront() override;
    void putBehind() override;

    void on_zoomRatioChanged(ZoomRatio) override;

    void parentGeometryChanged() override;

    const Process::ProcessModel& model() const override;
    const Id<Process::ProcessModel>& modelId() const override;

  private:
    const Model& m_model;
    View* m_view{};
};

}
