#include "Color.h"
#include "LightingPreset.h"
#include "Preset.h"
#include "PresetDatabase.h"
#include <fstream>

namespace preset
{
	class PresetSerializationControl
	{
	public:
		void Serialize(const PresetDatabase& a_presetDB);
		json PresetToJSON(PresetTID tid, const PresetDatabase::const_iterator& it) const;
		void Deserialize(PresetDatabase& a_presetDB);

	private:
		void RegisterPresets(PresetDatabase& a_presetDB, PresetTID tid, json array) const;

	private:
		static const constexpr char* file = "./Data/SKSE/Plugins/SceneCraftPresets.json";
	};
}
