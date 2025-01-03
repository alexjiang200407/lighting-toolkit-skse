#include "Color.h"
#include "LightingPreset.h"
#include "Preset.h"
#include "PresetDatabase.h"
#include <fstream>


class PresetSerializationControl
{
public:
	void Serialize(const PresetDatabase& presetDB);
	void Deserialize(PresetDatabase& presetDB);

private:
	void RegisterPresets(PresetDatabase& presetDB, PresetTID tid, json array) const;

private:
	static const constexpr char* file = "./Data/SKSE/Plugins/SceneCraftPresets.json";
};
