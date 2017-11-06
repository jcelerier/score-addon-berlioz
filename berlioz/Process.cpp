#include "Process.hpp"
#include <Process/Dataflow/Port.hpp>
namespace berlioz
{

std::vector<Process::Port*> Model::inlets() const
{
  return {};
}

std::vector<Process::Port*> Model::outlets() const
{
  return {outlet.get()};
}

Model::Model(
    const TimeVal& duration,
    const Id<Process::ProcessModel>& id,
    QObject* parent):
  Process::ProcessModel{duration, id, "berliozProcess", parent}
, outlet{std::make_unique<Process::Port>(Id<Process::Port>(0), this)}
{
  outlet->setPropagate(true);
  outlet->outlet = true;
  outlet->type = Process::PortType::Audio;
  metadata().setInstanceName(*this);
}

Model::Model(
    const Model& source,
    const Id<Process::ProcessModel>& id,
    QObject* parent):
  Process::ProcessModel{source, id, "berliozProcess", parent}
, outlet{std::make_unique<Process::Port>(source.outlet->id(), *source.outlet, this)}
{
}

Model::~Model()
{

}

QString Model::prettyName() const
{
  return tr("berlioz Process");
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
  m_stream << proc.m_rate << *proc.outlet;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(
    berlioz::Model& proc)
{
  m_stream >> proc.m_rate;
  proc.outlet = std::make_unique<Process::Port>(*this, &proc);

  checkDelimiter();
}

template <>
void JSONObjectReader::read(
    const berlioz::Model& proc)
{
  obj["Rate"] = proc.rate();
  obj["Outlet"] = toJsonObject(*proc.outlet);
}

template <>
void JSONObjectWriter::write(
    berlioz::Model& proc)
{
  proc.m_rate = obj["Rate"].toInt();
  JSONObjectWriter writer{obj["Outlet"].toObject()};
  proc.outlet = std::make_unique<Process::Port>(writer, &proc);

}
