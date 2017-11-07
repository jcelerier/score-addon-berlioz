#pragma once
#include <Process/Process.hpp>
#include <Process/GenericProcessFactory.hpp>
#include <berlioz/Metadata.hpp>

namespace berlioz
{
class berlioz_node;
struct chord_request
{
    std::shared_ptr<berlioz_node> node;
    std::string instrus;
    std::string attribute = "Brightness";
    int nInstruments{2};
    double position{};
};

class Model final
    : public Process::ProcessModel
{
    SCORE_SERIALIZE_FRIENDS
    PROCESS_METADATA_IMPL(berlioz::Model)
    Q_OBJECT
    Q_PROPERTY(int rate READ rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(QString startAttrib READ startAttrib WRITE setStartAttrib NOTIFY startAttribChanged)
    Q_PROPERTY(QString endAttrib READ endAttrib WRITE setEndAttrib NOTIFY endAttribChanged)
    Q_PROPERTY(QString instrus READ instrus WRITE setInstrus NOTIFY instrusChanged)
    public:
      Model(
        const TimeVal& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent);

    template<typename Impl>
    Model(Impl& vis, QObject* parent) :
      Process::ProcessModel{vis, parent}
    {
      vis.writeTo(*this);
      connect(this, &Model::sig_addFile,
              this, &Model::addFile, Qt::QueuedConnection);
    }

    ~Model();

    int rate() const;
    QString startAttrib() const;
    QString endAttrib() const;
    QString instrus() const;

    std::vector<Process::Port*> inlets() const override;
    std::vector<Process::Port*> outlets() const override;
    std::unique_ptr<Process::Port> inlet;
    std::unique_ptr<Process::Port> outlet;

    const QStringList& playedFiles() const { return m_files; }
    QStringList& playedFiles() { return m_files; }
  public slots:
    void setRate(int rate);
    void setStartAttrib(QString startAttrib);
    void setEndAttrib(QString endAttrib);
    void setInstrus(QString instrus);
    void addFile(QString);
    void clearFiles();

  signals:
    void sig_addFile(QString rate);
    void rateChanged(int rate);
    void startAttribChanged(QString startAttrib);
    void endAttribChanged(QString endAttrib);
    void instrusChanged(QString instrus);
    void playedFilesChanged();

  private:
    Model(
        const Model& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent);

    QString prettyName() const override;
    void startExecution() override;
    void stopExecution() override;
    void reset() override;

    void setDurationAndScale(const TimeVal& newDuration) override;
    void setDurationAndGrow(const TimeVal& newDuration) override;
    void setDurationAndShrink(const TimeVal& newDuration) override;

    int m_rate{44100};
    QString m_startAttrib;
    QString m_endAttrib;
    QString m_instrus;
    QStringList m_files;
};

using ProcessFactory = Process::GenericProcessModelFactory<berlioz::Model>;
}
