#include "Inspector.hpp"
#include <score/document/DocumentContext.hpp>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <score/widgets/SignalUtils.hpp>
#include <QListWidget>
#include <QSpinBox>

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

    auto start = new QComboBox{this};
    start->addItems({"Brightness", "Dullness", "Warmth", "Roughness", "Breathiness"});
    start->setCurrentText(object.startAttrib());
    auto end = new QComboBox{this};
    end->addItems({"Brightness", "Dullness", "Warmth", "Roughness", "Breathiness"});
    end->setCurrentText(object.endAttrib());
    auto instrus = new QComboBox{this};
    instrus->addItems({"Brass", "String"});
    instrus->setCurrentText(object.instrus());

    auto dur = new QSpinBox{this};
    dur->setRange(10, 10000);
    dur->setValue(object.rate());
    auto plabel = new QListWidget;

    lay->addRow("Start", start);
    lay->addRow("End", end);
    lay->addRow("Instrus", instrus);
    lay->addRow("Chord length", dur);
    lay->addRow("Played", plabel);

    con(object, &berlioz::Model::startAttribChanged,
        this, [=] (auto s) { if (s != start->currentText()) start->setCurrentText(s); });
    con(object, &berlioz::Model::endAttribChanged,
        this, [=] (auto s) { if (s != end->currentText()) end->setCurrentText(s); });
    con(object, &berlioz::Model::instrusChanged,
        this, [=] (auto s) { if (s != instrus->currentText()) instrus->setCurrentText(s); });
    con(object, &berlioz::Model::rateChanged,
        this, [=] (auto s) { if (s != dur->value()) dur->setValue(s / 44.1); });
    con(object, &berlioz::Model::playedFilesChanged,
        this, [=,&object] {
      plabel->clear();
      plabel->addItems(object.playedFiles());
    });

    connect(start, SignalUtils::QComboBox_currentIndexChanged_int(),
        this, [=,&context,&object] (int x) {
      CommandDispatcher<> disp(context.commandStack);
      if(start->currentText() != object.startAttrib())
        disp.submitCommand<SetStartAttribute>(object, start->currentText());
    });
    connect(end, SignalUtils::QComboBox_currentIndexChanged_int(),
        this, [=,&context,&object] (int x) {
      CommandDispatcher<> disp(context.commandStack);
      if(end->currentText() != object.endAttrib())
        disp.submitCommand<SetEndAttribute>(object, end->currentText());
    });
    connect(instrus, SignalUtils::QComboBox_currentIndexChanged_int(),
            this, [=,&context,&object] (int x) {
      CommandDispatcher<> disp(context.commandStack);
      if(instrus->currentText() != object.instrus())
        disp.submitCommand<SetInstrus>(object, instrus->currentText());
    });
    connect(dur, SignalUtils::QSpinBox_valueChanged_int(),
            this, [=,&context,&object] (int x) {
      CommandDispatcher<> disp(context.commandStack);
      if(dur->value() != object.rate())
        disp.submitCommand<SetRate>(object, x * 44.1);
    });

}

InspectorWidget::~InspectorWidget()
{

}
}
