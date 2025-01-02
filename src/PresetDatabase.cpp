#include "PresetDatabase.h"
#include "Color.h"
#include "LightingPreset.h"

std::pair<PresetDatabase::iterator, PresetDatabase::iterator> PresetDatabase::GetAllPresetsOfType(PresetTID type) const
{
	std::pair<iterator, iterator> ret;
	ret.first  = presets.lower_bound(type);
	ret.second = presets.upper_bound(type);
	return ret;
}

bool PresetDatabase::IsEnd(const iterator& it) const
{
	return it == presets.end();
}

PresetDatabase::iterator PresetDatabase::Find(PresetID key) const
{
	return presets.find(key);
}
