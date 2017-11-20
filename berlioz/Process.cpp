#include "Process.hpp"
#include <Process/Dataflow/Port.hpp>
namespace berlioz
{

Process::Inlets Model::inlets() const
{
  return {inlet.get()};
}

Process::Outlets Model::outlets() const
{
  return {outlet.get()};
}

QString Model::startAttrib() const
{
  return m_startAttrib;
}

QString Model::endAttrib() const
{
  return m_endAttrib;
}

QString Model::instrus() const
{
  return m_instrus;
}

void Model::setRate(int rate)
{
  if (m_rate == rate)
        return;

      m_rate = rate;
      emit rateChanged(m_rate);
}

void Model::setStartAttrib(QString startAttrib)
{
  if (m_startAttrib == startAttrib)
        return;

      m_startAttrib = startAttrib;
      emit startAttribChanged(m_startAttrib);
}

void Model::setEndAttrib(QString endAttrib)
{
  if (m_endAttrib == endAttrib)
        return;

      m_endAttrib = endAttrib;
      emit endAttribChanged(m_endAttrib);
}

void Model::setInstrus(QString instrus)
{
  if (m_instrus == instrus)
        return;

      m_instrus = instrus;
      emit instrusChanged(m_instrus);
}

void Model::addFile(QString s)
{
  m_files.append(s);
  emit playedFilesChanged();
}

void Model::clearFiles()
{
  m_files.clear();
  emit playedFilesChanged();
}

Model::Model(
    const TimeVal& duration,
    const Id<Process::ProcessModel>& id,
    QObject* parent):
  Process::ProcessModel{duration, id, "berliozProcess", parent}
, inlet{Process::make_inlet(Id<Process::Port>(0), this)}
, outlet{Process::make_outlet(Id<Process::Port>(1), this)}
{
  m_startAttrib = "Brightness";
  m_endAttrib = "Dullness";
  m_instrus = "Brass";

  inlet->type = Process::PortType::Message;
  inlet->setCustomData("Instruments");

  outlet->setPropagate(true);
  outlet->type = Process::PortType::Audio;
  outlet->setCustomData("Audio Out");
  connect(this, &Model::sig_addFile,
          this, &Model::addFile, Qt::QueuedConnection);
  metadata().setInstanceName(*this);
}

Model::Model(
    const Model& source,
    const Id<Process::ProcessModel>& id,
    QObject* parent):
  Process::ProcessModel{source, id, "berliozProcess", parent}
, inlet{Process::clone_inlet(*source.inlet, this)}
, outlet{Process::clone_outlet(*source.outlet, this)}
, m_startAttrib{source.m_startAttrib}
, m_endAttrib{source.m_endAttrib}
, m_instrus{source.m_instrus}
{
  connect(this, &Model::sig_addFile,
          this, &Model::addFile, Qt::QueuedConnection);
}

Model::~Model()
{

}

int Model::rate() const
{
  return m_rate;
}

QString Model::prettyName() const
{
  return tr("Berlioz");
}

void Model::startExecution()
{
}

void Model::stopExecution()
{
}

void Model::reset()
{
}

void Model::setDurationAndScale(const TimeVal& newDuration)
{
}

void Model::setDurationAndGrow(const TimeVal& newDuration)
{
}

void Model::setDurationAndShrink(const TimeVal& newDuration)
{
}

}
template <>
void DataStreamReader::read(
    const berlioz::Model& proc)
{
  m_stream << proc.m_rate
           << *proc.inlet << *proc.outlet
           << proc.m_startAttrib << proc.m_endAttrib << proc.m_instrus;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(
    berlioz::Model& proc)
{
  m_stream >> proc.m_rate;
  proc.inlet = Process::make_inlet(*this, &proc);
  proc.outlet = Process::make_outlet(*this, &proc);
  m_stream >> proc.m_startAttrib >> proc.m_endAttrib >> proc.m_instrus;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(
    const berlioz::Model& proc)
{
  obj["Rate"] = proc.rate();
  obj["Inlet"] = toJsonObject(*proc.inlet);
  obj["Outlet"] = toJsonObject(*proc.outlet);
  obj["StartAttrib"] = proc.m_startAttrib;
  obj["EndAttrib"] = proc.m_endAttrib;
  obj["Instrus"] = proc.m_instrus;
}

template <>
void JSONObjectWriter::write(
    berlioz::Model& proc)
{
  proc.m_rate = obj["Rate"].toInt();
  {
    JSONObjectWriter writer{obj["Inlet"].toObject()};
    proc.inlet = Process::make_inlet(writer, &proc);
  }
  {
    JSONObjectWriter writer{obj["Outlet"].toObject()};
    proc.outlet = Process::make_outlet(writer, &proc);
  }
  proc.m_startAttrib = obj["StartAttrib"].toString();
  proc.m_endAttrib = obj["EndAttrib"].toString();
  proc.m_instrus = obj["Instrus"].toString();
}
