#include "PresetDatabase.h"
#include "Color.h"
#include "LightingPreset.h"

void PresetDatabase::LoadConfigFromFile()
{
	using FLAGS = LightingPreset::Flags;

	//presets.insert(std::make_unique<Color>(1, "Red", RE::NiColor(1, 0, 0)));
	//presets.insert(std::make_unique<Color>(2, "Green", RE::NiColor(0, 1, 0)));
	//presets.insert(std::make_unique<Color>(3, "Blue", RE::NiColor(0, 0, 1)));

	LightingPreset::LightFlags flags;
	flags.set(FLAGS::kAffectAll, FLAGS::kCastsShadow, FLAGS::kPortalStrict, FLAGS::kNeverFades);

	//presets.insert(new LightingPreset(1, "Dynamic", flags));
}

void PresetDatabase::SaveConfigToFile()
{
}

std::pair<PresetDatabase::iterator, PresetDatabase::iterator> PresetDatabase::GetAllPresetsOfType(PresetTID type) const
{
	assert(type != Preset::configTypeMask);
	std::pair<iterator, iterator> ret;
	ret.first  = presets.lower_bound((PresetID)type << 48);
	ret.second = presets.upper_bound((PresetID)(type + 1) << 48);
	return ret;
}
