#include "Preset/Preset.h"

using namespace preset;

Preset::Preset(PresetTID type, std::string name) :
	Preset(PresetID(type, uuids::uuid_system_generator{}()), name)
{
}

Preset::Preset(PresetID id, std::string name):
	id(id), name(name)
{
}

bool Preset::operator<(const Preset& rhs) const
{
	return id < rhs.id;
}

PresetTID preset::Preset::GetTID() const
{
	return id.GetTID();
}

PresetID Preset::GetID() const
{
	return id;
}

const char* Preset::GetSelectionName() const
{
	return name.c_str();
}

PresetID::PresetID(PresetTID tid, PresetSID sid) :
	tid(tid), sid(sid) {}

bool PresetID::operator<(const PresetID& id) const
{
	return tid < id.tid || (tid == id.tid && sid < id.sid);
}

bool PresetID::operator<(const PresetTID& tid) const
{
	return this->tid < tid;
}

PresetTID PresetID::GetTID() const
{
	return tid;
}

bool PresetID::IsNull() const
{
	return sid.is_nil() || tid == kUnassigned;
}
