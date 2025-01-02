#include "Preset.h"

Preset::Preset(PresetTID type, PresetID objID, std::string name) :
	id(objID | (PresetID(type) << 48)), name(name)
{
	assert(objID <= PresetID(UINT64_MAX ^ configTypeMask));
}

bool Preset::operator<(const Preset& rhs) const
{
	return id < rhs.id;
}

PresetID Preset::GetID() const
{
	return id;
}

const char* Preset::GetSelectionName() const
{
	return name.c_str();
}
