#pragma once
#include <Process/Process.hpp>
#include <Process/GenericProcessFactory.hpp>
#include <berlioz/Metadata.hpp>

namespace berlioz
{
class Model final
    : public Process::ProcessModel
{
    SCORE_SERIALIZE_FRIENDS
    PROCESS_METADATA_IMPL(berlioz::Model)
    Q_OBJECT
    Q_PROPERTY(int rate READ rate WRITE setRate NOTIFY rateChanged)

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
    }

    ~Model();

    int rate() const
    {
      return m_rate;
    }


    std::vector<Process::Port*> inlets() const override;
    std::vector<Process::Port*> outlets() const override;
    std::unique_ptr<Process::Port> outlet;

  public slots:
    void setRate(int rate)
    {
      if (m_rate == rate)
        return;

      m_rate = rate;
      emit rateChanged(m_rate);
    }

  signals:
    void rateChanged(int rate);

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
};

using ProcessFactory = Process::GenericProcessModelFactory<berlioz::Model>;
}
