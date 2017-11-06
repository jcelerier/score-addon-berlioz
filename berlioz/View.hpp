#pragma once
#include <Process/LayerView.hpp>

namespace berlioz
{
class View final : public Process::LayerView
{
    Q_OBJECT
  public:
    explicit View(QGraphicsItem* parent);
    ~View();

  private:
    void paint_impl(QPainter*) const override;
};
}
