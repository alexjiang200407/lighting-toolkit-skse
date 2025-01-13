#include "Preset/Preset.h"

using namespace preset;

Preset::Preset(PresetTID type, std::string name) :
	Preset(PresetID(type, uuids::uuid_system_generator{}(), name))
{
}

Preset::Preset(PresetID id):
	id(id)
{
}

bool Preset::operator<(const Preset& rhs) const
{
	return id < rhs.id;
}

std::string preset::Preset::GetSIDAsString() const
{
	return id.GetSIDAsString();
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
	return GetName().c_str();
}

const char* preset::Preset::GetUniqueName() const
{
	return id.GetUniqueName();
}

std::string& preset::Preset::GetName()
{
	return id.GetName();
}

const std::string& preset::Preset::GetName() const
{
	return id.GetName();
}

json preset::Preset::Serialize() const
{
	json buf = json::object();
	buf["sid"] = GetSIDAsString();
	buf["name"] = GetName();
	return buf;
}

PresetID::PresetID(PresetTID tid, PresetSID sid, std::string name) :
	tid(tid), sid(sid), name(name), uniqueName(name + "##" + uuids::to_string(sid)) {}

bool PresetID::operator<(const PresetID& id) const
{
	if (tid != id.tid)
		return tid < id.tid;
	
	if (name != id.name)
		return name < id.name;

	return sid < id.sid;
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

const std::string& preset::PresetID::GetName() const
{
	return name;
}

std::string& preset::PresetID::GetName()
{
	return name;
}

std::string preset::PresetID::GetSIDAsString() const
{
	return uuids::to_string(sid);
}

const char* preset::PresetID::GetUniqueName() const
{
	return uniqueName.c_str();
}
