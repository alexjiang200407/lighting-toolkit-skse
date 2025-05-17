#include "PresetDatabase.h"

using namespace preset;

preset::PresetDatabase::const_iterator preset::PresetDatabase::cbegin() const
{
	return presets.cbegin();
}

preset::PresetDatabase::const_iterator preset::PresetDatabase::cend() const
{
	return presets.cend();
}

std::pair<PresetDatabase::iterator, PresetDatabase::iterator>
	PresetDatabase::GetAllPresetsOfType(PresetTID type) const
{
	std::pair<iterator, iterator> ret;
	ret.first  = presets.lower_bound(type);
	ret.second = presets.upper_bound(type);
	return ret;
}

bool PresetDatabase::IsEnd(const iterator& it) const { return it == presets.end(); }

PresetDatabase::iterator PresetDatabase::Find(PresetID key) const { return presets.find(key); }

void PresetDatabase::Insert(PresetPtr preset) { presets.insert(std::move(preset)); }
