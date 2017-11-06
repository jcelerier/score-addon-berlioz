#include "Inspector.hpp"
#include <score/document/DocumentContext.hpp>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

namespace berlioz
{
InspectorWidget::InspectorWidget(
        const berlioz::Model& object,
        const score::DocumentContext& context,
        QWidget* parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{context.commandStack}
{
    auto lay = new QFormLayout{this};
    lay->addWidget(new QLabel("change me"));
}

InspectorWidget::~InspectorWidget()
{

}
}
